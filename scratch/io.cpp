#include <charconv>
#include <cstdint>
#include <cstdio>
#include <fstream>
#include <iostream>
#include <string_view>
#include <vector>

// https://godbolt.org/z/5ajbvnsGf
std::vector<std::string_view> split(std::string_view sv, char separator = ' ') {
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

int main(int argc, char** argv) {
  // FILE* fp = std::fopen("tmp.mtx", "r");
  // std::fstream file("tmp.mtx");
  // std::stringstream ss;
  // ss << file.rdbuf();

  std::ifstream fin("tmp.mtx", std::ios::binary);
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

  size_t m = 0, n = 0, nnz = 0;
  std::string_view sv_header(&str[0] + start, end - start);
  std::vector<std::string_view> toks = split(sv_header);

  std::from_chars(toks[0].data(), toks[0].data() + toks[0].size(), m);
  std::from_chars(toks[1].data(), toks[1].data() + toks[1].size(), n);
  std::from_chars(toks[2].data(), toks[2].data() + toks[2].size(), nnz);

  std::vector<size_t> e_in(nnz), e_out(nnz);
  std::vector<double> e_weight(nnz);

  start = end + 1;
  end = str.find("\n", start);

  size_t data_line = 0;

  // Read the rest of the data
  while (end != std::string::npos) {
    // =( std::string_view initialization
    std::string_view sv(&str[0] + start, end - start);
    std::vector<std::string_view> toks = split(sv);

    std::from_chars(toks[0].data(), toks[0].data() + toks[0].size(),
                    e_in[data_line]);
    std::from_chars(toks[1].data(), toks[1].data() + toks[1].size(),
                    e_out[data_line]);
    std::from_chars(toks[2].data(), toks[2].data() + toks[2].size(),
                    e_weight[data_line]);

    data_line++;
    start = end + 1;
    end = str.find("\n", start);
  }

  return 0;
}