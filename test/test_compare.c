
#include "mtxio.h"
#include "tau/tau.h"
#include "test_util.h"

TAU_MAIN()

TEST(compare_read, simple) {
  char file_path[1024];
  get_data_path("simple.mtx", file_path);

  // mmio setup
  int M, N, NNZ;
  int *I, *J;
  double *VAL;
  MM_typecode matcode;
  int mmio_rv =
      mm_read_mtx_crd(file_path, &M, &N, &NNZ, &I, &J, &VAL, &matcode);
  REQUIRE_EQ(mmio_rv, 0);

  // mtxio setup
  size_t m, n, nnz;
  size_t *e_i = NULL;
  size_t *e_o = NULL;
  double *e_w = NULL;
  mtx_read_parallel(file_path, &m, &n, &nnz, &e_i, &e_o, &e_w);

  CHECK_EQ(m, M);
  CHECK_EQ(n, N);
  CHECK_EQ(nnz, NNZ);

  for (int i = 0; i < NNZ; i++) {
    CHECK_EQ(e_i[i], I[i]);
    CHECK_EQ(e_o[i], J[i]);
    // CHECK_EQ(e_w[i], VAL[i]);
    CHECK_LE(abs(e_w[i] - VAL[i]), 1e-12);
  }

  free(e_i);
  free(e_o);
  free(e_w);
}

TEST(compare_read, test_FW_1000) {
  char file_path[1024];
  get_data_path("test_FW_1000.mtx", file_path);
  // mmio setup
  int M, N, NNZ;
  int *I, *J;
  double *VAL;
  MM_typecode matcode;
  int mmio_rv =
      mm_read_mtx_crd(file_path, &M, &N, &NNZ, &I, &J, &VAL, &matcode);
  REQUIRE_EQ(mmio_rv, 0);

  // mtxio setup
  size_t m, n, nnz;
  size_t *e_i = NULL;
  size_t *e_o = NULL;
  double *e_w = NULL;
  mtx_read_parallel(file_path, &m, &n, &nnz, &e_i, &e_o, &e_w);

  CHECK_EQ(m, M);
  CHECK_EQ(n, N);
  CHECK_EQ(nnz, NNZ);

  for (int i = 0; i < NNZ; i++) {
    CHECK_EQ(e_i[i], I[i]);
    CHECK_EQ(e_o[i], J[i]);
    CHECK_LE(abs(e_w[i] - VAL[i]), 1e-12);
  }

  free(e_i);
  free(e_o);
  free(e_w);
  free(I);
  free(J);
  free(VAL);
}

TEST(compare_read, test_FW_2500) {
  char file_path[1024];
  get_data_path("test_FW_2500.mtx", file_path);
  // mmio setup
  int M, N, NNZ;
  int *I, *J;
  double *VAL;
  MM_typecode matcode;
  int mmio_rv =
      mm_read_mtx_crd(file_path, &M, &N, &NNZ, &I, &J, &VAL, &matcode);
  REQUIRE_EQ(mmio_rv, 0);

  // mtxio setup
  size_t m, n, nnz;
  size_t *e_i = NULL;
  size_t *e_o = NULL;
  double *e_w = NULL;
  mtx_read_parallel(file_path, &m, &n, &nnz, &e_i, &e_o, &e_w);

  CHECK_EQ(m, M);
  CHECK_EQ(n, N);
  CHECK_EQ(nnz, NNZ);

  for (int i = 0; i < NNZ; i++) {
    CHECK_EQ(e_i[i], I[i]);
    CHECK_EQ(e_o[i], J[i]);
    CHECK_LE(abs(e_w[i] - VAL[i]), 1e-12);
  }

  free(e_i);
  free(e_o);
  free(e_w);
  free(I);
  free(J);
  free(VAL);
}