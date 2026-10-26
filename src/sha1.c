#include "hash.h"
#include "hash_util.h"

#include <stdint.h>
#include <stdlib.h>

#include <string.h>


static inline uint32_t sha1_f1(uint32_t b, uint32_t c, uint32_t d)
{
    return (b & c) | ((~b) & d);
}

static inline uint32_t sha1_f2(uint32_t b, uint32_t c, uint32_t d)
{
    return b ^ c ^ d;
}

static inline uint32_t sha1_f3(uint32_t b, uint32_t c, uint32_t d)
{
    return (b & c) | (b & d) | (c & d);
}

static inline void sha1_round(uint32_t *a, uint32_t *b, uint32_t *c,
                              uint32_t *d, uint32_t *e,
                              uint32_t f, uint32_t k, uint32_t w)
{
    uint32_t temp = rotl32(*a, 5) + f + *e + k + w;

    *e = *d;
    *d = *c;
    *c = rotl32(*b, 30);
    *b = *a;
    *a = temp;
}

/* Process one 64-byte block; `state` is updated in place. */
static void sha1_process_block(uint32_t state[5], const uint8_t block[64])
{
    uint32_t w[80];

    for (unsigned j = 0; j < 16; ++j)
        w[j] = load_be32(block + 4 * j);

    for (unsigned i = 16; i < 80; ++i)
        w[i] = rotl32(w[i - 3] ^ w[i - 8] ^ w[i - 14] ^ w[i - 16], 1);

    uint32_t a = state[0];
    uint32_t b = state[1];
    uint32_t c = state[2];
    uint32_t d = state[3];
    uint32_t e = state[4];

    for (unsigned i = 0; i < 20; ++i) {
        uint32_t f = sha1_f1(b, c, d);
        sha1_round(&a, &b, &c, &d, &e, f, 0x5A827999u, w[i]);
    }

    for (unsigned i = 20; i < 40; ++i) {
        uint32_t f = sha1_f2(b, c, d);
        sha1_round(&a, &b, &c, &d, &e, f, 0x6ED9EBA1u, w[i]);
    }

    for (unsigned i = 40; i < 60; ++i) {
        uint32_t f = sha1_f3(b, c, d);
        sha1_round(&a, &b, &c, &d, &e, f, 0x8F1BBCDCu, w[i]);
    }

    for (unsigned i = 60; i < 80; ++i) {
        uint32_t f = sha1_f2(b, c, d);
        sha1_round(&a, &b, &c, &d, &e, f, 0xCA62C1D6u, w[i]);
    }

    state[0] += a;
    state[1] += b;
    state[2] += c;
    state[3] += d;
    state[4] += e;
}

/*
 * Finish the message: process the final `tail_len` bytes (0..63) plus
 * SHA-1 padding. `state` is updated in place.
 */
static void sha1_finish(uint32_t state[5], const uint8_t *tail,
                        size_t tail_len, uint64_t bit_len)
{
    uint8_t block[128] = {0};
    size_t final_len = tail_len < 56 ? 64 : 128;

    if (tail_len != 0)
        memcpy(block, tail, tail_len);

    block[tail_len] = 0x80;
    store_be32(block + final_len - 8, (uint32_t)(bit_len >> 32));
    store_be32(block + final_len - 4, (uint32_t)bit_len);

    for (size_t offset = 0; offset < final_len; offset += 64)
        sha1_process_block(state, block + offset);
}

uint8_t* sha1(const uint8_t* message, size_t size)
{
    uint32_t state[5] = {
        0x67452301u, 0xEFCDAB89u, 0x98BADCFEu, 0x10325476u, 0xC3D2E1F0u
    };

    if ((message == NULL && size != 0) || size > UINT64_MAX / 8 ||
        size > SIZE_MAX - 72)
        return NULL;

    uint64_t bit_len = (uint64_t)size << 3;
    size_t tail_len = size % 64;
    size_t full_len = size - tail_len;

    for (size_t offset = 0; offset < full_len; offset += 64)
        sha1_process_block(state, message + offset);

    const uint8_t *tail = tail_len == 0 ? NULL : message + full_len;
    sha1_finish(state, tail, tail_len, bit_len);

    uint8_t *v = (uint8_t*)malloc(20 * sizeof(uint8_t));

    if (v == NULL)
        return NULL;

    for (unsigned i = 0; i < 5; ++i)
        store_be32(v + 4 * i, state[i]);

    return &v[0];
}
