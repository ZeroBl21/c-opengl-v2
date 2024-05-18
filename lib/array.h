// Dynamic Arrays
#define ARENA_IMPLEMENTATION
#include "arena.h"

#include <stddef.h>
#include <string.h>

#define sizeof(x) (ptrdiff_t)sizeof(x)
#define countof(a) (sizeof(a) / sizeof(*(a)))
#define lengthof(s) (countof(s) - 1)

#define push(s, arena)                                                         \
  ((s)->len >= (s)->cap   ? grow(s, sizeof(*(s)->data), arena),                \
   (s)->data + (s)->len++ : (s)->data + (s)->len++)

static void grow(void *slice, ptrdiff_t size, arena *a) {
  struct {
    void *data;
    ptrdiff_t len;
    ptrdiff_t cap;
  } replica;
  memcpy(&replica, slice, sizeof(replica));

  replica.cap = replica.cap ? replica.cap : 1;
  ptrdiff_t align = 16;
  void *data = alloc(a, 2 * size, align, replica.cap);
  replica.cap *= 2;
  if (replica.len) {
    memcpy(data, replica.data, size * replica.len);
  }
  replica.data = data;

  memcpy(slice, &replica, sizeof(replica));
}

