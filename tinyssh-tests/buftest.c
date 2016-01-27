/*
20140303
Jan Mojzis
Public domain.
*/

#include "buf.h"
#include "byte.h"
#include "crypto_uint32.h"
#include "crypto_uint8.h"
#include "fail.h"
#include "str.h"
#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

/* XXX */
static int xbuf_putnum8_(const char* fn, unsigned long long line,
    struct buf* b, long long uu)
{
    crypto_uint8 u = uu;
    return buf_putnum8_(fn, line, b, u);
}
static int xbuf_putnum32_(const char* fn, unsigned long long line,
    struct buf* b, long long uu)
{
    crypto_uint32 u = uu;
    return buf_putnum32_(fn, line, b, u);
}

struct vectors1 {
    int (*op)(const char*, unsigned long long, struct buf*, long long);
    long long spacelen;
    long long xlen;
} testvectors1[] = {
    { buf_putzerobytes_, 5, 4 },
    { buf_putrandombytes_, 5, 4 },
    { buf_putpadding_, 5, 4 },
    { xbuf_putnum32_, 5, 0 },
    { xbuf_putnum8_, 2, 0 },
    { 0, 0, 0 }
};

struct vectors2 {
    int (*op)(const char*, unsigned long long, struct buf*, const unsigned char*, long long);
    long long spacelen;
    const unsigned char* x;
    long long xlen;
} testvectors2[] = {
    { buf_put_, 1, (unsigned char*)"", 0 },
    { buf_put_, 5, (unsigned char*)"ahoj", 4 },
    { buf_putstringlen_, 5, (unsigned char*)"", 0 },
    { buf_putstringlen_, 9, (unsigned char*)"ahoj", 4 },
    { 0, 0, 0, 0 }
};

struct vectors3 {
    int (*op)(const char*, unsigned long long, struct buf*, const char*);
    long long spacelen;
    const char* x;
} testvectors3[] = {
    { buf_puts_, 1, "" },
    { buf_puts_, 5, "ahoj" },
    { buf_putstring_, 5, "" },
    { buf_putstring_, 9, "ahoj" },
    { 0, 0, 0 }
};

static int _test1a1(long long spacelen, int (*op)(const char*, unsigned long long, struct buf*, long long), long long xlen)
{

    pid_t pid;
    int status;
    unsigned char bspace[10];
    struct buf b;

    pid = fork();
    if (pid == -1)
        return -1;
    if (pid == 0) {
        close(2);
        buf_init(&b, bspace, spacelen);
        op(__FILE__, __LINE__, &b, xlen);
        buf_put(&b, (unsigned char*)"", 1); /* overflow */
        _exit(0);
    }

    while (waitpid(pid, &status, 0) != pid) {
    };
    if (!WIFEXITED(status))
        return -1;
    return WEXITSTATUS(status);
}

static int _test1a2(long long spacelen, int (*op)(const char*, unsigned long long, struct buf*, const unsigned char*, long long), const unsigned char* x, long long xlen)
{

    pid_t pid;
    int status;
    unsigned char bspace[10];
    struct buf b;

    pid = fork();
    if (pid == -1)
        return -1;
    if (pid == 0) {
        close(2);
        buf_init(&b, bspace, spacelen);
        op(__FILE__, __LINE__, &b, x, xlen);
        buf_put(&b, (unsigned char*)"", 1); /* overflow */
        _exit(0);
    }

    while (waitpid(pid, &status, 0) != pid) {
    };
    if (!WIFEXITED(status))
        return -1;
    return WEXITSTATUS(status);
}

static int _test1a3(long long spacelen, int (*op)(const char*, unsigned long long, struct buf*, const char*), const char* x)
{

    pid_t pid;
    int status;
    unsigned char bspace[10];
    struct buf b;

    pid = fork();
    if (pid == -1)
        return -1;
    if (pid == 0) {
        close(2);
        buf_init(&b, bspace, spacelen);
        op(__FILE__, __LINE__, &b, x);
        buf_put(&b, (unsigned char*)"", 1); /* overflow */
        _exit(0);
    }

    while (waitpid(pid, &status, 0) != pid) {
    };
    if (!WIFEXITED(status))
        return -1;
    return WEXITSTATUS(status);
}

