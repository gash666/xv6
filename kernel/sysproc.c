#include "types.h"
#include "riscv.h"
#include "param.h"
#include "defs.h"
#include "memlayout.h"
#include "spinlock.h"
#include "proc.h"

uint64
sys_exit(void)
{
  int n;
  argint(0, &n);
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
  argaddr(0, &p);
  return wait(p);
}

uint64
sys_sbrk(void)
{
  uint64 addr;
  int n;

  argint(0, &n);
  addr = myproc()->sz;
  if(growproc(n) < 0)
    return -1;
  return addr;
}

uint64
sys_sleep(void)
{
  int n;
  uint ticks0;


  argint(0, &n);
  acquire(&tickslock);
  ticks0 = ticks;
  while(ticks - ticks0 < n){
    if(killed(myproc())){
      release(&tickslock);
      return -1;
    }
    sleep(&ticks, &tickslock);
  }
  release(&tickslock);
  return 0;
}


#ifdef LAB_PGTBL
int check_PTE_A(pagetable_t pagetable, uint64 va)
{
  for (int level = 2; level > 0; level--)
  {
    pte_t* pte = &pagetable[PX(level, va)];
    if (*pte & PTE_V)
      pagetable = (pagetable_t)PTE2PA(*pte);
    else
      return 0;
  }

  if (pagetable[PX(0, va)] & PTE_A)
  {
    pagetable[PX(0, va)] -= PTE_A;
    return 1;
  }
  return 0;
}

int
sys_pgaccess(void)
{
  uint64 addr;
  int number_pages;
  uint64 buffer;

  argaddr(0, &addr);
  argint(1, &number_pages);
  argaddr(2, &buffer);

  if (number_pages > PGSIZE)
  {
    return -1;
  }

  uint64 answer_size = (number_pages + 7) / 8;

  struct proc* p = myproc();
  char* temp_answer = (char*)kalloc();
  for (int i = 0; i < number_pages / 8; i++)
    temp_answer[i] = 0;
  
  for (int i = 0; i < number_pages; i++)
    if (check_PTE_A(p->pagetable, addr + i * PGSIZE))
      temp_answer[i / 8] |= (1 << (i % 8));

  copyout(p->pagetable, buffer, temp_answer, answer_size);
  kfree(temp_answer);
  return 0;
}
#endif

uint64
sys_kill(void)
{
  int pid;

  argint(0, &pid);
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
