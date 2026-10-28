#include "sha2_64.h"
#include "hash_util.h"

#include <stdint.h>
#include <stdlib.h>
#include <string.h>

static inline uint64_t sha2_64_ch(uint64_t x, uint64_t y, uint64_t z) {
    return (x & y) ^ ((~x) & z);
}

static inline uint64_t sha2_64_maj(uint64_t x, uint64_t y, uint64_t z) {
    return (x & y) ^ (x & z) ^ (y & z);
}

static inline uint64_t sha2_64_big_sigma0(uint64_t x) {
    return rotr64(x, 28) ^ rotr64(x, 34) ^ rotr64(x, 39);
}

static inline uint64_t sha2_64_big_sigma1(uint64_t x) {
    return rotr64(x, 14) ^ rotr64(x, 18) ^ rotr64(x, 41);
}

static inline uint64_t sha2_64_small_sigma0(uint64_t x) {
    return rotr64(x, 1) ^ rotr64(x, 8) ^ (x >> 7);
}

static inline uint64_t sha2_64_small_sigma1(uint64_t x) {
    return rotr64(x, 19) ^ rotr64(x, 61) ^ (x >> 6);
}

static inline void sha2_64_round(uint64_t *a, uint64_t *b, uint64_t *c, uint64_t *d, uint64_t *e,
                                 uint64_t *f, uint64_t *g, uint64_t *h, uint64_t k, uint64_t w) {
    uint64_t temp1 = *h + sha2_64_big_sigma1(*e) + sha2_64_ch(*e, *f, *g) + k + w;
    uint64_t temp2 = sha2_64_big_sigma0(*a) + sha2_64_maj(*a, *b, *c);

    *h = *g;
    *g = *f;
    *f = *e;
    *e = *d + temp1;
    *d = *c;
    *c = *b;
    *b = *a;
    *a = temp1 + temp2;
}

/* Process one 128-byte block; `state` is updated in place. */
static void sha2_64_process_block(uint64_t state[8], const uint8_t block[128]) {
    static const uint64_t k[80] = {
        0x428a2f98d728ae22, 0x7137449123ef65cd, 0xb5c0fbcfec4d3b2f, 0xe9b5dba58189dbbc,
        0x3956c25bf348b538, 0x59f111f1b605d019, 0x923f82a4af194f9b, 0xab1c5ed5da6d8118,
        0xd807aa98a3030242, 0x12835b0145706fbe, 0x243185be4ee4b28c, 0x550c7dc3d5ffb4e2,
        0x72be5d74f27b896f, 0x80deb1fe3b1696b1, 0x9bdc06a725c71235, 0xc19bf174cf692694,
        0xe49b69c19ef14ad2, 0xefbe4786384f25e3, 0x0fc19dc68b8cd5b5, 0x240ca1cc77ac9c65,
        0x2de92c6f592b0275, 0x4a7484aa6ea6e483, 0x5cb0a9dcbd41fbd4, 0x76f988da831153b5,
        0x983e5152ee66dfab, 0xa831c66d2db43210, 0xb00327c898fb213f, 0xbf597fc7beef0ee4,
        0xc6e00bf33da88fc2, 0xd5a79147930aa725, 0x06ca6351e003826f, 0x142929670a0e6e70,
        0x27b70a8546d22ffc, 0x2e1b21385c26c926, 0x4d2c6dfc5ac42aed, 0x53380d139d95b3df,
        0x650a73548baf63de, 0x766a0abb3c77b2a8, 0x81c2c92e47edaee6, 0x92722c851482353b,
        0xa2bfe8a14cf10364, 0xa81a664bbc423001, 0xc24b8b70d0f89791, 0xc76c51a30654be30,
        0xd192e819d6ef5218, 0xd69906245565a910, 0xf40e35855771202a, 0x106aa07032bbd1b8,
        0x19a4c116b8d2d0c8, 0x1e376c085141ab53, 0x2748774cdf8eeb99, 0x34b0bcb5e19b48a8,
        0x391c0cb3c5c95a63, 0x4ed8aa4ae3418acb, 0x5b9cca4f7763e373, 0x682e6ff3d6b2b8a3,
        0x748f82ee5defb2fc, 0x78a5636f43172f60, 0x84c87814a1f0ab72, 0x8cc702081a6439ec,
        0x90befffa23631e28, 0xa4506cebde82bde9, 0xbef9a3f7b2c67915, 0xc67178f2e372532b,
        0xca273eceea26619c, 0xd186b8c721c0c207, 0xeada7dd6cde0eb1e, 0xf57d4f7fee6ed178,
        0x06f067aa72176fba, 0x0a637dc5a2c898a6, 0x113f9804bef90dae, 0x1b710b35131c471b,
        0x28db77f523047d84, 0x32caab7b40c72493, 0x3c9ebe0a15c9bebc, 0x431d67c49c100d4c,
        0x4cc5d4becb3e42b6, 0x597f299cfc657e2a, 0x5fcb6fab3ad6faec, 0x6c44198c4a475817};

    uint64_t w[80];

    for (unsigned i = 0; i < 16; ++i)
        w[i] = load_be64(block + 8 * i);

    for (unsigned i = 16; i < 80; ++i)
        w[i] =
            w[i - 16] + sha2_64_small_sigma0(w[i - 15]) + w[i - 7] + sha2_64_small_sigma1(w[i - 2]);

    uint64_t a = state[0];
    uint64_t b = state[1];
    uint64_t c = state[2];
    uint64_t d = state[3];
    uint64_t e = state[4];
    uint64_t f = state[5];
    uint64_t g = state[6];
    uint64_t h = state[7];

    for (unsigned i = 0; i < 80; ++i)
        sha2_64_round(&a, &b, &c, &d, &e, &f, &g, &h, k[i], w[i]);

    state[0] += a;
    state[1] += b;
    state[2] += c;
    state[3] += d;
    state[4] += e;
    state[5] += f;
    state[6] += g;
    state[7] += h;
}

