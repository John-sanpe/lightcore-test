#ifndef _KLIST_H_
#define _KLIST_H_

#include <list.h> 
#include <system/kref.h>
#include <system/spinlock.h>

typedef struct klist_node {
    void        *n_klist;   // never access directly
    list_t      n_node;
    kref_t      n_ref;
} klist_node_t;

typedef struct klist {
    spinlock_t  k_lock;
    list_t      k_list;
    void        (*get)(struct klist_node *);
    void        (*put)(struct klist_node *);
} __attribute__ ((aligned (sizeof(size_t)))) klist_t;

#define KLIST_INIT(_name, _get, _put)   \
    {                                   \
                                        \
                                        \
    }


extern void klist_add_head();
extern void klist_add_tail();
extern void klist_add_before();
extern void klist_add_behind();

extern void klist_del();
extern void klist_remove();

struct klist_iter {
	struct klist		*i_klist;
	struct klist_node	*i_cur;
};

#endif