#ifndef ARENA_H_
#define ARENA_H_

#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include <string.h>

// Arena Allocator
//
#define make(a, t, n) (t *)alloc(a, sizeof(t), _Alignof(t), n)
// #define make(a, t, n) (t *)alloc(a, (n) * sizeof(t), _Alignof(t), n)

typedef struct {
  char *beg;
  char *end;
  char *current;
} arena;

void arena_init(arena *a, ptrdiff_t size);
void *arena_alloc(arena *a, ptrdiff_t size, ptrdiff_t align, ptrdiff_t count);
void arena_reset(arena *a);
void arena_free(arena *a);

#endif // ARENA_H_

// #define ARENA_IMPLEMENTATION
#ifdef ARENA_IMPLEMENTATION

__attribute__((malloc, alloc_size(2), alloc_align(3))) void *
alloc(arena *a, ptrdiff_t size, ptrdiff_t align, ptrdiff_t count) {

  ptrdiff_t padding =
      (align - ((uintptr_t)a->current & (align - 1))) & (align - 1);
  ptrdiff_t available = a->end - a->current - padding;

  if (available < 0 || count > available / size) {
    // Política de memoria insuficiente
    fprintf(stderr,
            "ERROR: Not enough available memory on this arena for allocating "
            "%td bits\n",
            count);
    abort();
  }

  void *p = a->current + padding;
  a->current += padding + count * size;
  return memset(p, 0, count * size);
}

// For debug alloc
// #define alloc(a, size, align, count) alloc_debug(a, size, align, count,
// __FILE__, __LINE__)

__attribute__((malloc, alloc_size(2), alloc_align(3))) void *
alloc_debug(arena *a, ptrdiff_t size, ptrdiff_t align, ptrdiff_t count,
            const char *file, int line) {

  ptrdiff_t padding =
      (align - ((uintptr_t)a->current & (align - 1))) & (align - 1);
  ptrdiff_t available = a->end - a->current - padding;

  if (available < 0 || count > available / size) {
    // Política de memoria insuficiente
    fprintf(stderr,
            "ERROR: Not enough available memory on this arena for allocating "
            "%td bits\nFile: %s:%d\n",
            count, file, line);
    abort();
  }

  void *p = a->current + padding;
  a->current += padding + count * size;
  return memset(p, 0, count * size);
}

arena new_arena(ptrdiff_t cap) {
  arena a = {0};
  a.beg = (char *)malloc(cap);
  a.end = a.beg ? a.beg + cap : 0;
  a.current = a.beg;
  return a;
}

void arena_free(arena *a) {
  free(a->beg);
  a->beg = a->end = NULL;
}

void arena_reset(arena *a) { a->current = a->beg; }

#endif // ARENA_IMPLEMENTATION
