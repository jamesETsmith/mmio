#include "mtxio.h"

#include <math.h>
#include <stdbool.h>

inline size_t read_int(char **dd, char *end) {
  char *d = *dd;
  // printf("Starting d = %lu\n", d);
  size_t res = 0;
  while (d < end && (*d < '0' || *d > '9'))
    ++d;

  // Read out digit by digit
  while (d < end && (*d >= '0' && *d <= '9')) {
    res = res * 10 + (*d - '0');
    ++d;
  }
  // printf("Final d =    %lu\n", d);
  *dd = d;
  return res;
}

inline double read_double(char *d, char *end) {
  double res = 0.0;
  while (d < end && !((*d >= '0' && *d <= '9') || *d == 'e' || *d == 'E' ||
                      *d == '-' || *d == '+' || *d == '.')) {
    ++d;
  }
  // Read the size
  bool positive = true;
  if (*d == '-') {
    positive = false;
    ++d;
  } else if (*d == '+')
    ++d;

  // Support a simple form of floating point integers
  // Note: this is not the most accurate or fastest strategy
  // (+-)AAA.BBB(eE)(+-)ZZ.YY
  // Read the 'A' part
  while (d < end && (*d >= '0' && *d <= '9')) {
    res = res * 10. + (double)(*d - '0');
    ++d;
  }
  if (*d == '.') {
    ++d;
    double fraction = 0.;
    size_t fraction_count = 0;
    // Read the 'B' part
    while (d < end && (*d >= '0' && *d <= '9')) {
      fraction = fraction * 10. + (double)(*d - '0');
      ++d;
      ++fraction_count;
    }
    res += fraction / pow(10., fraction_count);
  }
  if (*d == 'e' || *d == 'E') {
    ++d;
    double exp = read_double(d, end);
    res *= pow(10., exp);
  }

  if (!positive)
    res *= -1;
  return res;
}

inline size_t find_endline(char *data, size_t data_size, size_t start) {
  size_t end = start;
  while (end < data_size) {
    if (data[end] != '\n') {
      end++;
    } else {
      break;
    }
  }
  return end;
}

// void print_time(time_t t_start, const char *msg) {
//   double t_total = ((double)(clock() - t_start)) / CLOCKS_PER_SEC;
//   MTXIO_LOG("Time for %16s: %.6lf (s)\n", msg, t_total);
// }

