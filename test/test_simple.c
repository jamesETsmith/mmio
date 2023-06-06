
#include "mtxio.h"
#include "tau/tau.h"
#include "test_util.h"

TAU_MAIN()

TEST(simple, read) {
  size_t m, n, nnz;
  size_t *e_i;
  size_t *e_o;
  double *e_w;
  char simple_path[1024];
  get_data_path("simple.mtx", simple_path);
  mtx_read_parallel(simple_path, &m, &n, &nnz, e_i, e_o, e_w);

  CHECK_EQ(m, 1024);
  CHECK_EQ(n, 1024);
  CHECK_EQ(nnz, 4);

  free(e_i);
  free(e_o);
  free(e_w);
}

TEST(test_FW_1000, read) {
  size_t m, n, nnz;
  size_t *e_i;
  size_t *e_o;
  double *e_w;
  char simple_path[1024];
  get_data_path("test_FW_1000.mtx", simple_path);
  mtx_read_parallel(simple_path, &m, &n, &nnz, e_i, e_o, e_w);

  CHECK_EQ(m, 1000);
  CHECK_EQ(n, 1000);
  CHECK_EQ(nnz, 3996);

  free(e_i);
  free(e_o);
  free(e_w);
}
