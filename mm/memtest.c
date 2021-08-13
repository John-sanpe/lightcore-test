/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#define pr_fmt(fmt) "memtest: " fmt

#include <stddef.h>
#include <mm.h>
#include <memtest.h>
#include <printk.h>

static size_t patterns[] = {
    (size_t)0x0000000000000000ULL,
    (size_t)0xffffffffffffffffULL,
    (size_t)0x5555555555555555ULL,
    (size_t)0xaaaaaaaaaaaaaaaaULL,
};

static void bad_block(void *start, size_t size)
{
    pr_warn("Bad block size %ld @ 0xlx\n", size, start);
    memblock_reserve(va_to_pa(start), size);
}

static void __init memtest_once(size_t pattern, size_t *start, size_t *end)
{
    size_t *walk, *bad_start = NULL;

    for (walk = start; walk < end; ++walk)
        *walk++ = pattern;

    for (walk = start; walk < end; ++walk) {
        if (likely(*walk == pattern))
            continue;
        /* Bad memory start */
        if (!bad_start)
            bad_start = walk;
        /* Bad memory end */
        if (*(walk + 1) == pattern) {
            bad_block(walk, (size_t)walk - (size_t)bad_start + 1);
            bad_start = NULL;
        }
    }
}

void __init memtest(void)
{
    unsigned int c;
    void *va = &_ld_image_end;
    size_t size = 0x10000000;

    for(c = 0; c < ARRAY_SIZE(patterns); ++c)
        memtest_once(patterns[c], va, (void *)((size_t)va + size));
}
