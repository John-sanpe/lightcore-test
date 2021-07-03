/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Physical memory allocation core
 */

#include <mm/page.h>

static inline void free_list_add(struct page_free *free, int order, 
                          struct page *page)
{
    list_add(&free[order].list, &page->list);
    free->free_nr++;
}

static inline void free_list_del(struct page_free *free, int order, 
                          struct page *page)
{
    list_del(&page->list);
    free[order].free_nr--;
}

static inline struct page *free_list_get_page(struct page_free *free)
{
    return list_first_entry_or_null(&free->list, struct page, list);
}

static inline void queue_division(struct page_free *free, struct page *page, 
                                  int need_order, int alloc_order)
{
	size_t size = 1 << alloc_order;

    while(need_order < alloc_order--) {
		size >>= 1;

        /* Put buddy into free list */
        free_list_add(free, alloc_order, &page[size]);
        page[size].order = alloc_order;
    }
}

static inline void queue_merge(struct page_free *free, struct page *page, 
                                  int need_order, int alloc_order)
{


}

/**
 * queue_smallest - Get minimum fit page
 * 
 */
static struct page *queue_smallest(struct region *region, int order)
{
    unsigned int nd; 
    struct page_free *free;
    struct page *page;

    for (nd = order; nd < PAGE_ORDER_MAX; ++nd) {
        free = &region->page_free[nd];

        page = free_list_get_page(free);
        if(!page)
            continue;
        free_list_del(free, nd, page);
        queue_division(free, page, order, nd);
        return page;
    }

    return NULL;
}

struct page *buddy_alloc(int order, gfp_t gfp)
{
    struct page *page;

    page = queue_smallest(&region_map[REGION_NORMAL], order);
    if(page)
        return page;

    return NULL;
}

void buddy_free(struct page *page)
{


}

/**
 * buddy_add - Add pages into region
 */
void buddy_add(struct region *region, int nr, int order, struct page *page)
{
    while(nr--) {
        free_list_add(&region->page_free[0], order, page);
    }
}

