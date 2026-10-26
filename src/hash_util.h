#ifndef HASH_UTIL_H
#define HASH_UTIL_H

#include <stdint.h>

/* Rotations mask the count so a zero rotation is well-defined. */

static inline uint32_t rotl32(uint32_t x, unsigned n)
{
  n &= 31u;
  return n == 0u ? x : (x << n) | (x >> (32u - n));
}

static inline uint32_t rotr32(uint32_t x, unsigned n)
{
  n &= 31u;
  return n == 0u ? x : (x >> n) | (x << (32u - n));
}

static inline uint64_t rotr64(uint64_t x, unsigned n)
{
  n &= 63u;
  return n == 0u ? x : (x >> n) | (x << (64u - n));
}

static inline uint32_t load_be32(const uint8_t *p)
{
  return ((uint32_t)p[0] << 24) |
         ((uint32_t)p[1] << 16) |
         ((uint32_t)p[2] << 8) |
         ((uint32_t)p[3]);
}

static inline void store_be32(uint8_t *p, uint32_t x)
{
  p[0] = (uint8_t)(x >> 24);
  p[1] = (uint8_t)(x >> 16);
  p[2] = (uint8_t)(x >> 8);
  p[3] = (uint8_t)x;
}

static inline uint32_t load_le32(const uint8_t *p)
{
  return ((uint32_t)p[0]) |
         ((uint32_t)p[1] << 8) |
         ((uint32_t)p[2] << 16) |
         ((uint32_t)p[3] << 24);
}

static inline void store_le32(uint8_t *p, uint32_t x)
{
  p[0] = (uint8_t)x;
  p[1] = (uint8_t)(x >> 8);
  p[2] = (uint8_t)(x >> 16);
  p[3] = (uint8_t)(x >> 24);
}

static inline uint64_t load_be64(const uint8_t *p)
{
  return ((uint64_t)load_be32(p) << 32) | load_be32(p + 4);
}

static inline void store_be64(uint8_t *p, uint64_t x)
{
  store_be32(p, (uint32_t)(x >> 32));
  store_be32(p + 4, (uint32_t)x);
}

#endif
