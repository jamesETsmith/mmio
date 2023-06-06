#include "mtx_io.h"

size_t find_endline(char* data, size_t data_size, size_t start) {
  size_t end = start;
  while (start < data_size) {
    if (data[end] != '\n') {
      end++;
    } else {
      break;
    }
  }
  return end;
}

void print_time(time_t t_start, const char* msg) {
  double t_total = ((double)(clock() - t_start)) / CLOCKS_PER_SEC;
  printf("Time for %16s: %.6lf (s)\n", msg, t_total);
}

inline int find_chunk_boundaries(char* data, size_t buff_size, size_t* start,
                                 size_t* end, size_t* n_newlines) {
  // Find the new start
  if (omp_get_thread_num() != 0) {
    size_t curr = *start;
    while (curr < buff_size && data[curr] != '\n') {
      curr++;
    }
    if (curr == buff_size || curr + 1 == buff_size) {
      return -1;
    }
    *start = curr + 1;
  }

  // Find the new end
  if (omp_get_thread_num() != omp_get_num_threads() - 1) {
    size_t curr = *end;
    while (curr < buff_size && data[curr] != '\n') {
      curr++;
    }
    if (curr == buff_size) {
      return -2;
    }
    *end = curr + 1;
  }

  // Count new_lines
  *n_newlines = 0;
  for (size_t i = *start; i < *end; i++) {
    *n_newlines += (data[i] == '\n');
  }

  return 0;
}

