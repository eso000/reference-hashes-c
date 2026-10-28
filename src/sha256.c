#include "hash.h"
#include "sha2_32.h"

uint8_t *sha256(const uint8_t *message, size_t size) {
    static const uint32_t iv[8] = {0x6a09e667u, 0xbb67ae85u, 0x3c6ef372u, 0xa54ff53au,
                                   0x510e527fu, 0x9b05688cu, 0x1f83d9abu, 0x5be0cd19u};

    return sha2_32_digest(message, size, iv, 8);
}
