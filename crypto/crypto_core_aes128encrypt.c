#include "crypto_core_aes128encrypt.h"
#include "aes.h"
#include "cleanup.h"

int crypto_core_aes128encrypt_tinynacl(unsigned char* out, const unsigned char* in, const unsigned char* k, const unsigned char* c)
{

    unsigned char d[176];

    aes_expandkey128(d, k);
    aes_core(out, in, d, 10);
    cleanup(d);
    if (c)
        return -1;
    return 0;
}
