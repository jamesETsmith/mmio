#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <time.h>
#include <unistd.h>

//   FILE* fp = fopen(filename, "rb");
//   if (fp == NULL) {
//     fprintf(stderr, "Opening %s failed\n");
//     exit(-1);
//   }

//   int seek_rv = fseek(fp, 0L, SEEK_END);
//   if (seek_rv != 0) {
//     fprintf(stderr, "Seeking end of file failed\n");
//     exit(-1);
//   }

//   size_t buff_size = ftell(fp);
//   if (buff_size = (size_t)(-1)) {
//     fprintf(stderr, "Invalid size\n");
//   }

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

int main(int argc, char** argv) {
  time_t t_setup = clock();

  if (argc != 2) {
    fprintf(stderr, "Wrong command line args. Use './io matrix.mtx'");
    exit(1);
  }

  const char* filename = argv[1];
  printf("Parsing %s\n", filename);

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

  print_time(t_setup, "setup");

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
  time_t t_newlines = clock();
  size_t* line_start;
  size_t* line_end;
  line_start = (size_t*)malloc(nnz * sizeof(size_t));
  line_end = (size_t*)malloc(nnz * sizeof(size_t));

  for (int i = 0; i < nnz; i++) {
    start = end + 1;
    end = find_endline(data, buff_size, start);

    line_start[i] = start;
    line_end[i] = end;
  }

  line_end[nnz - 1] = buff_size;
  print_time(t_newlines, "newlines");

  //
  // Parse data
  //
  time_t t_parse = clock();
  size_t* e_i = (size_t*)malloc(nnz * sizeof(size_t));
  size_t* e_o = (size_t*)malloc(nnz * sizeof(size_t));
  double* e_w = (double*)malloc(nnz * sizeof(double));

#pragma omp parallel for schedule(static, 64)
  for (size_t i = 0; i < nnz; i++) {
    // sscanf(&data[line_start[i]], "%lu %lu %lf\n", &e_i[i], &e_o[i], &e_w[i]);
    char* end;
    e_i[i] = strtoul(&data[line_start[i]], &end, 10);
    e_o[i] = strtoul(end, &end, 10);
    e_w[i] = strtod(end, &end);
  }

  print_time(t_parse, "parse");

  printf("First line: i = %lu  j = %lu  val = %lf\n", e_i[0], e_o[0], e_w[0]);
  printf("Last line:  i = %lu  j = %lu  val = %lf\n", e_i[nnz - 1],
         e_o[nnz - 1], e_w[nnz - 1]);

  //
  // Cleanup
  //
  free(e_i);
  free(e_o);
  free(e_w);
  free(line_start);
  free(line_end);
  munmap(data, buff_size);
  close(fd);
  return 0;
}