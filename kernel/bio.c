// Buffer cache.
//
// The buffer cache is a linked list of buf structures holding
// cached copies of disk block contents.  Caching disk blocks
// in memory reduces the number of disk reads and also provides
// a synchronization point for disk blocks used by multiple processes.
//
// Interface:
// * To get a buffer for a particular disk block, call bread.
// * After changing buffer data, call bwrite to write it to disk.
// * When done with the buffer, call brelse.
// * Do not use the buffer after calling brelse.
// * Only one process at a time can use a buffer,
//     so do not keep them longer than necessary.


#include "types.h"
#include "param.h"
#include "spinlock.h"
#include "sleeplock.h"
#include "riscv.h"
#include "defs.h"
#include "fs.h"
#include "buf.h"

#define NBUCKET     13
/*
struct {
  struct spinlock lock;
  struct buf buf[NBUF*2];

  // Linked list of all buffers, through prev/next.
  // Sorted by how recently the buffer was used.
  // head.next is most recent, head.prev is least.
  struct buf head;
} bcache;
*/

struct {
    struct spinlock lockBucket[NBUCKET];
    struct buf bufBucket[NBUCKET];
    struct buf buf[NBUF];
    struct spinlock largeLock;
}bcache;


void binit(void){
    //struct buf *b;
    initlock(&bcache.largeLock,"bachea");
    for(int i = 0;i<=NBUCKET-1;i++) {
        initlock(&bcache.lockBucket[i], "bachealock");
        bcache.bufBucket[i].prev = &bcache.bufBucket[i];
        bcache.bufBucket[i].next = &bcache.bufBucket[i];
    }
    for(int i = 0;i<=NBUF-1;i++){
        struct buf *b = &bcache.buf[i];
        int hashnum = i%NBUCKET;
        //printf("%d\n",hashnum);
        b->next = bcache.bufBucket[hashnum].next;
        b->prev = &bcache.bufBucket[hashnum];
        initsleeplock(&b->lock,"bufferlock");
        bcache.bufBucket[hashnum].next->prev = b;
        bcache.bufBucket[hashnum].next = b;



    }


}

/*
void
binit(void)
{
  struct buf *b;

  initlock(&bcache.lock, "bcache");

  // Create linked list of buffers
  bcache.head.prev = &bcache.head;
  bcache.head.next = &bcache.head;
  for(b = bcache.buf; b < bcache.buf+NBUF*2; b++){
    b->next = bcache.head.next;
    b->prev = &bcache.head;
    initsleeplock(&b->lock, "buffer");
    bcache.head.next->prev = b;
    bcache.head.next = b;
  }
  //printf("ticks is %d\n",ticks);
   //bainit();
   //printf("we finish the bainit()\n");
}


// Look through buffer cache for block on device dev.
// If not found, allocate a buffer.
// In either case, return locked buffer.

static struct buf*
bget(uint dev, uint blockno)
{
  struct buf *b;

  acquire(&bcache.lock);

  // Is the block already cached?
  for(b = bcache.head.next; b != &bcache.head; b = b->next){
    if(b->dev == dev && b->blockno == blockno){
      b->refcnt++;
      release(&bcache.lock);
      acquiresleep(&b->lock);
      return b;
    }
  }
  //printf("ticks is %d\n",ticks);
  // Not cached.
  // Recycle the least recently used (LRU) unused buffer.
  for(b = bcache.head.prev; b != &bcache.head; b = b->prev){
    if(b->refcnt == 0) {
      b->dev = dev;
      b->blockno = blockno;
      b->valid = 0;
      b->refcnt = 1;
      release(&bcache.lock);
      acquiresleep(&b->lock);
      return b;
    }
  }
  panic("bget: no buffers");
}
*/

