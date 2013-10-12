#include <stdlib.h>
#include <assert.h>
#include <stdio.h>
#include <string.h>

struct buddy {
  unsigned size;
  unsigned longest[1];
};

#define _LEFT_CHILD(index) ((index) * 2 + 1)
#define _RIGHT_CHILD(index) ((index) * 2 + 2)
#define _PARENT(index) ((index + 1) / 2 - 1)

#define _IS_POWER_OF_2(x) ( !((x) & ((x) - 1)) )
#define _MAX(a, b) ((a) > (b) ? (a) : (b))


unsigned fixsize(unsigned size)
{
  unsigned i = 1;
  while (size > i) {
    i *= 2;
  }
  printf("fixsize: original size:%d, fixed size:%d\n", size, i);
  return i;
}

struct buddy*
buddy_new(int size) 
{
  struct buddy* self;
  unsigned node_size;
  int i;
  
  if (size < 1 || !_IS_POWER_OF_2(size)) {
    return NULL;
  }
  
  self = (struct buddy*) malloc (2 * size * sizeof(unsigned));
  self->size = size;
  node_size = size * 2;
  
  for (i = 0; i < 2 * size - 1; ++i ) {
    if (_IS_POWER_OF_2(i+1))
      node_size /= 2;
    self->longest[i] = node_size;
  }
  
  return self;
}

void
buddy_destory(struct buddy* self) 
{
  free(self);
}

int 
buddy_alloc(struct buddy* self, unsigned size)
{
  unsigned index = 0;
  unsigned node_size;
  unsigned offset = 0;
  
  if (self == NULL) {
    return -1;
  }
  
  if (size <= 0) {
    size = 1;
  } else {
    size = fixsize(size);
  }
  
  if (self->longest[index] < size) {
    return -1;
  }
  
  for (node_size = self->size; node_size != size; node_size /= 2) {
    if (self->longest[_LEFT_CHILD(index)] >= size) {
      index = _LEFT_CHILD(index);
    } else {
      index = _RIGHT_CHILD(index);
    }
  }
  //set current node's remand value to zero
  self->longest[index] = 0;
  offset = index * node_size - (self->size - node_size);
  
  while (index) {
    index = _PARENT(index);
    self->longest[index] = _MAX(self->longest[_LEFT_CHILD(index)], 
                                self->longest[_RIGHT_CHILD(index)]);
  }

  return offset;
}

void 
buddy_dump(struct buddy* self) {
  char canvas[65];
  int i,j;
  unsigned node_size, offset;

  if (self == NULL) {
    printf("buddy_dump: struct buddy* self == NULL");
    return;
  }

  //FIXME:
  if (self->size > 64) {
    printf("buddy_dump: (struct buddy*) self is too large to dump");
    return;
  }

  memset(canvas, '_', sizeof(canvas));
  node_size = self->size * 2;
  for (i = 0; i < 2 * self->size - 1; ++i) {
    if ( _IS_POWER_OF_2(i + 1)) {
      node_size /= 2;
    }

    if (self->longest[i] == 0) {
      if (i >= self->size - 1) {
        canvas[i - self->size + 1] = '*';
      } else if (self->longest[_LEFT_CHILD(i)] && self->longest[_RIGHT_CHILD(i)]) {
        offset = (i + 1) * node_size - self->size;

        for (j = offset; j < offset + node_size; ++j) {
          canvas[j] = '*';
        }
      }
    }
  }
  canvas[self->size] = '\0';
  puts(canvas);
}

void 
tree_dump( struct buddy* self) {
  int i;
  for (i = 0; i < self->size * 2 -1; ++i) {
    printf("%d ", self->longest[i]);
  }

  printf("\n");
}
void
buddy_free( struct buddy* self, int offset) {
  unsigned node_size;
  unsigned index = 0;
  unsigned left_longest , right_longest;

  assert(self && offset >= 0 && offset < self->size);

  node_size = 1;
  index = offset + self->size - 1;
  //find the allocated block, longest == 0 means the memory
  for (; self->longest[index]; index = _PARENT(index)) {
    node_size *= 2;
    if (index == 0) {
      printf("free error\n");
      return;
    }
  }

  self->longest[index] = node_size;

  while (index) {
    index = _PARENT(index);
    node_size *= 2;

    left_longest = self->longest[_LEFT_CHILD(index)];
    right_longest = self->longest[_RIGHT_CHILD(index)];
    // need merge
    if (left_longest + right_longest == node_size) {
      self->longest[index] = node_size;
    } else {
      self->longest[index] = _MAX(left_longest, right_longest);
    }
  }
}

int 
buddy_size(struct buddy* self, int offset) {
  unsigned node_size;
  unsigned index = 0;
  assert(self && offset >= 0 && offset < self->size);

  node_size = 1;
  for (index = offset + self->size - 1; self->longest[index]; index = _PARENT(index)) {
    node_size *= 2;
  }
  return node_size;
}

