/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#include <size.h>
#include <initcall.h>
#include <timekeeping.h>
#include <vmalloc.h>
#include <string.h>
#include <selftest.h>

struct strnicmp_pdata {
    char mempool_a[SZ_1MiB];
    char mempool_b[SZ_1MiB];
};

static state strnicmp_testing(void *pdata)
{
    struct strnicmp_pdata *mdata = pdata;
    ktime_t start = timekeeping_get_time();
    unsigned int count = 0;
    char sbuff[32];

    do {
        if (strnicmp(mdata->mempool_b, mdata->mempool_a, SZ_1MiB))
            return -EFAULT;
        count++;
    } while (ktime_after(
        start, ktime_sub_ms
        (timekeeping_get_time(), 1000)
    ));

    gsize(sbuff, count * SZ_1MiB);
    kshell_printf("strnicmp bandwidth: %s/s\n", sbuff);
    return -ENOERR;
}

static void *strnicmp_prepare(int argc, char *argv[])
{
    struct strnicmp_pdata *pdata;
    unsigned int count;

    pdata = vmalloc(sizeof(*pdata));
    if (!pdata)
        return ERR_PTR(-ENOMEM);

    for (count = 0; count < SZ_1MiB; ++count)
        pdata->mempool_a[count] =
        pdata->mempool_b[count] = count % UINT8_MAX | 1;

    return pdata;
}

static void strnicmp_release(void *pdata)
{
    vfree(pdata);
}

static struct selftest_command strnicmp_command = {
    .group = "string",
    .name = "strnicmp",
    .desc = "benchmark strnicmp",
    .testing = strnicmp_testing,
    .prepare = strnicmp_prepare,
    .release = strnicmp_release,
};

static state selftest_strnicmp_init(void)
{
    return selftest_register(&strnicmp_command);
}
kshell_initcall(selftest_strnicmp_init);
