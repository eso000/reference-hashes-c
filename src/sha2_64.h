#ifndef SHA2_64_H
#define SHA2_64_H

#include <stddef.h>
#include <stdint.h>

/*
 * Internal one-shot SHA-384/SHA-512 core.
 *
 * Returns a newly allocated digest containing `output_words` big-endian
 * 64-bit words, or NULL on invalid arguments or allocation failure. The
 * caller owns the returned buffer. This is not part of the public API.
 */
uint8_t *sha2_64_digest(const uint8_t *message, size_t size,
                        const uint64_t iv[8], unsigned output_words);

#endif
