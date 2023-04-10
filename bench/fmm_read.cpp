#include <fast_matrix_market/app/GraphBLAS.hpp>
#include <fast_matrix_market/fast_matrix_market.hpp>
#include <fstream>

int main(int argc, char* argv[]) {
  if (argc != 2) {
    fprintf(stderr, "Usage: %s [martix-market-filename]\n", argv[0]);
    exit(1);
  }

  GrB_Matrix A;
  std::ifstream f(argv[1]);
  fast_matrix_market::read_matrix_market_graphblas(f, &A);
  return 0;
}