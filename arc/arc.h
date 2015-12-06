
#ifndef __ARC_H__
#define __ARC_H__

#include <memory.h>
#include <stddef.h>

/**********************************************************************
 * Simple double-linked list, inspired by the implementation used in the
 * linux kernel.
 */
struct __arc_list {
    struct __arc_list *prev, *next;
};

#define __arc_list_entry(ptr, type, field) \
    ((type*) (((char*)ptr) - offsetof(type,field)))

#define __arc_list_each(pos, head) \
    for (pos = (head)->next; pos != (head); pos = pos->next)

#define __arc_list_each_prev(pos, head) \
    for (pos = (head)->prev; pos != (head); pos = pos->prev)

static inline void
__arc_list_init( struct __arc_list * head )
{
    head->next = head->prev = head;
}

static inline void
__arc_list_insert(struct __arc_list *list, struct __arc_list *prev, struct __arc_list *next)
{
    next->prev = list;
    list->next = next;
    list->prev = prev;
    prev->next = list;
}

static inline void
__arc_list_splice(struct __arc_list *prev, struct __arc_list *next)
{
    next->prev = prev;
    prev->next = next;
}


static inline void
__arc_list_remove(struct __arc_list *head)
{
    __arc_list_splice(head->prev, head->next);
    head->next = head->prev = NULL;
}

static inline void
__arc_list_prepend(struct __arc_list *head, struct __arc_list *list)
{
    __arc_list_insert(head, list, list->next);
}


/**********************************************************************
 * All objects are inserted into a hashtable to make the lookup fast.
 */
struct __arc_hash {
    unsigned long size;
    struct __arc_list *bucket;
};


/**********************************************************************
 * The arc state represents one of the m{r,f}u{g,} lists
 */
struct __arc_state {
    unsigned long size;
    struct __arc_list head;
};

/* This structure represents an object that is stored in the cache. Consider
 * this structure private, don't access the fields directly. When creating
 * a new object, use the __arc_object_init() function to initialize it. */
struct __arc_object {
    struct __arc_state *state;
    struct __arc_list head, hash;
    unsigned long size;
};

struct __arc_ops {
    /* Return a hash value for the given key. */
    unsigned int (*hash) (unsigned int  *key);

    /* Compare the object with the key. */
    int (*cmp) (struct __arc_object *obj, unsigned int  *key);

    /* Create a new object. The size of the new object must be know at
     * this time. Use the __arc_object_init() function to initialize
     * the __arc_object structure. */
    struct __arc_object *(*create) (unsigned int *key);
    
    /* Fetch the data associated with the object. */
    int (*fetch) (struct __arc_object *obj);
    
    /* This function is called when the cache is full and we need to evict
     * objects from the cache. Free all data associated with the object. */
    void (*evict) (struct __arc_object *obj);
    
    /* This function is called when the object is completely removed from
     * the cache directory. Free the object data and the object itself. */
    void (*destroy) (struct __arc_object *obj);
};






/* The actual cache. */
struct __arc {
    struct __arc_ops *ops;
    struct __arc_hash hash;
    
    unsigned long c, p;
    struct __arc_state mrug, mru, mfu, mfug;
    int total, hits;
};

/* Functions to create and destroy the cache. */
struct __arc * __arc_create(struct __arc_ops *ops, unsigned long c);

struct __arc * arc_init(struct __arc_ops *ops, unsigned long c);
void arc_end();

/* Initialize a new object. To be used from the alloc() op function. */
void __arc_object_init(struct __arc_object *obj, unsigned long size);

/* Lookup an object in the cache. The cache automatically allocates and
 * fetches the object if it does not exists yet. */
/* struct __arc_object *__arc_lookup(struct __arc *cache, const void *key); */

struct __arc_object *__arc_lookup(struct __arc *cache, unsigned int *key, int stat_update ); /*stat_update is passed as 1 when doing new element lookup and 0 when used in read_ahead*/

/* To find the number of hits so far and total number of requests entertained 
 * by the cache. Hit and miss ratios can be calculated using followings.
 */

static inline int __arc_hit_count(struct __arc *cache){
 return cache->hits;
}

static inline int __arc_total_request_count(struct __arc *cache){
	 return cache->total;
}

/* enables to readahead ra_count number of blocks from from_offset.
*/
void __arc_readahead(struct __arc *cache, unsigned int offset, int ra_count);




/*merging test.c*/
struct object {
	unsigned int offset;  /*io-req of file offset / offset block start address will be the key...eventually*/
	struct __arc_object entry;
};


/*static unsigned int objname(struct __arc_object *entry)
{
	struct object *obj = __arc_list_entry(entry, struct object, entry);
	//return obj->sha1[0];
	return obj->offset;
}
*/


static unsigned int __op_hash(unsigned int *key)
{
	unsigned int *sha1 = key;
	return *sha1;
}

static int __op_compare(struct __arc_object *e, unsigned int *key)
{
	struct object *obj = __arc_list_entry(e, struct object, entry);
	return obj->offset == *key ? 1 : 0 ;

}


static struct __arc_object *__op_create(unsigned int *key)
{
	struct object *obj = malloc(sizeof(struct object));
	obj->offset = *key;
       	__arc_object_init(&obj->entry, 1);
	return &obj->entry;
}

static int __op_fetch(struct __arc_object *e)
{
	return 0;
}

static void __op_evict(struct __arc_object *e)
{
	//struct object *obj = __arc_list_entry(e, struct object, entry);
	//free(obj->data);
}

static void __op_destroy(struct __arc_object *e)
{
	struct object *obj = __arc_list_entry(e, struct object, entry);
	free(obj);
}


static struct __arc_ops ops = {
	.hash           = __op_hash,
	.cmp            = __op_compare,
	.create         = __op_create,
	.fetch          = __op_fetch,
	.evict          = __op_evict,
	.destroy        = __op_destroy
};

/*merging end test.c*/

static struct __arc *cache;


#endif /* __ARC_H__ */