/*
 * Finish the message: process the final `tail_len` bytes (0..127) plus
 * SHA-384/SHA-512 padding. `state` is updated in place.
 */
static void sha2_64_finish(uint64_t state[8], const uint8_t *tail, size_t tail_len,
                           uint64_t bit_len_high, uint64_t bit_len_low) {
    uint8_t block[256] = {0};
    size_t final_len = tail_len < 112 ? 128 : 256;

    if (tail_len != 0)
        memcpy(block, tail, tail_len);

    block[tail_len] = 0x80;
    store_be64(block + final_len - 16, bit_len_high);
    store_be64(block + final_len - 8, bit_len_low);

    for (size_t offset = 0; offset < final_len; offset += 128)
        sha2_64_process_block(state, block + offset);
}

uint8_t *sha2_64_digest(const uint8_t *message, size_t size, const uint64_t iv[8],
                        unsigned output_words) {
    if ((message == NULL && size != 0) || size > SIZE_MAX - 144 || output_words == 0 ||
        output_words > 8)
        return NULL;

    uint64_t state[8];
    memcpy(state, iv, sizeof state);

    uint8_t *digest = (uint8_t *)malloc(output_words * sizeof(uint64_t));

    if (digest == NULL)
        return NULL;

    uint64_t bit_len_high = (uint64_t)size >> 61;
    uint64_t bit_len_low = (uint64_t)size << 3;
    size_t tail_len = size % 128;
    size_t full_len = size - tail_len;

    for (size_t offset = 0; offset < full_len; offset += 128)
        sha2_64_process_block(state, message + offset);

    const uint8_t *tail = tail_len == 0 ? NULL : message + full_len;
    sha2_64_finish(state, tail, tail_len, bit_len_high, bit_len_low);

    for (unsigned i = 0; i < output_words; ++i)
        store_be64(digest + 8 * i, state[i]);

    return digest;
}
