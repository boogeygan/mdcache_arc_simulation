
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <assert.h>
#include "arc.h"
/*
static void stats(struct __arc *s)
{
	int i = 0;
	struct __arc_list *pos;

	__arc_list_each_prev(pos, &s->mrug.head) {
		struct __arc_object *e = __arc_list_entry(pos, struct __arc_object, head);
		assert(e->state == &s->mrug);
		struct object *obj = __arc_list_entry(e, struct object, entry);
		printf("[%d]", obj->offset);
	}
	printf(" + ");
	__arc_list_each_prev(pos, &s->mru.head) {
		struct __arc_object *e = __arc_list_entry(pos, struct __arc_object, head);
		assert(e->state == &s->mru);
		struct object *obj = __arc_list_entry(e, struct object, entry);
		printf("[%d]", obj->offset);

		if (i++ == s->p)
			printf(" # ");
	}
	printf(" + ");
	__arc_list_each(pos, &s->mfu.head) {
		struct __arc_object *e = __arc_list_entry(pos, struct __arc_object, head);
		assert(e->state == &s->mfu);
		struct object *obj = __arc_list_entry(e, struct object, entry);
		printf("[%d]", obj->offset);

		if (i++ == s->p)
			printf(" # ");
	}
	if (i == s->p)
		printf(" # ");
	printf(" + ");
	__arc_list_each(pos, &s->mfug.head) {
		struct __arc_object *e = __arc_list_entry(pos, struct __arc_object, head);
		assert(e->state == &s->mfug);
		struct object *obj = __arc_list_entry(e, struct object, entry);
		printf("[%d]", obj->offset);
	}
	printf("\n");
	printf("sizes of lists mru:%lu mfu:%lu mrug:%lu mfug:%lu\n",s->mru.size,s->mfu.size,s->mrug.size,s->mfug.size);
}

*/



#define MAXREQ 30000
#define MOD 1000
int main(int argc, char *argv[])
{
	//init phase
	int in_size, cache_size;
	unsigned int *offset_req;
	int *readahead_size;
	srand(time(NULL));

	if ( ! sscanf (argv[1], "%i", &in_size) ){
		printf ("couldn simulate!\n");
		exit(-1);
	}
	if ( ! sscanf (argv[2], "%i", &cache_size) ){
		printf ("couldn simulate, invalid cache size!\n");
		exit(-1);
	}

	struct __arc *s = arc_init(&ops , cache_size);
	offset_req = (unsigned int  *) malloc(sizeof(unsigned int) * in_size);
	readahead_size = (int *) malloc(sizeof(int) * in_size);
	
	for (int i = 0 ; i < in_size ; i++){
		scanf("%u%d", offset_req + i , readahead_size + i);
		//printf("%d %d\n", offset_req[i] , readahead_size [i]);
	}
	printf("cache_size=%d ", cache_size);
	for(int i = 0 ; i < in_size ; i++){
		if (readahead_size[i] - 1 <= 0)
			assert(__arc_lookup(s, &offset_req[i], 1));
		else{
			assert(__arc_lookup(s, &offset_req[i], 1));
			__arc_readahead(s, offset_req[i]+1 , readahead_size[i] );
		}

	}

	//clean up phase
	arc_end();
	free(readahead_size);
	free(offset_req);
	return 0;
}
