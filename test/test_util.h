#include <stdio.h>
#include <stdlib.h>

#include "mmio.h"

void get_data_path(char const *filename, char *data_path) {
#define BUFSIZE 1024
  char *envvar = "MTXIO_DATA_DIR";

  // Make sure envar actually exists
  if (!getenv(envvar)) {
    fprintf(stderr, "[ERROR] The environment variable %s was not found.\n",
            envvar);
    exit(1);
  }

  // Make sure the buffer is large enough to hold the environment variable
  // value.
  if (snprintf(data_path, BUFSIZE, "%s/%s", getenv(envvar), filename) >=
      BUFSIZE) {
    fprintf(stderr, "[ERROR] BUFSIZE of %d was too small. Aborting\n", BUFSIZE);
    exit(1);
  }
  printf("MTXIO_DATA_DIR: %s\n", data_path);
}
