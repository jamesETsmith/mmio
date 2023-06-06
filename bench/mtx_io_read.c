#include <mtx_io.h>

int main(int argc, char** argv) {
  // time_t t_setup = clock();

  if (argc != 2) {
    fprintf(stderr, "Wrong command line args. Use './io matrix.mtx'");
    exit(1);
  }

  const char* filename = argv[1];
  printf("Parsing %s\n", filename);

  size_t* e_i = NULL;
  size_t* e_o = NULL;
  double* e_w = NULL;
  mtx_read_parallel(filename, e_i, e_o, e_w);
  return 0;
}