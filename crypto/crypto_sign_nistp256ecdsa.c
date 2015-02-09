/*
20140917
Jan Mojzis
Public domain.
*/

#include "fep256.h"
#include "scp256.h"
#include "gep256.h"
#include "crypto_hash_sha512.h"
#include "crypto_hash_sha256.h"
#include "cleanup.h"
#include "randombytes.h"
#include "crypto_verify_32.h"
#include "crypto_sign_nistp256ecdsa.h"


int crypto_sign_nistp256ecdsa_tinynacl(unsigned char *sm, unsigned long long *smlen, const unsigned char *m, unsigned long long n, const unsigned char *skorig) {

    long long i;
    unsigned char nonce[64], hm[64], sk[32], zero[32];
    gep256 R;
    int ret = -1;

    /* copy m to sm and copy secret key */
    *smlen = n + 64;
    for (i = n - 1; i >= 0; --i) sm[i + 64] = m[i];
    for (i = 31; i >= 0; --i) sm[i + 32] = skorig[i + 32];
    for (i = 31; i >= 0; --i) sk[i     ] = skorig[i     ];
    for (i = 31; i >= 0; --i) zero[i] = 0;

    /* get pseudorandom nonce = H(sk2, m) */
    crypto_hash_sha512(nonce, sm + 32, n + 32);
    scp256_reduce64(nonce);
    if (crypto_verify_32(nonce, zero) == 0) goto fail;

    /* compute R and check whether R != 0 */
    gep256_scalarmult_base(R, nonce);
    if (gep256_tobytes(sm, R) != 0) goto fail;
    if (crypto_verify_32(sm, zero) == 0) goto fail;

    /* calculate hm = H(m) */
    crypto_hash_sha256(hm, sm + 64, n);

    /* compute S and check whether S != 0 */
    scp256_inv(nonce);
    scp256_mul(sm + 32, sm     , sk);
    scp256_add(sm + 32, sm + 32, hm);
    scp256_mul(sm + 32, sm + 32, nonce);
    if (crypto_verify_32(sm + 32, zero) == 0) goto fail;

    ret = 0;
    goto cleanup;

fail:
    for (i = 0; i < 64; ++i) sm[i] = 0;

cleanup:
    cleanup(nonce); cleanup(hm); cleanup(sk); cleanup(R);
    return ret;
}


int crypto_sign_nistp256ecdsa_tinynacl_open(unsigned char *m, unsigned long long *mlen, const unsigned char *sm, unsigned long long n, const unsigned char *pk) {

    long long i;
    unsigned char rcopy[32], scopy[32], tmp[64];
    gep256 R, S, A;
    int ret = -1;

    /* check input */
    if (n < 64) goto fail;

    /* unpack pk */
    if (gep256_frombytes(A, pk) != 0) goto fail;

    /* copy pk, r, s */
    for (i = 0; i < 32; ++i) tmp[i] = 0;
    for (i = 0; i < 32; ++i) rcopy[i] = sm[i];
    for (i = 0; i < 32; ++i) scopy[i] = sm[i + 32];

    /* copy sm to m */
    for (i = n - 1; i >= 0; --i) m[i] = sm[i];

    /* check whether R != 0 and S != 0 */
    scp256_reduce32(rcopy); 
    if (crypto_verify_32(rcopy, tmp) == 0) goto fail;
    scp256_reduce32(scopy);
    if (crypto_verify_32(scopy, tmp) == 0) goto fail;

    /* calculate tmp = H(m) */
    crypto_hash_sha256(tmp, m + 64, n - 64);

    /* compute R */
    scp256_inv(scopy);
    scp256_mul(tmp, tmp, scopy);
    gep256_scalarmult_base(S, tmp);
    scp256_mul(tmp, rcopy, scopy);
    gep256_scalarmult(A, A, tmp);
    gep256_add(R, S, A);

    /* check R */
    if (gep256_tobytes(tmp, R) != 0) goto fail;
    scp256_reduce32(tmp); 
    if (crypto_verify_32(tmp, rcopy) != 0) goto fail;

    /* copy message */
    n -= 64; *mlen = n;
    for (i = n - 1; i >= 0; --i) m[i] = m[i + 64];
    for (i = 0; i < 64; ++i) m[i + n] = 0;
    ret = 0;
    goto cleanup;

fail:
    for (i = 0; i < n; ++i) m[i] = 0;

cleanup:
    cleanup(rcopy); cleanup(scopy); cleanup(tmp);
    cleanup(R); cleanup(S); cleanup(A);
    return ret;
}

/*
creates public key and secret key for signing
secret key has 2 parts:
32B - the discrete logarithm of the public key
32B - is used for k-generation (k is generated by hashing the secret key together with the entire message)
*/
int crypto_sign_nistp256ecdsa_tinynacl_keypair(unsigned char *pk, unsigned char *sk) {

    gep256 A;
    long long i;
    int ret = -1;

    randombytes(sk, 64);                     /* create random initial value */
    crypto_hash_sha512(sk, sk, 64);          /* use crypto_hash(iv) as sk   */

    gep256_scalarmult_base(A, sk);           /* create pk                   */
    if (gep256_tobytes(pk, A) == 0) ret = 0;
    cleanup(A);
    return ret;
}
