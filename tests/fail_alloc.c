#include "fail_alloc.h"

#include <stdlib.h>

void* __real_malloc(size_t size);
void* __real_calloc(size_t count, size_t size);
void* __real_realloc(void* pointer, size_t size);
void __real_free(void* pointer);

static long fail_index = -1;
static long alloc_calls = 0;
static int did_fail = 0;
static long live = 0;

static int should_fail(void) {
  if (fail_index < 0)
    return 0;

  if (alloc_calls++ == fail_index) {
    fail_index = -1;
    did_fail = 1;
    return 1;
  }

  return 0;
}

void* __wrap_malloc(size_t size) {
  if (should_fail())
    return NULL;

  void* pointer = __real_malloc(size);
  if (pointer != NULL)
    ++live;
  return pointer;
}

void* __wrap_calloc(size_t count, size_t size) {
  if (should_fail())
    return NULL;

  void* pointer = __real_calloc(count, size);
  if (pointer != NULL)
    ++live;
  return pointer;
}

void* __wrap_realloc(void* pointer, size_t size) {
  if (should_fail())
    return NULL;

  void* result = __real_realloc(pointer, size);
  if (result != NULL && pointer == NULL)
    ++live;
  return result;
}

void __wrap_free(void* pointer) {
  if (pointer != NULL)
    --live;
  __real_free(pointer);
}

void fail_alloc_arm(long index) {
  fail_index = index;
  alloc_calls = 0;
  did_fail = 0;
}

void fail_alloc_disarm(void) {
  fail_index = -1;
}

long fail_alloc_calls(void) {
  return alloc_calls;
}

int fail_alloc_triggered(void) {
  return did_fail;
}

long fail_alloc_live(void) {
  return live;
}
