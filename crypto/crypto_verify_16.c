#include "crypto_verify_16.h"
#include "verify.h"

int crypto_verify_16_tinynacl(const unsigned char* x, const unsigned char* y)
{
    return verify(x, y, 16);
}
