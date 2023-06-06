#include <assert.h>
#include <fcntl.h>
#include <omp.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <time.h>
#include <unistd.h>

size_t find_endline(char *data, size_t data_size, size_t start);
void print_time(time_t t_start, const char *msg);
int find_chunk_boundaries(char *data, size_t buff_size, size_t *start,
                          size_t *end, size_t *n_newlines);
int mtx_read_parallel(const char *filename, size_t *m, size_t *n, size_t *nnz,
                      size_t *e_i, size_t *e_o, double *e_w);