static void _test1b1(long long spacelen, int (*op)(const char*, unsigned long long, struct buf*, long long), long long xlen)
{

    unsigned char bspace[10];
    struct buf b;

    buf_init(&b, bspace, spacelen);
    op(__FILE__, __LINE__, &b, xlen);
}

static void _test1b2(long long spacelen, int (*op)(const char*, unsigned long long, struct buf*, const unsigned char*, long long), const unsigned char* x, long long xlen)
{

    unsigned char bspace[10];
    struct buf b;

    buf_init(&b, bspace, spacelen);
    op(__FILE__, __LINE__, &b, x, xlen);
}

static void _test1b3(long long spacelen, int (*op)(const char*, unsigned long long, struct buf*, const char*), const char* x)
{

    unsigned char bspace[10];
    struct buf b;

    buf_init(&b, bspace, spacelen);
    op(__FILE__, __LINE__, &b, x);
}

static void test1(void)
{

    long long i;
    int r;

    for (i = 0; testvectors1[i].op; ++i) {
        r = _test1a1(testvectors1[i].spacelen, testvectors1[i].op, testvectors1[i].xlen);
        if (r == 0)
            fail("failure");
        _test1b1(testvectors1[i].spacelen, testvectors1[i].op, testvectors1[i].xlen);
    }

    for (i = 0; testvectors2[i].op; ++i) {
        r = _test1a2(testvectors2[i].spacelen, testvectors2[i].op, testvectors2[i].x, testvectors2[i].xlen);
        if (r == 0)
            fail("failure");
        _test1b2(testvectors2[i].spacelen, testvectors2[i].op, testvectors2[i].x, testvectors2[i].xlen);
    }

    for (i = 0; testvectors3[i].op; ++i) {
        r = _test1a3(testvectors3[i].spacelen, testvectors3[i].op, testvectors3[i].x);
        if (r == 0)
            fail("failure");
        _test1b3(testvectors3[i].spacelen, testvectors3[i].op, testvectors3[i].x);
    }
}

static void test2(void)
{

    unsigned char bspace[2];
    struct buf b;
    unsigned char ch = 0;
    long long i;

    buf_init(&b, bspace, sizeof bspace);

    for (i = 0; i < 10; ++i) {
        buf_put(&b, &ch, 1);
        buf_purge(&b);
    }
}

