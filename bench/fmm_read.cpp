#include <fast_matrix_market/fast_matrix_market.hpp>
#include <fstream>

int main(int argc, char* argv[]) {
  if (argc != 2) {
    fprintf(stderr, "Usage: %s [martix-market-filename]\n", argv[0]);
    exit(1);
  }

  struct triplet_matrix {
    int64_t nrows = 0, ncols = 0;
    std::vector<int64_t> rows, cols;
    std::vector<double> vals;  // or int64_t, float, std::complex<double>, etc.
  } mat;

  std::ifstream f(argv[1]);
  fast_matrix_market::read_matrix_market_triplet(f, mat.nrows, mat.ncols,
                                                 mat.rows, mat.cols, mat.vals);
  return 0;
}