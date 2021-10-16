/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _KTHREAD_H_
#define _KTHREAD_H_

#include <state.h>
#include <list.h>

typedef void (*kcoro_entry_t)(struct kcoro_work *, void *data);

struct kthread_worker {
    struct list_head work;
};

struct kthread_work {
    struct kthread_worker *worker;
    struct list_head list;
    void *data;
};

typedef state (*kthread_t)(void *data);

__printf(2, 3) struct task *
kthread_create(kthread_t kthread, void *data, const char *name, ...);

#endif  /* _KTHREAD_H_ */
