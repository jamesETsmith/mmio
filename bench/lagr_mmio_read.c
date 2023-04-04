#include <GraphBLAS.h>
#include <LAGraph.h>

int main(int argc, char *argv[]) {
  char msg[LAGRAPH_MSG_LEN];
  LAGraph_Init(msg);
  FILE *f;
  GrB_Matrix A;

  if (argc < 2) {
    fprintf(stderr, "Usage: %s [martix-market-filename]\n", argv[0]);
    exit(1);
  } else {
    if ((f = fopen(argv[1], "r")) == NULL) exit(1);
  }

  LAGraph_MMRead(&A, f, msg);
  LAGraph_Finalize(msg);

  return 0;
}