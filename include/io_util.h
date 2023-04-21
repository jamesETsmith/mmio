#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

char* allocate_padded_buffer(size_t length);

int read_padded_data(char* padded_data_buf, const char* filename);
