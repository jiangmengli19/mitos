#include "types.h"
#include "riscv.h"
#include "defs.h"
#include "date.h"
#include "param.h"
#include "memlayout.h"
#include "spinlock.h"
#include "proc.h"

uint64
sys_exit(void)
{
  int n;
  if(argint(0, &n) < 0)
    return -1;
  exit(n);
  return 0;  // not reached
}

uint64
sys_getpid(void)
{
  return myproc()->pid;
}

uint64
sys_fork(void)
{
  return fork();
}

uint64
sys_wait(void)
{
  uint64 p;
  if(argaddr(0, &p) < 0)
    return -1;
  return wait(p);
}

uint64
sys_sbrk(void)
{
  int addr;
  int n;

  if(argint(0, &n) < 0)
    return -1;
  addr = myproc()->sz;
  if(growproc(n) < 0)
    return -1;
  return addr;
}

uint64
sys_sleep(void)
{
   /*
    uint64 fp = r_fp();
    uint64 pgup = PGROUNDUP(fp);
    printf("%p\n",pgup);
    backtrace(0,fp,pgup);
   */
   backtrace(0,0,0);
  int n;
  uint ticks0;

  if(argint(0, &n) < 0)
    return -1;
  acquire(&tickslock);
  ticks0 = ticks;
  while(ticks - ticks0 < n){
    if(myproc()->killed){
      release(&tickslock);
      return -1;
    }
    sleep(&ticks, &tickslock);
  }
  release(&tickslock);

  return 0;
}

uint64
sys_kill(void)
{
  int pid;

  if(argint(0, &pid) < 0)
    return -1;
  return kill(pid);
}

// return how many clock tick interrupts have occurred
// since start.
uint64
sys_uptime(void)
{
  uint xticks;

  acquire(&tickslock);
  xticks = ticks;
  release(&tickslock);
  return xticks;
}

uint64
sys_sigreturn(void)
{

    struct proc *p = myproc();

    p->trapframe->a0 = p->a0store;
    p->trapframe->a1 = p->a1store;
    p->trapframe->a2 = p->a2store;
    p->trapframe->a3 = p->a3store;
    p->trapframe->a4 = p->a4store;
    p->trapframe->a5 = p->a5store;
    p->trapframe->a6 = p->a6store;
    p->trapframe->a7 = p->a7store;
    p->trapframe->t0 = p->t0store;
    p->trapframe->t1 = p->t1store;
    p->trapframe->t2 = p->t2store;
    p->trapframe->t3 = p->t3store;
    p->trapframe->t4 = p->t4store;
    p->trapframe->t5 = p->t5store;
    p->trapframe->t6 = p->t6store;
    p->trapframe->ra = p->rastore;
    p->trapframe->epc = p->epcstore;






    p->trapframe->ra = p->rastore;
    p->trapframe->sp = p->spstore;
    p->trapframe->gp = p->gpstore;
    p->trapframe->tp = p->tpstore;
    p->trapframe->s0 = p->s0store;
    p->trapframe->s1 = p->s1store;
    p->trapframe->s2 = p->s2store;
    p->trapframe->s3 = p->s3store;
    p->trapframe->s4 = p->s4store;
    p->trapframe->s5 = p->s5store;
    p->trapframe->s6 = p->s6store;
    p->trapframe->s7 = p->s7store;
    p->trapframe->s8 = p->s8store;
    p->trapframe->s9 = p->s9store;
    p->trapframe->s10 = p->s10store;
    p->trapframe->s11= p->s11store;
    p->sigstate = 0;

    //printf("the returned s0 is %p\n",p->trapframe->s0);
    //printf("the returned ra is %p\n",p->trapframe->ra);
    //printf("the returned epc is %p\n",p->trapframe->epc);
    //p->trapframe = (struct trapframe *)p->startpos;
    p->sigstate = 0;
    return 0;
}

uint64
sys_sigalarm(void)
{
    int interval;
    if(argint(0,&interval)<0){
        return -1;
    }
    //printf("interval is %d\n",interval);
    //proc *p = myproc();
    uint64 p;
    if(argaddr(1,&p)<0){
        return -1;
    }
    //printf("the address of function is %p\n",p);
    myproc()->handler = p;
    myproc()->numhandler = interval;
    return 0;


}



