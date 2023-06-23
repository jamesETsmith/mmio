
#include "mtxio.h"
#include "tau/tau.h"
#include "test_util.h"

TAU_MAIN()

TEST(simple, read) {
  size_t m, n, nnz;
  size_t *e_i = NULL;
  size_t *e_o = NULL;
  double *e_w = NULL;
  char simple_path[1024];
  get_data_path("simple.mtx", simple_path);
  mtx_read_parallel(simple_path, &m, &n, &nnz, &e_i, &e_o, &e_w);

  CHECK_EQ(m, 1024);
  CHECK_EQ(n, 1024);
  CHECK_EQ(nnz, 4);

  double e_w_true[] = {5.644968938262174e-01, 4.976118787264324e-01,
                       5.474516390978542e-01, 4.063977374072301e-01};

  for (size_t i = 0; i < nnz; i++) {
    // CHECK_EQ(e_w[i], e_w_true[i]);
    CHECK_LE(abs(e_w[i] - e_w_true[i]), 1e-12);
  }

  free(e_i);
  free(e_o);
  free(e_w);
}

TEST(test_FW_1000, read) {
  size_t m, n, nnz;
  size_t *e_i = NULL;
  size_t *e_o = NULL;
  double *e_w = NULL;
  char simple_path[1024];
  get_data_path("test_FW_1000.mtx", simple_path);
  mtx_read_parallel(simple_path, &m, &n, &nnz, &e_i, &e_o, &e_w);

  CHECK_EQ(m, 1000);
  CHECK_EQ(n, 1000);
  CHECK_EQ(nnz, 3996);

  free(e_i);
  free(e_o);
  free(e_w);
}
