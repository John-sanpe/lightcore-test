/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _MM_MEMBLOCK_H_
#define _MM_MEMBLOCK_H_

#include <types.h>
#include <state.h>
#include <list.h>
#include <numa.h>
#include <asm/page.h>

#ifndef MEMBLOCK_ALLOC_ADDR
# define MEMBLOCK_ALLOC_ADDR NORMAL_OFFSET
#endif

#ifndef MEMBLOCK_ALLOC_LIMIT
# define MEMBLOCK_ALLOC_LIMIT CONFIG_HIGHMAP_OFFSET
#endif

/**
 * enum memblock_type - memblock state types.
 * @usable: memory that can be used
 * @allocated: allocated by memblock
 * @takeover: buddy system takeover
 * @reserved: nonexistent memory
 */
enum memblock_type {
    MEMBLOCK_USABLE     = 0x00,
    MEMBLOCK_ALLOCATED  = 0x01,
    MEMBLOCK_TAKEOVER   = 0x02,
    MEMBLOCK_RESERVED   = 0x03,
};

/**
 * struct memblock_region - describe a memory region
 * @name: register name of region
 * @addr: base address of region
 * @size: size of region
 * @use: free region flag
 * @type: memory region attributes
 * @list: memblock list node
 * @nid: NUMA node id
 */
struct memblock_region {
    const char *name;
    phys_addr_t addr;
    phys_addr_t size;
    uint8_t use:1;
    uint8_t type:4;
    struct list_head list;
#ifdef CONFIG_NUMA
    int nid;
#endif
};

#ifdef CONFIG_MEMTEST
extern void memtest(void *block, size_t size);
#else
extern void memtest(void *block, size_t size) {}
#endif

extern void memblock_dump(void);
extern state memblock_add_numa(const char *name, phys_addr_t addr, phys_addr_t size, int numa);
extern state memblock_reserve(const char *name, phys_addr_t addr, phys_addr_t size);
extern state memblock_remove(phys_addr_t addr, size_t size);
extern void memblock_takeover(enum memblock_type type, bool (*fn)(struct memblock_region *));
extern phys_addr_t memblock_phys_alloc_range(size_t size, size_t align, phys_addr_t min_addr, phys_addr_t max_addr, int numa);
extern void __malloc *memblock_alloc_range(size_t size, size_t align, phys_addr_t min_addr, phys_addr_t max_addr, int numa);

static inline state memblock_add(const char *name, phys_addr_t addr, phys_addr_t size)
{
    return memblock_add_numa(name, addr, size, 0);
}

static inline phys_addr_t memblock_phys_alloc_numa(size_t size, size_t align, int numa)
{
    return memblock_phys_alloc_range(size, align,
            MEMBLOCK_ALLOC_ADDR, MEMBLOCK_ALLOC_LIMIT, numa);
}

static inline phys_addr_t memblock_phys_alloc_align(size_t size, size_t align)
{
    return memblock_phys_alloc_range(size, align,
            MEMBLOCK_ALLOC_ADDR, MEMBLOCK_ALLOC_LIMIT, NUMA_NONE);
}

static inline phys_addr_t memblock_phys_alloc(size_t size)
{
    return memblock_phys_alloc_range(size, PAGE_SIZE,
            MEMBLOCK_ALLOC_ADDR, MEMBLOCK_ALLOC_LIMIT, NUMA_NONE);
}

static inline void __malloc *memblock_alloc_numa(size_t size, size_t align, int numa)
{
    return memblock_alloc_range(size, align,
            MEMBLOCK_ALLOC_ADDR, MEMBLOCK_ALLOC_LIMIT, numa);
}

static inline void __malloc *memblock_alloc_align(size_t size, size_t align)
{
    return memblock_alloc_range(size, align,
            MEMBLOCK_ALLOC_ADDR, MEMBLOCK_ALLOC_LIMIT, NUMA_NONE);
}

static inline void __malloc *memblock_alloc(size_t size)
{
    return memblock_alloc_range(size, PAGE_SIZE,
            MEMBLOCK_ALLOC_ADDR, MEMBLOCK_ALLOC_LIMIT, NUMA_NONE);
}

#endif /* _MM_MEMBLOCK_H_ */
