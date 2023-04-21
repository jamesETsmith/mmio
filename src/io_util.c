#include "io_util.h"

// From https://github.com/geofflangdale/simdcsv/blob/master/src/io_util.cpp

char* allocate_padded_buffer(size_t length) {
  size_t total_padded_len = length + 64;
  char* padded_buffer = (char*)aligned_alloc(64, total_padded_len);
  return padded_buffer;
}

// Now I need to figure out how to parse this data buffer, one line at a time
int read_padded_data(char* padded_data_buf, const char* filename) {
  FILE* fp = fopen(filename, "rb");
  if (fp != nullptr) {
    fseek(fp, 0, SEEK_END);
    size_t len = ftell(fp);
    padded_data_buf = allocate_padded_buffer(len);
    if (padded_data_buf == nullptr) {
      fclose(fp);
      return 1;  // there was a problem
    } else {
      rewind(fp);
      size_t readb = fread(padded_data_buf, 1, len, fp);
      fclose(fp);
      if (readb != len) {
        free(padded_data_buf);
        return 1;  // there was a problem
      }
      return 0;
    }
  } else {
    return 1;  // There was a problem
  }
}