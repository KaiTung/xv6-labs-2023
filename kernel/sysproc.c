#include "types.h"
#include "riscv.h"
#include "defs.h"
#include "param.h"
#include "memlayout.h"
#include "spinlock.h"
#include "proc.h"
#include "sysinfo.h"

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
  if(n < 0)
    n = 0;
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

//在 xv6 這樣的操作系統中，當用戶程序調用系統調用（如 trace()）時，參數會被放到特定的寄存器中或者保存到內核棧裡。內核會根據系統調用的編號來查找對應的處理函數，並把這些參數取出來。
uint64
sys_trace(void)
{
  int mask;
  
  argint(0, &mask);  //通過調用 argint(0, &mask) 來獲取系統調用的參數。argint() 函數會從用戶態傳遞過來的參數列表中提取第 0 個參數（即第一個參數），並將其值存入 mask 變量中。
  // 假設這裡可以處理 mask 的邊界情況 
  if (mask < 0) {
      return -1;  // 錯誤處理
  }

  // 記錄 mask 到當前進程的 trace_mask 中
  struct proc *p = myproc();
  p->trace_mask = mask;

  return 0;  // 成功
}

uint64
sys_sysinfo(void){
  struct sysinfo info;
  uint64 addr;

  // 從用戶空間系統調用的參數列表中獲取第一個參數（索引 0），並將其解釋為一個指針（地址），然後存入 addr 變量中。
  argaddr(0, &addr);
  if(addr < 0){
    return -1;
  }

  info.freemem = get_freemem();
  info.nproc = get_nproc();

  // 將 sysinfo 結構體複製回用戶空間
  if (copyout(myproc()->pagetable, addr, (char *)&info, sizeof(info)) < 0) {
    return -1;
  }

  return 0;
}
