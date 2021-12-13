/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _ASM_ARM_PAGE_H_
#define _ASM_ARM_PAGE_H_

#include <const.h>

#define PAGE_SHIFT      12
#define PAGE_SIZE       (_AC(1,UL) << PAGE_SHIFT)
#define PAGE_MASK       (~(PAGE_SIZE - 1))

#define THREAD_SHIFT    (PAGE_SHIFT + 1)
#define THREAD_SIZE     (_AC(1,UL) << THREAD_SHIFT)
#define THREAD_MASK     (~(THREAD_SIZE - 1))

#define PHYS_SHIFT      32
#define PHYS_SIZE       (_AC(1,ULL) << PHYS_SHIFT)
#define PHYS_MASK       (~(PHYS_SIZE - 1))

#define VIRTS_SHIFT     32
#define VIRTS_SIZE      (_AC(1,ULL) << VIRTS_SHIFT)
#define VIRTS_MASK      (~(VIRTS_SHIFT - 1))

#define PGDIR_SHIFT     20
#define PGDIR_SIZE      (_AC(1,UL) << PGDIR_SHIFT)
#define PGDIR_MASK      (~(PGDIR_SHIFT - 1))

#define PGDIR_COARSE_SHIFT  10
#define PTRS_PER_PGD    4096
#define PTRS_PER_PTE    256

#define SPARCE_SHIFT    24

#include <asm-generic/page.h>

#endif