int mtx_read_parallel(const char* filename, size_t* e_i, size_t* e_o,
                      double* e_w) {
  struct stat file_stats;
  int fd = 0;
  fd = open(filename, O_RDONLY);

  if (fstat(fd, &file_stats) == -1) {
    fprintf(stderr, "Problem getting the stats for the file %s\n", filename);
    exit(-1);
  }

  size_t buff_size = file_stats.st_size;
  printf("Size of file: %lu bytes\n", buff_size);
  char* data;
  data =
      (char*)mmap(NULL, buff_size * sizeof(char), PROT_READ, MAP_SHARED, fd, 0);

  size_t start = 0;
  size_t end = find_endline(data, buff_size, start);

  int i = 0;
  while (data[start] == '%' && i < 5) {
    fwrite(&data[start], 1, end - start, stdout);
    printf("\n");

    start = end + 1;
    end = find_endline(data, buff_size, start);

    i++;
  }

  // print_time(t_setup, "setup");

  //
  // Meta info
  //
  time_t t_meta = clock();

  size_t m = 0, n = 0, nnz = 0;

  fwrite(&data[start], 1, end - start, stdout);
  printf("\n");

  int items_read = sscanf(&data[start], "%lu %lu %lu\n", &m, &n, &nnz);
  printf("items read: %d\n", items_read);
  printf("m: %lu n: %lu nnz: %lu\n", m, n, nnz);
  print_time(t_meta, "meta");

  //
  // Find new lines
  //
  // time_t t_newlines = clock();

  /*
  size_t chunk_start[nthreads]
  size_t chunk_end[nthreads]
  size_t chunk_n_newlines[nthreads]

  for chunk c in data do in parallel
    - t <- thead id
    - walk forward until you reach a new line (set chunk_start[t]) (unless
  you're the first thread)
    - walk end pointer until it reaches a new line (set chunk_end[t])
    - walk from start to end and count new lines (set chunk_n_newlines)
    -

  bool find_chunk_boundaries(data, buff_size, start, end, n_newlines)
  */
  size_t chunk_size = (buff_size - (end + 1)) / omp_get_max_threads();
  size_t* chunk_start = (size_t*)malloc(omp_get_max_threads() * sizeof(size_t));
  size_t* chunk_end = (size_t*)malloc(omp_get_max_threads() * sizeof(size_t));
  size_t* chunk_n_newlines =
      (size_t*)malloc(omp_get_max_threads() * sizeof(size_t));
  size_t* chunk_offsets =
      (size_t*)malloc(omp_get_max_threads() * sizeof(size_t));

  if (chunk_start == NULL || chunk_end == NULL || chunk_n_newlines == NULL ||
      chunk_offsets == NULL) {
    fprintf(stderr, "Something went wrong during allocation\n");
    return -1;
  }

  for (int i = 0; i < omp_get_max_threads(); i++) {
    chunk_start[i] = (end + 1) + i * chunk_size;
    chunk_end[i] = (end + 1) + (i + 1) * chunk_size;
    chunk_n_newlines[i] = 0;

    printf("t = %d    start = %lu   end = %lu\n", i, chunk_start[i],
           chunk_end[i]);
  }
  chunk_end[omp_get_max_threads() - 1] = buff_size;

  // Setup final data structures
  e_i = (size_t*)malloc(nnz * sizeof(size_t));
  e_o = (size_t*)malloc(nnz * sizeof(size_t));
  e_w = (double*)malloc(nnz * sizeof(double));

//
// Parse data
//
#pragma omp parallel
  {
    size_t t_id = omp_get_thread_num();
    find_chunk_boundaries(data, buff_size, &chunk_start[t_id], &chunk_end[t_id],
                          &chunk_n_newlines[t_id]);
    printf("[THREAD %lu]: found %lu new lines\n", t_id, chunk_n_newlines[t_id]);

#pragma omp barrier

#pragma omp master
    {
      size_t check_nnz = 0;
      for (int i = 0; i < omp_get_max_threads(); i++) {
        check_nnz += chunk_n_newlines[i];
      }
      printf("[CHECK]: check_nnz = %lu    real nnz = %lu\n", check_nnz, nnz);
      assert(check_nnz == nnz);

      chunk_offsets[0] = 0;
      for (int i = 1; i < omp_get_max_threads(); i++) {
        chunk_offsets[i] = chunk_n_newlines[i - 1] + chunk_offsets[i - 1];
        printf("Offset for thread %d = %lu\n", i, chunk_offsets[i]);
      }
    }

    // print_time(t_newlines, "newlines");
    // time_t t_parse = clock();

    size_t t_newlines = chunk_n_newlines[t_id];
    size_t t_offset = chunk_offsets[t_id];

    size_t chunk_size = chunk_end[t_id] - chunk_start[t_id];
    char* local_data = (char*)malloc(chunk_size * sizeof(char*));
    char* memcpy_res =
        (char*)memcpy(local_data, &data[chunk_start[t_id]], chunk_size);
    if (memcpy_res != local_data) {
      fprintf(stderr, "Problem with allocating local data to parse\n");
      exit(-1);
    }

    size_t t_start = 0;
    size_t t_end = find_endline(local_data, chunk_size, t_start);

#pragma omp for
    for (size_t i = 0; i < t_newlines; i++) {
      char* line_end;
      e_i[t_offset + i] = strtoul(&local_data[t_start], &line_end, 10);
      e_o[t_offset + i] = strtoul(line_end, &line_end, 10);
      e_w[t_offset + i] = strtod(line_end, &line_end);

      t_start = t_end + 1;
      t_end = find_endline(local_data, chunk_size, t_start);
    }

    free(local_data);

  }  // End of omp parallel region

  // #pragma omp parallel for schedule(static, 64)
  //   for (size_t i = 0; i < nnz; i++) {
  //     // sscanf(&data[line_start[i]], "%lu %lu %lf\n", &e_i[i], &e_o[i],
  //       &e_w[i]);
  //       char* end;
  //       e_i[i] = strtoul(&data[line_start[i]], &end, 10);
  //       e_o[i] = strtoul(end, &end, 10);
  //       e_w[i] = strtod(end, &end);
  //   }

  // print_time(t_parse, "parse");

  printf("First line: i = %lu  j = %lu  val = %lf\n", e_i[0], e_o[0], e_w[0]);
  printf("Last line:  i = %lu  j = %lu  val = %lf\n", e_i[nnz - 1],
         e_o[nnz - 1], e_w[nnz - 1]);

  //
  // Cleanup
  //
  free(e_i);
  free(e_o);
  free(e_w);
  // free(new_lines);
  // free(line_start);
  // free(line_end);
  munmap(data, buff_size);
  close(fd);
  return 0;
}