#ifndef HASH_TESTUTIL_H
#define HASH_TESTUTIL_H

#include <limits.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef uint8_t* (*hash_fn)(uint8_t* message, int size);

typedef struct {
  int length;
  unsigned char fill;
  const char* expected;
} boundary_vector;

static int test_total = 0;
static int test_failed = 0;

static inline void encode_hex(const uint8_t* digest, size_t length, char* out) {
  static const char digits[] = "0123456789abcdef";

  for (size_t i = 0; i < length; ++i) {
    out[2 * i] = digits[digest[i] >> 4];
    out[2 * i + 1] = digits[digest[i] & 0x0f];
  }
  out[2 * length] = '\0';
}

static inline void check_digest(const char* name, hash_fn fn, size_t digest_length,
                                const uint8_t* message, size_t length,
                                const char* expected) {
  ++test_total;

  if (length > INT_MAX || digest_length > 64) {
    printf("FAIL %s: unsupported vector size\n", name);
    ++test_failed;
    return;
  }

  uint8_t* input = (uint8_t*)malloc(length ? length : 1);
  if (input == NULL) {
    printf("FAIL %s: out of memory\n", name);
    ++test_failed;
    return;
  }
  memcpy(input, message, length);

  uint8_t* digest = fn(input, (int)length);
  if (digest == NULL) {
    printf("FAIL %s: hash returned NULL\n", name);
    ++test_failed;
    free(input);
    return;
  }

  char got[129];
  encode_hex(digest, digest_length, got);
  if (strcmp(got, expected) != 0) {
    printf("FAIL %s\n  expected: %s\n  actual:   %s\n", name, expected, got);
    ++test_failed;
  }

  if (memcmp(input, message, length) != 0) {
    printf("FAIL %s: input buffer was modified\n", name);
    ++test_failed;
  }

  free(digest);
  free(input);
}

static inline int test_summary(const char* suite) {
  if (test_failed != 0) {
    printf("%s: %d of %d vectors failed\n", suite, test_failed, test_total);
    return 1;
  }

  printf("%s: %d vectors passed\n", suite, test_total);
  return 0;
}

#endif
