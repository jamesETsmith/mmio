#include <charconv>
#include <chrono>
#include <cstdint>
#include <cstdio>
#include <fstream>
#include <iostream>
#include <string_view>
#include <vector>

// https://godbolt.org/z/5ajbvnsGf
inline std::vector<std::string_view> split(std::string_view sv,
                                           char separator = ' ') {
  std::vector<std::string_view> res;
  size_t start = 0, end = 0;
  for (size_t i = 0; i < sv.size(); i++) {
    char const c = sv[i];
    if (c == separator) {
      // No non-separator
      if (start == end) {
        start++;
        end++;
      } else {
        res.push_back(sv.substr(start, end - start));
        start = i + 1;
        end = i + 1;
      }
    } else {
      end++;
    }
  }

  if (start != end) {
    res.push_back(sv.substr(start, end - start));
  }

  return res;
}

template <typename T>
inline void from_chars_range(std::string_view sv, T& result) {
  std::from_chars(sv.data(), sv.data() + sv.size(), result);
}

int main(int argc, char** argv) {
  if (argc != 2) {
    std::cerr << "Wrong command line args. Use './io matrix.mtx'" << std::endl;
    exit(1);
  }
  std::string filename(argv[1]);
  std::cout << "Parsing " << filename << std::endl;

  std::ifstream fin(filename, std::ios::binary);
  if (!fin) return 0;

  fin.seekg(0, std::ios::end);
  size_t filesize = (size_t)fin.tellg();
  fin.seekg(0);
  std::string str(filesize, 0);
  fin.read(&str[0], filesize);
  fin.close();

  // Read m, n, nnz
  size_t start = 0, end = 0;
  end = str.find("\n");

  // skip comments
  while (str[start] == '%') {
    start = end + 1;
    end = str.find("\n", start);
  }

  // Meta info

  size_t m = 0, n = 0, nnz = 0;
  std::string_view sv_header(&str[0] + start, end - start);
  std::vector<std::string_view> toks_header = split(sv_header);

  from_chars_range(toks_header[0], m);
  from_chars_range(toks_header[1], n);
  from_chars_range(toks_header[2], nnz);

  std::vector<size_t> e_in(nnz), e_out(nnz);
  std::vector<double> e_weight(nnz);
  std::vector<size_t> line_start(nnz);
  std::vector<size_t> line_end(nnz);

  start = end + 1;
  end = str.find("\n", start);

  // size_t data_line = 0;
  // auto find_time_start = std::chrono::steady_clock::now();
  for (size_t i = 0; i < nnz; i++) {
    line_start[i] = start;
    line_end[i] = end;
    // std::cout << start << " " << end << std::endl;

    start = end + 1;
    end = str.find("\n", start);
  }
  // double find_time = std::chrono::duration<double>(
  //                        std::chrono::steady_clock::now() - find_time_start)
  //                        .count();

  line_end.back() = filesize;
  // std::cout << line_end.back() << std::endl;

  // Read the rest of the data
  // double split_time = 0;
  // double from_chars_time = 0;
// #pragma omp parallel for reduction(+ : split_time, from_chars_time)
#pragma omp parallel for
  for (size_t i = 0; i < nnz; i++) {
    // =( std::string_view initialization
    std::string_view sv(&str[0] + line_start[i], line_end[i] - line_start[i]);
    // auto st_i = std::chrono::steady_clock::now();
    std::vector<std::string_view> toks = split(sv);
    // split_time +=
    //     std::chrono::duration<double>(std::chrono::steady_clock::now() -
    //     st_i)
    //         .count();

    // auto fc_i = std::chrono::steady_clock::now();
    from_chars_range(toks[0], e_in[i]);
    from_chars_range(toks[1], e_out[i]);
    from_chars_range(toks[2], e_weight[i]);
    // from_chars_time +=
    //     std::chrono::duration<double>(std::chrono::steady_clock::now() -
    //     fc_i)
    //         .count();
  }

  // Logging
  std::cout << "Number of edges " << e_in.size() << std::endl;
  std::cout << "First edge: " << e_in[0] << " " << e_out[0] << " "
            << e_weight[0] << std::endl;
  std::cout << "Last edge: " << e_in.back() << " " << e_out.back() << " "
            << e_weight.back() << std::endl;

  // std::cout << "Find time            (cumulative): " << find_time <<
  // std::endl; std::cout << "Split time           (cumulative): " << split_time
  // << std::endl; std::cout << "std::from_chars time (cumulative): " <<
  // from_chars_time
  //           << std::endl;

  return 0;
}