struct vectors64 {
    const char* in;
    long long inlen;
    const char* out;
} testvectors64[] = {
    { "\107\303\221\307\306\077\106\014\235\115\356\215\375", 13, "R8ORx8Y/RgydTe6N/Q==" },
    { "\133\253\044\246\070\374", 6, "W6skpjj8" },
    { "\253\374\323\142\326", 5, "q/zTYtY=" },
    { "\335\251\125\150\245\321\040\143\174\152", 10, "3alVaKXRIGN8ag==" },
    { "\155\065\344\122\353", 5, "bTXkUus=" },
    { "\266\071\073\373\072\035\270\151\201", 9, "tjk7+zoduGmB" },
    { "\302\373\135\206\370\012\365", 7, "wvtdhvgK9Q==" },
    { "\071\203", 2, "OYM=" },
    { "\210\236\076\336\014\122\125\050\016\076\063", 11, "iJ4+3gxSVSgOPjM=" },
    { "\241\330\251\122\325\061\142\167\174\315\201\000\275\157\056", 15, "odipUtUxYnd8zYEAvW8u" },
    { "\334\306\014\014\021\323\000\120\347\363\335", 11, "3MYMDBHTAFDn890=" },
    { "\356\243\102\105\343\146\154\064\006\316\360\301\256\164", 14, "7qNCReNmbDQGzvDBrnQ=" },
    { "\250\211\165\344\276\224\031\205\162\315", 10, "qIl15L6UGYVyzQ==" },
    { "\356\104\051\125\022\121\070\074\332\301\014\377\303\254\140", 15, "7kQpVRJRODzawQz/w6xg" },
    { "\065\340\116\113\133\334\100", 7, "NeBOS1vcQA==" },
    { "\151\340\101\120\376", 5, "aeBBUP4=" },
    { "\345", 1, "5Q==" },
    { "\010\134", 2, "CFw=" },
    { "\221\044\355\143\047\351\127\165\314\370\017\347\365", 13, "kSTtYyfpV3XM+A/n9Q==" },
    { "\222\237\301\021\214\011\002\367", 8, "kp/BEYwJAvc=" },
    { "\235\245\251\304\303\232\223\046\063\062\252\366", 12, "naWpxMOakyYzMqr2" },
    { "\215\267\333\017\237\313\210\114\067\164\067\337\024\211\174", 15, "jbfbD5/LiEw3dDffFIl8" },
    { "\344\122\212\142\231\251\146\253", 8, "5FKKYpmpZqs=" },
    { "\147\105", 2, "Z0U=" },
    { "\013\235\177\100\212\167\123\172\201", 9, "C51/QIp3U3qB" },
    { "\102\136\120\260\230\125\046\253\323\305\044", 11, "Ql5QsJhVJqvTxSQ=" },
    { "\101\152\200\060\134\014\124\202\023\057\250\115\341", 13, "QWqAMFwMVIITL6hN4Q==" },
    { "\012\310\224\166\265\072\072\270\052\262\147\145\022", 13, "CsiUdrU6OrgqsmdlEg==" },
    { "\021\131\104\067\051\316", 6, "EVlENynO" },
    { "\104\323", 2, "RNM=" },
    { "\070\274\150\150\014\077", 6, "OLxoaAw/" },
    { "\100\132\201\357\023\222\243\301\357", 9, "QFqB7xOSo8Hv" },
    { "\225\244", 2, "laQ=" },
    { "\212\270\067\052\145\203", 6, "irg3KmWD" },
    { "\363\252\326\213\370\065\072\154\077\335\202\242\236\274", 14, "86rWi/g1Omw/3YKinrw=" },
    { "\141\041\315\033", 4, "YSHNGw==" },
    { "\216\257\025\305\362\044\264\133", 8, "jq8VxfIktFs=" },
    { "\342\377\135\236", 4, "4v9dng==" },
    { "\121\100\077\162\374\162\274\121\100", 9, "UUA/cvxyvFFA" },
    { "\204", 1, "hA==" },
    { "\306\345\332\023\241\273\064\227\020\326\102\123\003", 13, "xuXaE6G7NJcQ1kJTAw==" },
    { "\227\142\207\137\322\344\074\317\007\245\120", 11, "l2KHX9LkPM8HpVA=" },
    { "\267\051\051", 3, "tykp" },
    { "\005\244", 2, "BaQ=" },
    { "\215\060\165\046\210", 5, "jTB1Jog=" },
    { "\100\376\271\317\250\013\264\360\330\012\325\103", 12, "QP65z6gLtPDYCtVD" },
    { "\233\340\212\127\160\167\077", 7, "m+CKV3B3Pw==" },
    { "\200\041\036\206\273\067", 6, "gCEehrs3" },
    { "\207\067\003\324\025\351\033\223\226\300\002\255\167\342\316", 15, "hzcD1BXpG5OWwAKtd+LO" },
    { "\263\364\115\054\347\350", 6, "s/RNLOfo" },
    { "\211\252\065\167\224", 5, "iao1d5Q=" },
    { "\024\327\120\076\031\035\104\003\247", 9, "FNdQPhkdRAOn" },
    { "\143\302\131\030\204\134\036\023\305\203\010\236\204\303", 14, "Y8JZGIRcHhPFgwiehMM=" },
    { "\124\225\066\007", 4, "VJU2Bw==" },
    { "\173\043\162\031\330\351", 6, "eyNyGdjp" },
    { "\323\112\227\176\021\120\311\202\035\041\153\326\137", 13, "00qXfhFQyYIdIWvWXw==" },
    { "\235\006\017\301\200\121\013\230\373\264\215\256\322\126\111", 15, "nQYPwYBRC5j7tI2u0lZJ" },
    { "\317\113\144\056\014\120\246\143\325\272", 10, "z0tkLgxQpmPVug==" },
    { "\106\136\226\023\146\054\366\070\007\270\345\321\245\304\177\147", 16, "Rl6WE2Ys9jgHuOXRpcR/Zw==" },
    { "\042\163\374\234\332\173\055\166", 8, "InP8nNp7LXY=" },
    { "\375\011\321\370\300", 5, "/QnR+MA=" },
    { "\034", 1, "HA==" },
    { "\350", 1, "6A==" },
    { "\147\136\210\264\353\267\311\341\073\110\270\071\033", 13, "Z16ItOu3yeE7SLg5Gw==" },
    { "\133\341\131\206\211\171", 6, "W+FZhol5" },
    { "\146\176\236\340\264\370\270\313\261\304\217", 11, "Zn6e4LT4uMuxxI8=" },
    { "\347\111\263\244\131\103\022\272\163\100\337\324\340\247\006", 15, "50mzpFlDErpzQN/U4KcG" },
    { "\132\167\314\125\023\172\223\140\264\146\167\156\245", 13, "WnfMVRN6k2C0ZndupQ==" },
    { "\206", 1, "hg==" },
    { "\000\346\367\051\051\116", 6, "AOb3KSlO" },
    { "\253\117\134\331", 4, "q09c2Q==" },
    { "\267\130\307\170", 4, "t1jHeA==" },
    { "\252\367\311\250\126\031\001\065\107", 9, "qvfJqFYZATVH" },
    { "\204\011\351\022\206\076\056\334\100", 9, "hAnpEoY+LtxA" },
    { "\004\302\141\333\230\255\215", 7, "BMJh25itjQ==" },
    { "\052\357\233\266\237\017\260\101\000", 9, "Ku+btp8PsEEA" },
    { "\302\024\373\144\071\312\012\044\304\340\026\222\066\216", 14, "whT7ZDnKCiTE4BaSNo4=" },
    { "\342\054\344\331\312\200\052", 7, "4izk2cqAKg==" },
    { "\156\234\310\023\077\041\310\005\137\175\034\307\247", 13, "bpzIEz8hyAVffRzHpw==" },
    { "\051\316\243\225\076\257", 6, "Kc6jlT6v" },
    { "\275\263\026\127\315\140\172\110\002\365\067\061\270\331", 14, "vbMWV81gekgC9TcxuNk=" },
    { "\240\021", 2, "oBE=" },
    { "\125\161\171\026\354", 5, "VXF5Fuw=" },
    { "\052\131", 2, "Klk=" },
    { "\335\234\230\006\054\265\273\124\147\131\015\054\036\335\353", 15, "3ZyYBiy1u1RnWQ0sHt3r" },
    { "\073\126\160\312\341\345\234\353\264\245\115\220\316\063\102", 15, "O1ZwyuHlnOu0pU2QzjNC" },
    { "\200\070\242\226\226\027\147", 7, "gDiilpYXZw==" },
    { "\132\111\013\325\224\115\107\236\124\310\144\262\156\001\004\227", 16, "WkkL1ZRNR55UyGSybgEElw==" },
    { "\340\357\121\156\255\173\153\221\125\024", 10, "4O9Rbq17a5FVFA==" },
    { "\060\037\251\063\073\022\206\033", 8, "MB+pMzsShhs=" },
    { "\355\241", 2, "7aE=" },
    { "\144\033\132\321\153\017\244\145\211\027\313\067\166\167\104", 15, "ZBta0WsPpGWJF8s3dndE" },
    { "\073\227\264\070\151\002", 6, "O5e0OGkC" },
    { "\201\364\206\002\033\125\075\110\036\152\347\011\146\077\023\354", 16, "gfSGAhtVPUgeaucJZj8T7A==" },
    { "\232\205\305\006\214\266\355\107\007\314\223\254\142\055\045\260", 16, "moXFBoy27UcHzJOsYi0lsA==" },
    { "\053\221\122\341\163", 5, "K5FS4XM=" },
    { "\245\113\335\066\012\245\334\335\006\350\373\254\121\346", 14, "pUvdNgql3N0G6PusUeY=" },
    { "\250\126\360\135\053\301\342\200", 8, "qFbwXSvB4oA=" },
    { "\154\323\054\236\175\252\270\270\220", 9, "bNMsnn2quLiQ" },
    { "\322\260\320\175\263", 5, "0rDQfbM=" },
    { "\136\073\307\102\376\033\002\243", 8, "XjvHQv4bAqM=" },
    { "\361\323\236", 3, "8dOe" },
    { "\143\007\375\015", 4, "Ywf9DQ==" },
    { "\122\240\036\350\132\212\037\134\114", 9, "UqAe6FqKH1xM" },
    { "\213\004\377\313\312\314\144\350\055\147\062\064\270\273\150\221", 16, "iwT/y8rMZOgtZzI0uLtokQ==" },
    { "\246\346\106\301\277\300\100", 7, "puZGwb/AQA==" },
    { "\176\034\360\143\160\016\072\017\322", 9, "fhzwY3AOOg/S" },
    { "\046\335\103\356\145\364\266\165\356\146", 10, "Jt1D7mX0tnXuZg==" },
    { "\147\231\300\372\053\373\375\363\257\071", 10, "Z5nA+iv7/fOvOQ==" },
    { "\241", 1, "oQ==" },
    { "\044\231\150\021\346\347", 6, "JJloEebn" },
    { "\026\361\151\071\101", 5, "FvFpOUE=" },
    { "\274\021\324\260\123\343", 6, "vBHUsFPj" },
    { "\255\312", 2, "rco=" },
    { "\012\362\172\166", 4, "CvJ6dg==" },
    { "\223\117\213\351\120\321", 6, "k0+L6VDR" },
    { "\314\150\017\355\321\372", 6, "zGgP7dH6" },
    { "\262\152\141\170\227\353\363\162\116\327\247\342\302\326\255\245", 16, "smpheJfr83JO16fiwtatpQ==" },
    { "\276\336\043\244\015\360\114\273\113", 9, "vt4jpA3wTLtL" },
    { "\307\075\305\152\226\265\045\170", 8, "xz3Fapa1JXg=" },
    { "\342\210\307\244\132\345", 6, "4ojHpFrl" },
    { "\370\125\040\013\267\004\057\371\155\321\107\174\241\041", 14, "+FUgC7cEL/lt0Ud8oSE=" },
    { "\327\111\202\040\314\147\164\116\217\217\220", 11, "10mCIMxndE6Pj5A=" },
    { "\176", 1, "fg==" },
    { "\107\326\002\256\146\112\137\304\013\020\020\163\264\213\145\141", 16, "R9YCrmZKX8QLEBBztItlYQ==" },
    { "\216", 1, "jg==" },
    { "\327\246\371\266\023", 5, "16b5thM=" },
    { "\027\352\164\125\266\241\266\272", 8, "F+p0Vbahtro=" },
    { "\001\135\250\345", 4, "AV2o5Q==" },
    { "\237\302\312\152\355\022\161\240\000\151\330", 11, "n8LKau0ScaAAadg=" },
    { "\343\144\171\236\255\261\275\315\300\026\346", 11, "42R5nq2xvc3AFuY=" },
    { "\371\206\264\033\200\232\373\003\237\061\120\051\353", 13, "+Ya0G4Ca+wOfMVAp6w==" },
    { "\315\220\061\106", 4, "zZAxRg==" },
    { "\007", 1, "Bw==" },
    { "\125\217\033\216\066\340\143\254\133\004\165\110\143\117\146", 15, "VY8bjjbgY6xbBHVIY09m" },
    { "\050\073\200\116\346\355\073\205\061\266\161\033\125\301", 14, "KDuATubtO4UxtnEbVcE=" },
    { "\233\140\272\145\333", 5, "m2C6Zds=" },
    { "\045\256\173\034\034\373\051\205\041\016\070\212\023\073\204\267", 16, "Ja57HBz7KYUhDjiKEzuEtw==" },
    { "\342\254\000\251\017\204\350\152\211\106\116\323\146\262\067\031", 16, "4qwAqQ+E6GqJRk7TZrI3GQ==" },
    { "\100\246\015\313\000\366\205\367\273\053", 10, "QKYNywD2hfe7Kw==" },
    { "\353\127\334\173\217\266\372\214\374", 9, "61fce4+2+oz8" },
    { "\144\243\162\123\006\211\035\337\342\174\071\157\244\316\063\154", 16, "ZKNyUwaJHd/ifDlvpM4zbA==" },
    { "\013\371\150\050", 4, "C/loKA==" },
    { "\340\214\237\210\113\145\007\077\136\351", 10, "4IyfiEtlBz9e6Q==" },
    { "\336\055\356\341\212\040", 6, "3i3u4Yog" },
    { "\354\316\211\157\113\304\374\267\010\137\264\043\220\306\210\125", 16, "7M6Jb0vE/LcIX7QjkMaIVQ==" },
    { "\355\116\363\331\314\366\005\230\253\137", 10, "7U7z2cz2BZirXw==" },
    { "\017\353\200\145\335\054\106\231", 8, "D+uAZd0sRpk=" },
    { "\017\146\253\337\150\156\326\076\105\040\026\154\133", 13, "D2ar32hu1j5FIBZsWw==" },
    { "\072", 1, "Og==" },
    { "\163\107\062\261\176\103\261\267\031\214\346\350\124\101\257", 15, "c0cysX5DsbcZjOboVEGv" },
    { "\333\005\104\376\205\323\203", 7, "2wVE/oXTgw==" },
    { "\072", 1, "Og==" },
    { "\246\270\237\262\261\252\220\273", 8, "prifsrGqkLs=" },
    { "\076\140", 2, "PmA=" },
    { "\343\043\335\346\134\003\240\244\067\271\105\311", 12, "4yPd5lwDoKQ3uUXJ" },
    { "\327\375\152\126", 4, "1/1qVg==" },
    { "\103\231\176\252\263\126\047\356\370\266\027\072\003\162", 14, "Q5l+qrNWJ+74thc6A3I=" },
    { "\373\321", 2, "+9E=" },
    { "\127\052\054\340\245\243\261", 7, "Vyos4KWjsQ==" },
    { "\027\236\317\116\373\155\111\057\026\371\247\227\055\242", 14, "F57PTvttSS8W+aeXLaI=" },
    { "\175\166\256\272\045\302\221\306\237\151\116\031\223\233", 14, "fXauuiXCkcafaU4Zk5s=" },
    { "\140\241", 2, "YKE=" },
    { "\064\332\317\246\113\370\276\341\302", 9, "NNrPpkv4vuHC" },
    { "\245\306\176\341\217\000\073\265\237\124", 10, "pcZ+4Y8AO7WfVA==" },
    { "\026\173\274\034\340\061\376\246\233\375\326\250\164\313\304\353", 16, "Fnu8HOAx/qab/daodMvE6w==" },
    { "\277\034", 2, "vxw=" },
    { "\215\120\005\160\173\000\153\373\026\123\045\074\142\337", 14, "jVAFcHsAa/sWUyU8Yt8=" },
    { "\221\122\070\352\346\035\227\270\342\066\237\170\266", 13, "kVI46uYdl7jiNp94tg==" },
    { "\274\270", 2, "vLg=" },
    { "\341\155\367\111\106\042\036\112\242\262\004\354\131\153", 14, "4W33SUYiHkqisgTsWWs=" },
    { "\254\103\176\060", 4, "rEN+MA==" },
    { "\277\305\046\061\037\111\121\315", 8, "v8UmMR9JUc0=" },
    { "\220\155\334\167\323\340\065\301\101\014\101\277", 12, "kG3cd9PgNcFBDEG/" },
    { "\117", 1, "Tw==" },
    { "\152\165\104\351\003\100\006\311\061\045", 10, "anVE6QNABskxJQ==" },
    { "\171\020\056\304\213", 5, "eRAuxIs=" },
    { "\263\317\223\332\321\036\203\241\016\355\173\163\060\121", 14, "s8+T2tEeg6EO7XtzMFE=" },
    { "\051\046\035\170\002", 5, "KSYdeAI=" },
    { "\214\324\001\125\062\247\010\122\147\376\021\031", 12, "jNQBVTKnCFJn/hEZ" },
    { "\035\126\117\235\225\176\135\370\347", 9, "HVZPnZV+Xfjn" },
    { "\334", 1, "3A==" },
    { "\203\117\166\021\144\257", 6, "g092EWSv" },
    { "\262\155", 2, "sm0=" },
    { "\241\242\031\110\014\223\340\175\311", 9, "oaIZSAyT4H3J" },
    { "\074\373\225\217\164\140\125\353\300\256\146\202\301\133", 14, "PPuVj3RgVevArmaCwVs=" },
    { "\336\354\143\220\367\007\342\114\007\040\331\362\245", 13, "3uxjkPcH4kwHINnypQ==" },
    { "\144\107\331\316\274\005\264", 7, "ZEfZzrwFtA==" },
    { "\027\371\125\116\174\275\160\220\306\266", 10, "F/lVTny9cJDGtg==" },
    { "\155\315\167\325\171\213\242\064\211\122\217\335\250\154", 14, "bc131XmLojSJUo/dqGw=" },
    { "\126\075\332\131\265\225\065\337\130\133\314\337", 12, "Vj3aWbWVNd9YW8zf" },
    { "\051\032\131\176\353\047\001\037\304\170\136\155\222\344", 14, "KRpZfusnAR/EeF5tkuQ=" },
    { "\323\011\213\013\160\037\274\235\026\155\352\234", 12, "0wmLC3AfvJ0Wbeqc" },
    { "\174\260\057\114\125\272\330\213\141\034\130\277", 12, "fLAvTFW62IthHFi/" },
    { "\234\376\012\217\376\061\152\202\177", 9, "nP4Kj/4xaoJ/" },
    { "\162\267\211\246\213\237\325\136\140\143\173", 11, "creJpouf1V5gY3s=" },
    { "\224\317\166\131\020\266\045", 7, "lM92WRC2JQ==" },
    { "\075\002\145\273\112\375\070\101\311\125\337\070\003\336\363", 15, "PQJlu0r9OEHJVd84A97z" },
    { "\177\105\134\344\106\350\034\205", 8, "f0Vc5EboHIU=" },
    { "\130\336\125\300\053\331\050", 7, "WN5VwCvZKA==" },
    { 0, 0, 0 }
};

