/*
20140423
Jan Mojzis
Public domain.
*/

#include "channel.h"
#include "byte.h"
#include "crypto_uint32.h"
#include "fail.h"
#include "run.h"
#include <pwd.h>
#include <sys/types.h>

const char* user = "user";
const char* termname = "xterm";
crypto_uint32 id = 0;
crypto_uint32 remotewindow = 100;
crypto_uint32 maxpacket = 100;
crypto_uint32 localwindow;
unsigned char ch = 'x';

static char* name(void)
{

    struct passwd* pw;

    pw = getpwuid(geteuid());
    return pw->pw_name;
}

/* channel can't be opened 2x */
static void testopen1(void)
{
    channel_open(user, id, remotewindow, maxpacket, &localwindow);
    channel_open(user, id, remotewindow, maxpacket, &localwindow);
}

/* *localwindow can't be 0 */
static void testopen2(void)
{
    channel_open(user, id, remotewindow, maxpacket, 0);
}

/* maxpacket can't be 0 */
static void testopen3(void)
{
    channel_open(user, id, remotewindow, 0, &localwindow);
}

/* remotewindow can't be 0 */
static void testopen4(void)
{
    channel_open(user, id, 0, maxpacket, &localwindow);
}

/* channel_openterminal() can't be called before channel_open() */
static void testtermopen1(void)
{
    channel_openterminal(termname, 0, 0, 0, 0);
}

/* channel_env() can't be called before channel_open() */
static void testenv1(void)
{
    channel_env("a", "b");
}

/* channel_env() can't be called after channel_exec() */
static void testenv2(void)
{

    channel_open(name(), id, remotewindow, maxpacket, &localwindow);
    channel_exec("exit 0");
    channel_env("a", "b");
}

/* channel_exec() can't be called before channel_open() */
static void testexec1(void)
{
    channel_exec("true");
}

/* channel_put() can't be called before channel_exec() */
static void testput1(void)
{
    channel_open(user, id, remotewindow, maxpacket, &localwindow);
    channel_put(&ch, 1);
}

/* channel_puteof() can't be called before channel_exec() */
static void testputeof1(void)
{
    channel_open(user, id, remotewindow, maxpacket, &localwindow);
    channel_puteof();
}

/* channel_read() can't be called before channel_exec() */
static void testread1(void)
{
    channel_open(user, id, remotewindow, maxpacket, &localwindow);
    channel_read(&ch, 1);
}

/* channel_extendedread() can't be called before channel_exec() */
static void testextendedread1(void)
{
    channel_open(user, id, remotewindow, maxpacket, &localwindow);
    channel_extendedread(&ch, 1);
}

/* channel_write() can't be called before channel_exec() */
static void testwrite1(void)
{
    channel_open(user, id, remotewindow, maxpacket, &localwindow);
    channel_write();
}

/* OK - tests */

static void testok1(void)
{

    int r, s, e;

    channel_open(name(), id, remotewindow, maxpacket, &localwindow);
    channel_exec("exit 0");

    do {
        r = channel_waitnohang(&s, &e);
    } while (r == 0);

    if (s != 0)
        fail("process killed");
    if (e != 0)
        fail("bad status");
    _exit(0);
}

static void testok2(void)
{

    int r, s, e;

    channel_open(name(), id, remotewindow, maxpacket, &localwindow);
    channel_exec("exit 1");

    do {
        r = channel_waitnohang(&s, &e);
    } while (r == 0);

    if (s != 0)
        fail("process killed");
    if (e != 1)
        fail("bad status");
    _exit(0);
}

static void testok3(void)
{

    int r, s, e;

    channel_open(name(), id, remotewindow, maxpacket, &localwindow);
    channel_exec("kill -9 $$");

    do {
        r = channel_waitnohang(&s, &e);
    } while (r == 0);

    if (s != 9)
        fail("bad exit signal");
    _exit(0);
}

static void testok4(void)
{

    int r, s, e;
    unsigned char buf[10];

    channel_open(name(), id, remotewindow, maxpacket, &localwindow);
    channel_exec("cat");

    ch = 'a';
    channel_put(&ch, 1);
    do {
        r = channel_write();
    } while (r == 0);

    do {
        r = channel_read(buf, sizeof buf);
    } while (r == 0);

    if (r != 1)
        fail("channel_write()/channel_read() failure");
    if (buf[0] != 'a')
        fail("channel_write()/channel_read() failure");

    channel_puteof();

    do {
        r = channel_waitnohang(&s, &e);
    } while (r == 0);

    if (s != 0)
        fail("process killed");
    if (e != 0)
        fail("bad status");
    _exit(0);
}

int main(void)
{

    run_mustfail(testopen1);
    run_mustfail(testopen2);
    run_mustfail(testopen3);
    run_mustfail(testopen4);

    run_mustfail(testtermopen1);

    run_mustfail(testenv1);
    run_mustfail(testenv2);

    run_mustfail(testexec1);

    run_mustfail(testput1);

    run_mustfail(testputeof1);

    run_mustfail(testread1);

    run_mustfail(testextendedread1);

    run_mustfail(testwrite1);

    run_mustpass(testok1);
    run_mustpass(testok2);
    run_mustpass(testok3);
    run_mustpass(testok4);

    _exit(0);
}