static struct buf*
bget(uint dev, uint blockno){
    int hashnum = blockno%NBUCKET;
    struct buf *b;
    acquire(&bcache.lockBucket[hashnum]);
    //printf("we come to here\n");
    for(b = bcache.bufBucket[hashnum].next;b!=&bcache.bufBucket[hashnum];b = b->next){
        //printf(" i\n");
        if(b->dev == dev && b->blockno == blockno){
            b->refcnt++;
            release(&bcache.lockBucket[hashnum]);
            acquiresleep(&b->lock);
            return b;
        }
    }
    for(b = bcache.bufBucket[hashnum].prev;b!=&bcache.bufBucket[hashnum];b = b->prev){
        //printf("we come to here\n");
        if(b->refcnt == 0){
            b->dev = dev;
            b->blockno = blockno;
            b->valid = 0;
            b->refcnt = 1;
            release(&bcache.lockBucket[hashnum]);
            acquiresleep(&b->lock);
            return b;
        }
    }

    release(&bcache.lockBucket[hashnum]);
    acquire(&bcache.largeLock);
    for(int i = (hashnum+1)%NBUCKET;i!=hashnum;i++){
        acquire(&bcache.lockBucket[i]);
        for(b = bcache.bufBucket[i].prev;b!=&bcache.bufBucket[i];b = b->prev){
            if(b->refcnt == 0){
                b->dev = dev;
                b->blockno = blockno;
                b->valid = 0;
                b->refcnt = 1;
                acquire(&bcache.lockBucket[hashnum]);
                b->next->prev = b->prev;
                b->prev->next = b->next;
                b->next = bcache.bufBucket[hashnum].next;
                b->prev = &bcache.bufBucket[hashnum];
                bcache.bufBucket[hashnum].next->prev = b;
                bcache.bufBucket[hashnum].next = b;
                release(&bcache.lockBucket[hashnum]);
                release(&bcache.lockBucket[i]);
                release(&bcache.largeLock);
                acquiresleep(&b->lock);
                return b;
                //release(&bcache.lockBucket[hashnum]);

            }
        }
        release(&bcache.lockBucket[i]);
    }
    release(&bcache.largeLock);

    panic("bget:no buffer");



};

// Return a locked buf with the contents of the indicated block.
struct buf*
bread(uint dev, uint blockno)
{
  struct buf *b;

  b = bget(dev, blockno);
  if(!b->valid) {
    virtio_disk_rw(b, 0);
    b->valid = 1;
  }
  return b;
}

// Write b's contents to disk.  Must be locked.
void
bwrite(struct buf *b)
{
  if(!holdingsleep(&b->lock))
    panic("bwrite");
  virtio_disk_rw(b, 1);
}

// Release a locked buffer.
// Move to the head of the most-recently-used list.
/*
void
brelse(struct buf *b)
{
  if(!holdingsleep(&b->lock))
    panic("brelse");

  releasesleep(&b->lock);

  acquire(&bcache.lock);
  b->refcnt--;
  if (b->refcnt == 0) {
    // no one is waiting for it.
    b->next->prev = b->prev;
    b->prev->next = b->next;
    b->next = bcache.head.next;
    b->prev = &bcache.head;
    bcache.head.next->prev = b;
    bcache.head.next = b;
  }
  
  release(&bcache.lock);
}
*/

void brelse(struct buf *b)
{
    if(!holdingsleep(&b->lock))
    {
        panic("brelsea");
    }
    releasesleep(&b->lock);
    int hashnum = (b->blockno)%NBUCKET;
    acquire(&bcache.lockBucket[hashnum]);
    b->refcnt--;
    if(b->refcnt == 0){
        b->next->prev = b->prev;
        b->prev->next = b->next;
        b->next = bcache.bufBucket[hashnum].next;
        b->prev = &bcache.bufBucket[hashnum];
        bcache.bufBucket[hashnum].next->prev = b;
        bcache.bufBucket[hashnum].next = b;
    }
    release(&bcache.lockBucket[hashnum]);

}

/*
void
bpin(struct buf *b) {
  //int hashnum = (b->blockno)%NBUCKET;
  acquire(&bcache.lock);
  b->refcnt++;
  release(&bcache.lock);
}


void
bunpin(struct buf *b) {
  acquire(&bcache.lock);
  b->refcnt--;
  release(&bcache.lock);
}
*/


void
bpin(struct buf *b){
    int hashnum = (b->blockno)%NBUCKET;
    acquire(&bcache.lockBucket[hashnum]);
    b->refcnt++;
    release(&bcache.lockBucket[hashnum]);
}

void
bunpin(struct buf *b){
    int hashnum = (b->blockno)%NBUCKET;
    acquire(&bcache.lockBucket[hashnum]);
    b->refcnt--;
    release(&bcache.lockBucket[hashnum]);
}