static void test3(void)
{

    unsigned char bspace[100];
    struct buf b;
    long long i, outlen;

    buf_init(&b, bspace, sizeof bspace);

    for (i = 0; testvectors64[i].in; ++i) {
        buf_purge(&b);
        outlen = str_len(testvectors64[i].out);
        buf_putbase64(&b, (unsigned char*)testvectors64[i].in, testvectors64[i].inlen);
        if (b.len != outlen || !byte_isequal(b.buf, outlen, testvectors64[i].out)) {
            fail("buf_putbase64 failed");
        }
    }
}

static void test4(void)
{

    unsigned char bspace[2];
    struct buf b;

    buf_init(&b, bspace, 1);
    if (buf_ready(&b, 1))
        fail("buf_ready failed");

    buf_init(&b, bspace, 2);
    if (!buf_ready(&b, 1))
        fail("buf_ready failed");
}

struct vectorssh {
    const char* in;
    long long inlen;
    const char* out;
    long long outlen;
} testvectorssh[] = {
    { "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0", 32, "\0\0\0\0", 4 },
    { "\1\1\1\1\1\1\1\1\1\1\1\1\1\1\1\1\1\1\1\1\1\1\1\1\1\1\1\1\1\1\1\1", 32, "\0\0\0\040\1\1\1\1\1\1\1\1\1\1\1\1\1\1\1\1\1\1\1\1\1\1\1\1\1\1\1\1\1\1\1\1", 36 },
    { "\377\1\1\1\1\1\1\1\1\1\1\1\1\1\1\1\1\1\1\1\1\1\1\1\1\1\1\1\1\1\1\1", 32, "\0\0\0\041\0\377\1\1\1\1\1\1\1\1\1\1\1\1\1\1\1\1\1\1\1\1\1\1\1\1\1\1\1\1\1\1\1", 37 },
    { 0, 0, 0, 0 },
};

static unsigned char bspacesh[64];
static struct buf b;

static void test5(void)
{

    long long i;

    buf_init(&b, bspacesh, sizeof bspacesh);

    for (i = 0; testvectorssh[i].in; ++i) {
        buf_purge(&b);
        buf_putsharedsecret(&b, (unsigned char*)testvectorssh[i].in, testvectorssh[i].inlen);
        if (b.len != testvectorssh[i].outlen || !byte_isequal(b.buf, b.len, testvectorssh[i].out)) {
            fail("buf_putbasesharedsecret failed");
        }
    }
}

int main(void)
{

    test1();
    test2();
    test3();
    test4();
    test5();
    _exit(0);
}
