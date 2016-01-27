#ifndef crypto_scalarmult_nistp256_H
#define crypto_scalarmult_nistp256_H

#define crypto_scalarmult_nistp256_tinynacl_BYTES 64
#define crypto_scalarmult_nistp256_tinynacl_SCALARBYTES 32
extern int crypto_scalarmult_nistp256_tinynacl(unsigned char*, const unsigned char*, const unsigned char*);
extern int crypto_scalarmult_nistp256_tinynacl_base(unsigned char*, const unsigned char*);

#define crypto_scalarmult_nistp256 crypto_scalarmult_nistp256_tinynacl
#define crypto_scalarmult_nistp256_base crypto_scalarmult_nistp256_tinynacl_base
#define crypto_scalarmult_nistp256_BYTES crypto_scalarmult_nistp256_tinynacl_BYTES
#define crypto_scalarmult_nistp256_SCALARBYTES crypto_scalarmult_nistp256_tinynacl_SCALARBYTES
#define crypto_scalarmult_nistp256_IMPLEMENTATION "tinynacl"
#define crypto_scalarmult_nistp256_VERSION "-"

#endif
