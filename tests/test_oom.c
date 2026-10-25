#include "hash.h"
#include "testutil.h"
#include "fail_alloc.h"

/*
 * Each hash function performs a small, fixed number of allocations. Fail each
 * one in turn and require the function to report the failure by returning NULL
 * instead of crashing, leaking, or returning a digest built from partial data.
 */

static const uint8_t message[] = {
    0x00, 0x01, 0x02, 0x03, 0x7f, 0x80, 0x81, 0xfe,
    0xff, 0x20, 0x00, 0x80, 0xff, 0xaa, 0x55, 0x10,
};

static void check_failures(const char* name, hash_fn fn, size_t digest_length) {
  (void)digest_length;

  long live_before = fail_alloc_live();

  for (long index = 0;; ++index) {
    ++test_total;

    fail_alloc_arm(index);
    uint8_t* digest = fn(message, sizeof message);
    int triggered = fail_alloc_triggered();
    fail_alloc_disarm();

    if (!triggered) {
      if (digest == NULL) {
        printf("FAIL %s: no allocation failed but hash returned NULL\n", name);
        ++test_failed;
      } else {
        free(digest);
      }

      if (fail_alloc_live() != live_before) {
        printf("FAIL %s: leaked memory after successful hash\n", name);
        ++test_failed;
      }
      return;
    }

    if (digest != NULL) {
      printf("FAIL %s: allocation %ld failed but hash returned a digest\n",
             name, index);
      free(digest);
      ++test_failed;
    }

    if (fail_alloc_live() != live_before) {
      printf("FAIL %s: allocation %ld failure leaked memory\n", name, index);
      ++test_failed;
    }
  }
}

int main(void) {
  check_failures("md5", md5, 16);
  check_failures("sha1", sha1, 20);
  check_failures("sha224", sha224, 28);
  check_failures("sha256", sha256, 32);
  check_failures("sha384", sha384, 48);
  check_failures("sha512", sha512, 64);

  return test_summary("allocation failures");
}
