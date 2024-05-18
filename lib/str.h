#ifndef STR_H_
#define STR_H_

#include <stddef.h>
#include <stdint.h>

typedef struct {
  uint8_t *data;
  ptrdiff_t len;
} string;

#endif // STR_H_
