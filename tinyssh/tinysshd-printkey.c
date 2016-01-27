/*
20150124
Jan Mojzis
Public domain.
*/

#include "buf.h"
#include "crypto.h"
#include "die.h"
#include "e.h"
#include "global.h"
#include "load.h"
#include "log.h"
#include "sshcrypto.h"
#include "writeall.h"
#include <unistd.h>

static struct buf b1 = { global_bspace1, 0, sizeof global_bspace1 }; /* reusing global buffer */
static struct buf b2 = { global_bspace2, 0, sizeof global_bspace2 }; /* reusing global buffer */
static unsigned char pk[sshcrypto_sign_PUBLICKEYMAX];

#define USAGE "\
\n\
 name:\n\
   tinysshd-printkey - print public-keys TinySSH keys\n\
\n\
 syntax:\n\
   tinysshd-printkey keydir\n\
\n\
 description:\n\
   tinysshd-printkey - prints TinySSH public-keys in base64 format.\n\
   \n\
\n\
 options:\n\
   keydir: directory containing secret and public TinySSH keys\n\
\n\
"

int main(int argc, char** argv)
{

    char* x;
    long long i;

    log_init(3, "tinysshd-printkey", 0, 0);

    if (argc < 2)
        die_usage(USAGE);
    if (!argv[0])
        die_usage(USAGE);
    for (;;) {
        if (!argv[1])
            break;
        if (argv[1][0] != '-')
            break;
        x = *++argv;
        if (x[0] == '-' && x[1] == 0)
            break;
        if (x[0] == '-' && x[1] == '-' && x[2] == 0)
            break;
        while (*++x) {
            die_usage(USAGE);
        }
    }
    x = *++argv;
    if (!x)
        die_usage(USAGE);

    if (chdir(x) == -1)
        die_fatal("unable to chdir to directory", x, 0);

    /* read public keys */
    for (i = 0; sshcrypto_keys[i].name; ++i) {
        if (load(sshcrypto_keys[i].sign_publickeyfilename, pk, sshcrypto_keys[i].sign_publickeybytes) == -1) {
            if (errno == ENOENT)
                continue;
            die_fatal("unable to read public key from file", x, sshcrypto_keys[i].sign_publickeyfilename);
        }

        buf_purge(&b1);
        sshcrypto_keys[i].buf_putsignpk(&b1, pk);
        if (b1.len < 4)
            die_fatal("unable to get publickey", 0, 0);

        /* base64 */
        buf_purge(&b2);
        buf_puts(&b2, sshcrypto_keys[i].name);
        buf_puts(&b2, " ");
        buf_putbase64(&b2, b1.buf + 4, b1.len - 4);
        buf_puts(&b2, "\n");
        if (writeall(1, b2.buf, b2.len) == -1)
            die_fatal("unable to write output", 0, 0);
    }
    global_die(0);
    return 111;
}