inline int find_chunk_boundaries(char *data, size_t buff_size, size_t *start,
                                 size_t *end, size_t *n_newlines) {
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
  if (omp_get_thread_num() != omp_get_max_threads() - 1) {
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
#pragma omp simd
  for (size_t i = *start; i < *end; i++) {
    *n_newlines += (data[i] == '\n');
  }

  // TODO this feels like sloppy way to handle the final lines that
  // don't terminate with a newline
  if (omp_get_thread_num() == (omp_get_max_threads() - 1)) {
    if (data[*end - 1] != '\n') {
      *n_newlines += 1;
    }
  }

  return 0;
}
/*
PIGO COO algorithm

- Read the three integers (m,n,nnz)
- Read all the edges
  - Move to the appropriate starting/ending points on each thread
  - Iterate through and count new lines
  - (inside omp single): Count up the offsets
  - iterate through again, but parse the data
- Sanity checks
*/

int mtx_read_parallel(const char *filename, size_t *m, size_t *n, size_t *nnz,
                      size_t **e_i_p, size_t **e_o_p, double **e_w_p) {

  //
  // Open file
  //
  struct stat file_stats;
  int fd = 0;
  fd = open(filename, O_RDONLY);

  if (fstat(fd, &file_stats) == -1) {
    fprintf(stderr, "Problem getting the stats for the file %s\n", filename);
    exit(-1);
  }

  size_t buff_size = file_stats.st_size;
  MTXIO_LOG("Size of file: %lu bytes\n", buff_size);
  char *data;
  data = (char *)mmap(NULL, buff_size * sizeof(char), PROT_READ, MAP_SHARED, fd,
                      0);

  //
  // Skip header (TODO actually read header)
  //
  size_t start = 0;
  size_t end = find_endline(data, buff_size, start);

  while (data[start] == '%') {
    start = end + 1;
    end = find_endline(data, buff_size, start);
  }

  //
  // Meta info
  //

  *m = 0, *n = 0, *nnz = 0;
  char *end_p = NULL;
  *m = strtoul(&data[start], &end_p, 10);
  *n = strtoul(end_p, &end_p, 10);
  *nnz = strtoul(end_p, &end_p, 10);
  MTXIO_LOG("m: %lu n: %lu nnz: %lu\n", *m, *n, *nnz);

  if (*nnz < omp_get_max_threads()) {
    omp_set_num_threads(1);
    fprintf(stderr,
            "[WARNING]: Number of threads greater than number of non-zero "
            "elements, reducing the number of threads.\n");
  }

  //
  // Find new lines
  //

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
  // clang-format off
  size_t chunk_size = (buff_size - (end + 1)) / omp_get_max_threads();
  size_t *chunk_start =      (size_t *)malloc(omp_get_max_threads() * sizeof(size_t));
  size_t *chunk_end =        (size_t *)malloc(omp_get_max_threads() * sizeof(size_t));
  size_t *chunk_n_newlines = (size_t *)malloc(omp_get_max_threads() * sizeof(size_t));
  size_t *chunk_offsets =    (size_t *)malloc(omp_get_max_threads() * sizeof(size_t));
  // clang-format on

  if (chunk_start == NULL || chunk_end == NULL || chunk_n_newlines == NULL ||
      chunk_offsets == NULL) {
    fprintf(stderr, "Something went wrong during allocation\n");
    return -1;
  }

  for (int i = 0; i < omp_get_max_threads(); i++) {
    chunk_start[i] = (end + 1) + i * chunk_size;
    chunk_end[i] = (end + 1) + (i + 1) * chunk_size;
    chunk_n_newlines[i] = 0;

    MTXIO_LOG("t = %d    start = %lu   end = %lu\n", i, chunk_start[i],
              chunk_end[i]);
  }
  chunk_end[omp_get_max_threads() - 1] = buff_size;

  // Setup final data structures
  *e_i_p = (size_t *)malloc(*nnz * sizeof(size_t));
  *e_o_p = (size_t *)malloc(*nnz * sizeof(size_t));
  *e_w_p = (double *)malloc(*nnz * sizeof(double));
  size_t *e_i = *e_i_p;
  size_t *e_o = *e_o_p;
  double *e_w = *e_w_p;

//
// Parse data
//
#pragma omp parallel
  {
    size_t t_id = omp_get_thread_num();
    find_chunk_boundaries(data, buff_size, &chunk_start[t_id], &chunk_end[t_id],
                          &chunk_n_newlines[t_id]);
    MTXIO_LOG("[THREAD %lu]: found %lu new lines\n", t_id,
              chunk_n_newlines[t_id]);

#pragma omp barrier

#pragma omp single
    {
      size_t check_nnz = 0;
      for (int i = 0; i < omp_get_max_threads(); i++) {
        check_nnz += chunk_n_newlines[i];
      }
      MTXIO_LOG("[CHECK]: check_nnz = %lu    real nnz = %lu\n", check_nnz,
                *nnz);
      assert(check_nnz == *nnz);

      chunk_offsets[0] = 0;
      MTXIO_LOG("Offset for thread %d = %lu\n", 0, chunk_offsets[0]);

      for (int i = 1; i < omp_get_max_threads(); i++) {
        chunk_offsets[i] = chunk_n_newlines[i - 1] + chunk_offsets[i - 1];
        MTXIO_LOG("Offset for thread %d = %lu\n", i, chunk_offsets[i]);
      }
    } // end omp single

    size_t t_newlines = chunk_n_newlines[t_id];
    size_t t_offset = chunk_offsets[t_id];
    // size_t chunk_size = chunk_end[t_id] - chunk_start[t_id];

    size_t t_start = chunk_start[t_id];
    size_t t_end = find_endline(data, chunk_end[t_id], t_start);

    for (size_t i = 0; i < t_newlines; i++) {
      char *line_end = &data[t_start];

      e_i[t_offset + i] = strtoul(&data[t_start], &line_end, 10);
      e_o[t_offset + i] = strtoul(line_end, &line_end, 10);
      // e_w[t_offset + i] = strtod(line_end, &line_end);

      // e_i[t_offset + i] = read_int(&line_end, &local_data[t_end]);
      // e_o[t_offset + i] = read_int(&line_end, &local_data[t_end]);
      e_w[t_offset + i] = read_double(line_end, &data[t_end]);

      t_start = t_end + 1;
      t_end = find_endline(data, chunk_end[t_id], t_start);
    }

  } // End of omp parallel region

  // clang-format off
  MTXIO_LOG("First line:           i = %lu  j = %lu  val = %lf\n", e_i[0], e_o[0], e_w[0]);
  MTXIO_LOG("Second to last line:  i = %lu  j = %lu  val = %lf\n", e_i[*nnz - 2], e_o[*nnz - 2], e_w[*nnz - 2]);
  MTXIO_LOG("Last line:            i = %lu  j = %lu  val = %lf\n", e_i[*nnz - 1], e_o[*nnz - 1], e_w[*nnz - 1]);
  // clang-format on

  //
  // Cleanup
  //
  if (munmap(data, buff_size) != 0) {
    fprintf(stderr, "Problem unmapping file\n");
  }
  if (close(fd)) {
    fprintf(stderr, "Problem closing file\n");
  }
  return 0;
}