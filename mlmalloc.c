#include "mlmalloc.h"
#include <stdio.h>
#include <sys/mman.h>

static void *old_malloc_hook;
static void *old_free_hook;

static void *ml_malloc_hook (size_t, const void *);
static void ml_free_hook (void*, const void *);

#define hooks_save() {old_malloc_hook = __malloc_hook; old_free_hook = __free_hook;}
#define hooks_usemalloc() {__malloc_hook = old_malloc_hook; __free_hook = old_free_hook;}
#define hooks_useml() {__malloc_hook = ml_malloc_hook; __free_hook = ml_free_hook;}

struct mlblock{
  void *addr;
  size_t len;
  struct mlblock *next;
};

struct mldb{
  /* lock */
  struct mlblock *alloc;
} db;

static void
ml_init_hook (void)
{
  hooks_save();
  hooks_useml();
  db.alloc = NULL;
}

static void * get_mlblock()
{
  void *ptr;
  hooks_usemalloc();
  ptr = malloc(sizeof(struct mlblock));
  hooks_useml();
  return ptr;
}

static void put_mlblock(void *ptr)
{
  hooks_usemalloc();
  free(ptr);
  hooks_useml();
}

static void ml_block_append(struct mlblock *block)
{
  struct mlblock *cur = db.alloc;
  if (!db.alloc)
    db.alloc = block;
  else{
    while(cur && cur->next){
      cur = cur->next;
    }
    cur->next = block;
  }
}

static void * ml_block_remove(void *ptr)
{
  struct mlblock *cur = db.alloc;
  struct mlblock *tmp;
  if (db.alloc && db.alloc->addr == ptr){
    db.alloc = db.alloc->next;
    return db.alloc;
  }
  else
  {
    while(cur && cur->next && cur->next->addr!=ptr)
      cur = cur->next;
    if(!cur || !cur->next)
      return NULL;
    tmp = cur->next;
    cur->next = tmp->next;
    return tmp;
  }
  return NULL;
}

static void *
ml_malloc_hook (size_t size, const void *caller)
{
  void *result;
  void *addr;
  struct mlblock *block;
  addr = mmap(NULL, size, PROT_READ | PROT_WRITE, 
    MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
  if (addr == MAP_FAILED)
    return NULL;
  block = get_mlblock();
  block->addr = addr;
  block->len = size;
  block->next = NULL;
  // hooks_usemalloc();
  // printf("ml_malloc %x, %d\n", block->addr, size);
  // hooks_useml();
  return block->addr;
}

static void
ml_free_hook (void *ptr, const void *caller)
{
  struct mlblock *block;
  block = ml_block_remove(ptr);
  if(block){
    munmap(block->addr, block->len);
    put_mlblock(block);
  }
    
  // hooks_usemalloc();
  // printf("ml_free %x\n", ptr);
  // hooks_useml();
}
