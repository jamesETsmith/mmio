#include "pigo.hpp"

#include <iostream>
using namespace std;
using namespace pigo;
int main(int argc, char **argv) {
  if (argc != 2) {
    cerr << "Usage: " << argv[0] << " filename" << endl;
    return 1;
  }

  COO g{argv[1]};

  // cout << "number of vertices: " << g.n() << endl;
  // cout << "number of edges: " << g.m() << endl;

  // cout << "vertex 0's neighbors:\n";
  // for (auto n : g.neighbors(0))
  //   cout << n << endl;
  return 0;
}