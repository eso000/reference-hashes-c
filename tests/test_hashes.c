#include "testutil.h"
#include "boundary_vectors.h"

uint8_t* md5(uint8_t* message, int size);
uint8_t* sha1(uint8_t* message, int size);
uint8_t* sha224(uint8_t* message, int size);
uint8_t* sha256(uint8_t* message, int size);
uint8_t* sha384(uint8_t* message, int size);
uint8_t* sha512(uint8_t* message, int size);

typedef struct {
  const char* name;
  hash_fn fn;
  size_t digest_length;
  const uint8_t* message;
  size_t length;
  const char* expected;
} kat_vector;

/*
 * Published known-answer tests:
 *   MD5      RFC 1321, appendix A.5
 *   SHA-1    FIPS 180-4 / RFC 3174
 *   SHA-2    FIPS 180-4 / RFC 6234
 *
 * The 56-byte message for SHA-1/SHA-224/SHA-256 and the 112-byte message for
 * SHA-384/SHA-512 both end exactly at the point where the length field forces
 * an extra padding block, so they exercise the block-boundary cases.
 */
static const kat_vector kats[] = {
    {"md5/empty", md5, 16, (const uint8_t*)"", 0,
     "d41d8cd98f00b204e9800998ecf8427e"},
    {"md5/a", md5, 16, (const uint8_t*)"a", 1,
     "0cc175b9c0f1b6a831c399e269772661"},
    {"md5/abc", md5, 16, (const uint8_t*)"abc", 3,
     "900150983cd24fb0d6963f7d28e17f72"},
    {"md5/message-digest", md5, 16, (const uint8_t*)"message digest", 14,
     "f96b697d7cb7938d525a2f31aaf161d0"},
    {"md5/alphabet", md5, 16, (const uint8_t*)"abcdefghijklmnopqrstuvwxyz", 26,
     "c3fcd3d76192e4007dfb496cca67e13b"},
    {"md5/alphanumeric", md5, 16,
     (const uint8_t*)"ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789",
     62, "d174ab98d277d9f5a5611c2c9f419d9f"},
    {"md5/digits", md5, 16,
     (const uint8_t*)"12345678901234567890123456789012345678901234567890123456789012345678901234567890",
     80, "57edf4a22be3c955ac49da2e2107b67a"},

    {"sha1/empty", sha1, 20, (const uint8_t*)"", 0,
     "da39a3ee5e6b4b0d3255bfef95601890afd80709"},
    {"sha1/abc", sha1, 20, (const uint8_t*)"abc", 3,
     "a9993e364706816aba3e25717850c26c9cd0d89d"},
    {"sha1/56-bytes", sha1, 20,
     (const uint8_t*)"abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq",
     56, "84983e441c3bd26ebaae4aa1f95129e5e54670f1"},

    {"sha224/empty", sha224, 28, (const uint8_t*)"", 0,
     "d14a028c2a3a2bc9476102bb288234c415a2b01f828ea62ac5b3e42f"},
    {"sha224/abc", sha224, 28, (const uint8_t*)"abc", 3,
     "23097d223405d8228642a477bda255b32aadbce4bda0b3f7e36c9da7"},
    {"sha224/56-bytes", sha224, 28,
     (const uint8_t*)"abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq",
     56, "75388b16512776cc5dba5da1fd890150b0c6455cb4f58b1952522525"},

    {"sha256/empty", sha256, 32, (const uint8_t*)"", 0,
     "e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855"},
    {"sha256/abc", sha256, 32, (const uint8_t*)"abc", 3,
     "ba7816bf8f01cfea414140de5dae2223b00361a396177a9cb410ff61f20015ad"},
    {"sha256/56-bytes", sha256, 32,
     (const uint8_t*)"abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq",
     56, "248d6a61d20638b8e5c026930c3e6039a33ce45964ff2167f6ecedd419db06c1"},

    {"sha384/empty", sha384, 48, (const uint8_t*)"", 0,
     "38b060a751ac96384cd9327eb1b1e36a21fdb71114be07434c0cc7bf63f6e1da274edebfe76f65fbd51ad2f14898b95b"},
    {"sha384/abc", sha384, 48, (const uint8_t*)"abc", 3,
     "cb00753f45a35e8bb5a03d699ac65007272c32ab0eded1631a8b605a43ff5bed8086072ba1e7cc2358baeca134c825a7"},
    {"sha384/112-bytes", sha384, 48,
     (const uint8_t*)"abcdefghbcdefghicdefghijdefghijkefghijklfghijklmghijklmnhijklmnoijklmnopjklmnopqklmnopqrlmnopqrsmnopqrstnopqrstu",
     112, "09330c33f71147e83d192fc782cd1b4753111b173b3b05d22fa08086e3b0f712fcc7c71a557e2db966c3e9fa91746039"},

    {"sha512/empty", sha512, 64, (const uint8_t*)"", 0,
     "cf83e1357eefb8bdf1542850d66d8007d620e4050b5715dc83f4a921d36ce9ce47d0d13c5d85f2b0ff8318d2877eec2f63b931bd47417a81a538327af927da3e"},
    {"sha512/abc", sha512, 64, (const uint8_t*)"abc", 3,
     "ddaf35a193617abacc417349ae20413112e6fa4e89a97ea20a9eeee64b55d39a2192992a274fc1a836ba3c23a3feebbd454d4423643ce80e2a9ac94fa54ca49f"},
    {"sha512/112-bytes", sha512, 64,
     (const uint8_t*)"abcdefghbcdefghicdefghijdefghijkefghijklfghijklmghijklmnhijklmnoijklmnopjklmnopqklmnopqrlmnopqrsmnopqrstnopqrstu",
     112, "8e959b75dae313da8cf4f72814fc143f8f7779c6eb9f7fa17299aeadb6889018501d289e4900f7e4331b99dec4b5433ac7d329eeb6dd26545e96e55b874be909"},
};

