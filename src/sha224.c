#include "hash.h"
#include "sha2_32.h"

uint8_t *sha224(const uint8_t *message, size_t size) {
    static const uint32_t iv[8] = {0xc1059ed8u, 0x367cd507u, 0x3070dd17u, 0xf70e5939u,
                                   0xffc00b31u, 0x68581511u, 0x64f98fa7u, 0xbefa4fa4u};

    return sha2_32_digest(message, size, iv, 7);
}
