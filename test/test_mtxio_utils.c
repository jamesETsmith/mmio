
#include "mtxio.h"
#include "tau/tau.h"
#include "test_util.h"

TAU_MAIN()

TEST(read_header, simple) {
  char good_banner[1024] = "%%MatrixMarket matrix coordinate real general";
  CHECK_EQ(read_header(good_banner, strlen(good_banner)), MTXIO_SUCCESS);

  char bad_banner[1024] = "%%MaMarket matrix coordinate real general";
  CHECK_EQ(read_header(bad_banner, strlen(bad_banner)), MTXIO_PANIC);

  char bad_object[1024] = "%%MatrixMarket mtx coordinate real general";
  CHECK_EQ(read_header(bad_object, strlen(bad_object)), MTXIO_PANIC);

  char bad_format[1024] = "%%MatrixMarket matrix coonate real general";
  CHECK_EQ(read_header(bad_format, strlen(bad_format)), MTXIO_PANIC);

  char bad_type[1024] = "%%MatrixMarket matrix coordinate comp general";
  CHECK_EQ(read_header(bad_type, strlen(bad_type)), MTXIO_PANIC);

  char bad_scheme[1024] = "%%MatrixMarket matrix coordinate real skeeeew";
  CHECK_EQ(read_header(bad_scheme, strlen(bad_scheme)), MTXIO_PANIC);
}