static void run_boundary_vectors(const char* name, hash_fn fn,
                                 size_t digest_length,
                                 const boundary_vector* vectors,
                                 size_t count) {
  size_t max_length = 0;
  for (size_t i = 0; i < count; ++i) {
    if ((size_t)vectors[i].length > max_length)
      max_length = (size_t)vectors[i].length;
  }

  uint8_t* message = (uint8_t*)malloc(max_length ? max_length : 1);
  if (message == NULL) {
    printf("FAIL %s: out of memory\n", name);
    ++test_total;
    ++test_failed;
    return;
  }

  for (size_t i = 0; i < count; ++i) {
    char vector_name[64];
    memset(message, 'a', (size_t)vectors[i].length);
    snprintf(vector_name, sizeof vector_name, "%s/boundary-%d", name,
             vectors[i].length);
    check_digest(vector_name, fn, digest_length, message,
                 (size_t)vectors[i].length, vectors[i].expected);
  }

  free(message);
}

static void run_million_a(const char* name, hash_fn fn, size_t digest_length,
                          const char* expected) {
  size_t length = 1000000;
  uint8_t* message = (uint8_t*)malloc(length);
  if (message == NULL) {
    printf("FAIL %s: out of memory\n", name);
    ++test_total;
    ++test_failed;
    return;
  }

  memset(message, 'a', length);
  check_digest(name, fn, digest_length, message, length, expected);
  free(message);
}

int main(void) {
  for (size_t i = 0; i < sizeof kats / sizeof kats[0]; ++i) {
    check_digest(kats[i].name, kats[i].fn, kats[i].digest_length,
                 kats[i].message, kats[i].length, kats[i].expected);
  }

  run_boundary_vectors("md5", md5, 16, md5_boundary,
                       sizeof md5_boundary / sizeof md5_boundary[0]);
  run_boundary_vectors("sha1", sha1, 20, sha1_boundary,
                       sizeof sha1_boundary / sizeof sha1_boundary[0]);
  run_boundary_vectors("sha224", sha224, 28, sha224_boundary,
                       sizeof sha224_boundary / sizeof sha224_boundary[0]);
  run_boundary_vectors("sha256", sha256, 32, sha256_boundary,
                       sizeof sha256_boundary / sizeof sha256_boundary[0]);
  run_boundary_vectors("sha384", sha384, 48, sha384_boundary,
                       sizeof sha384_boundary / sizeof sha384_boundary[0]);
  run_boundary_vectors("sha512", sha512, 64, sha512_boundary,
                       sizeof sha512_boundary / sizeof sha512_boundary[0]);

  run_million_a("md5/million-a", md5, 16,
                "7707d6ae4e027c70eea2a935c2296f21");
  run_million_a("sha1/million-a", sha1, 20,
                "34aa973cd4c4daa4f61eeb2bdbad27316534016f");
  run_million_a("sha224/million-a", sha224, 28,
                "20794655980c91d8bbb4c1ea97618a4bf03f42581948b2ee4ee7ad67");
  run_million_a("sha256/million-a", sha256, 32,
                "cdc76e5c9914fb9281a1c7e284d73e67f1809a48a497200e046d39ccc7112cd0");
  run_million_a("sha384/million-a", sha384, 48,
                "9d0e1809716474cb086e834e310a4a1ced149e9c00f248527972cec5704c2a5b07b8b3dc38ecc4ebae97ddd87f3d8985");
  run_million_a("sha512/million-a", sha512, 64,
                "e718483d0ce769644e2e42c7bc15b4638e1f98b13b2044285632a803afa973ebde0ff244877ea60a4cb0432ce577c31beb009c5c2c49aa2e4eadb217ad8cc09b");

  return test_summary("hash KATs");
}
