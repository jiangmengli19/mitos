// Physical memory allocator, for user processes,
// kernel stacks, page-table pages,
// and pipe buffers. Allocates whole 4096-byte pages.

#include "types.h"
#include "param.h"
#include "memlayout.h"
#include "spinlock.h"
#include "riscv.h"
#include "defs.h"

void freerange(void *pa_start, void *pa_end);

extern char end[]; // first address after kernel.
                   // defined by kernel.ld.
//struct spinlock spinlockarray[NCPU];
struct{
    struct spinlock spinlockarray[NCPU];
    struct run *freelista[NCPU];
}kmemb;
struct run {
  struct run *next;
};
/*
struct {
  struct spinlock lock;
  struct run *freelist;
} kmem;
*/
void lockinit(){

    for(int i = 0;i<=NCPU-1;i++){
        initlock(kmemb.spinlockarray+i,"kmemlock")
        ;
    }

    //initlock(&spinlockarray,"kmemlock");
}

void
kinit()
{
  lockinit();
  //initlock(&kmem.lock, "kmem");
  freerange(end, (void*)PHYSTOP);
}

void
freerange(void *pa_start, void *pa_end)
{
  char *p;
  p = (char*)PGROUNDUP((uint64)pa_start);
  for(; p + PGSIZE <= (char*)pa_end; p += PGSIZE)
    kfree(p);
}

// Free the page of physical memory pointed at by v,
// which normally should have been returned by a
// call to kalloc().  (The exception is when
// initializing the allocator; see kinit above.)
void
kfree(void *pa)
{
  struct run *r;

  if(((uint64)pa % PGSIZE) != 0 || (char*)pa < end || (uint64)pa >= PHYSTOP)
    panic("kfree");

  // Fill with junk to catch dangling refs.
  memset(pa, 1, PGSIZE);

  r = (struct run*)pa;
  int corenum = (((char *)pa - (char *) PGROUNDUP((uint64)end))/PGSIZE)%NCPU;
  //printf("corenum is %d\n",corenum );
  acquire(kmemb.spinlockarray+corenum);
  r->next = kmemb.freelista[corenum];
  //r->next = (struct run *)kmemb.freelista+corenum;
  kmemb.freelista[corenum] = r;
  //*(kmemb.freelista+corenum) = r;
  release(kmemb.spinlockarray+corenum);
  /*
  acquire(&kmem.lock);
  r->next = kmem.freelist;
  kmem.freelist = r;
  release(&kmem.lock);
   */
}

// Allocate one 4096-byte page of physical memory.
// Returns a pointer that the kernel can use.
// Returns 0 if the memory cannot be allocated.
void *
kalloc(void)
{
  struct run *r;
  int releaseflag = 1;
  int subreleaseflag = 0;
  push_off();
  int id = cpuid();
  pop_off();
  acquire(kmemb.spinlockarray+id);
  r = kmemb.freelista[id];
  if(r){
      kmemb.freelista[id] = r->next;
  }
  else{
      release(kmemb.spinlockarray+id);
      releaseflag = 0;
      for(int i = 0;i<=NCPU-1;i++){
          acquire(kmemb.spinlockarray+i);
          r = kmemb.freelista[i];
          if(r){
              kmemb.freelista[i] = r->next;
              subreleaseflag = 1;
          }
          release(kmemb.spinlockarray+i);
          if(subreleaseflag){
              break;
          }
      }
  }

      //kmemb.freelista[id] = r->next;
  if(releaseflag){
      release(kmemb.spinlockarray+id);
  }

  /*
  acquire(&kmem.lock);
  r = kmem.freelist;
  if(r)
    kmem.freelist = r->next;
  release(&kmem.lock);
  */

  if(r)
    memset((char*)r, 5, PGSIZE); // fill with junk
  return (void*)r;
}
