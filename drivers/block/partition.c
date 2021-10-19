/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#define pr_fmt(fmt) "part: " fmt

#include <string.h>
#include <driver/block.h>
#include <driver/block/partition.h>
#include <export.h>
#include <printk.h>

static SLIST_HEAD(partition_list);

static struct partition_type *partition_find(const char *name)
{
    struct partition_type *part;

    slist_for_each_entry(part, &partition_list, list)
        if (!strcmp(part->name, name))
            return part;

    return NULL;
}

state partition_scan(struct block_device *bdev)
{
    struct partition_type *part;
    state ret;

    slist_for_each_entry(part, &partition_list, list) {
        ret = part->match(bdev);
        if (ret)
            return ret;
    }

    return -ENOERR;
}

state partition_register(struct partition_type *part)
{
    if (!part->name || !part->match)
        return -EINVAL;

    if (partition_find(part->name)) {
        pr_err("part %s already registered\n", part->name);
        return -EINVAL;
    }

    slist_add(&partition_list, &part->list);
    return -ENOERR;
}

void partition_unregister(struct partition_type *part)
{
    slist_del(&partition_list, &part->list);
}

EXPORT_SYMBOL(partition_register);
EXPORT_SYMBOL(partition_unregister);
