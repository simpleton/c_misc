#ifndef __BUDDY_H__
#define __BUDDY_H__

#include <stdlib.h>

struct buddy;

struct buddy* buddy_new( int size );
void buddy_destory( struct buddy* self );

int buddy_alloc( struct buddy* self, int size );
void buddy_free( struct buddy* self, int offset );

int buddy_size( struct buddy* self, int offset );
void buddy_dump( struct buddy* self );

void tree_dump(struct buddy* self);

#endif
