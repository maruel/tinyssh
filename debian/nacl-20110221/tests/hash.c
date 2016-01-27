#include "crypto_hash.h"
#include <stdio.h>

unsigned char x[8] = "testing\n";
unsigned char h[crypto_hash_BYTES];

int main()
{
    int i;
    crypto_hash(h, x, sizeof x);
    for (i = 0; i < crypto_hash_BYTES; ++i)
        printf("%02x", (unsigned int)h[i]);
    printf("\n");
    return 0;
}
