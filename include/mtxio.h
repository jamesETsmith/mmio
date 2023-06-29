#ifndef MTXIO_H
#define MTXIO_H

#include <assert.h>
#include <fcntl.h>
#include <omp.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <time.h>
#include <unistd.h>

#ifndef NDEBUG
#define MTXIO_LOG(...) printf(__VA_ARGS__)
#else
#define MTXIO_LOG(...)
#endif

//
// Matrix Market Enums
//

typedef enum {
  MTX_MATRIX,
  MTX_VECTOR,
} MTX_OBJ;

typedef enum {
  MTX_COORD,
  MTX_ARRAY,
} MTX_FMT;

typedef enum {
  MTX_REAL,
  MTX_COMPLEX,
  MTX_INTEGER,
  MTX_PATTERN,
} MTX_FIELD;

typedef enum {
  MTX_GENERAL,
  MTX_SYMMETRIC,
  MTX_SKEW_SYMM,
  MTX_HERMITIAN,
} MTX_SYMM;

//
// MTXIO enums
//
typedef enum {
  MTXIO_SUCCESS = 0,
  MTXIO_PANIC = -1,
  MTXIO_NOT_IMPLEMENTED = -99,
} MTXIO_RESULT;

MTXIO_RESULT read_header(char *data, size_t data_size);

MTXIO_RESULT
mtx_read_parallel(const char *filename, size_t *m, size_t *n, size_t *nnz,
                  size_t **e_i_p, size_t **e_o_p, double **e_w_p);

// Patch for pretty formatting until
// https://github.com/llvm/llvm-project/issues/18080 is closed
// see
// https://github.com/travisjeffery/ClangFormat-Xcode/issues/131#issuecomment-990072302
#define GENERIC_CASE(type, fn)                                                 \
  type:                                                                        \
  fn

#define mtxio_read(filename, m, n, nnz, e_i_p, e_o_p, e_w_p)                   \
  _Generic((e_w_p),                                                            \
           GENERIC_CASE(double **, mtxio_read_parallel(filename, m, n, nnz,    \
                                                       e_i_p, e_o_p, e_w_p)))

#endif
