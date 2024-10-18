#include "kernel/param.h"
#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int
main(int argc, char *argv[])
{
  int i;
  char *nargv[MAXARG];

  //如果命令列參數的數量小於 3 或者 argv[1] 的第一個字元不是數字，則進入 if 條件
  if(argc < 3 || (argv[1][0] < '0' || argv[1][0] > '9')){
    fprintf(2, "Usage: %s mask command\n", argv[0]);
    exit(1);
  }

  
  // 補充說明: 當 trace.c 中的 trace() 被調用時，系統會切換到內核態，並進入 syscall.c 中的 syscall() 函數。系統呼叫流程如下：
  // syscall() 會根據進程的 trapframe->a7 寄存器中的值（這個值代表系統呼叫編號），在 syscalls[] 陣列中查找對應的系統呼叫函數。
  // 如果找到對應的系統呼叫（如 sys_trace），則調用該函數並將其返回值存入 p->trapframe->a0。
  // 如果當前進程的 trace_mask 包含了該系統呼叫的編號（通過位運算來檢查），那麼內核會打印追蹤信息，例如系統呼叫名稱和返回值。

  if (trace(atoi(argv[1])) < 0) {//調用 trace 系統呼叫並傳入整數型的掩碼。atoi(argv[1]) 將字串轉換為整數。
    fprintf(2, "%s: trace failed\n", argv[0]);
    exit(1);
  }
  
  //從 argv[2] 開始（第一個命令參數），將參數複製到 nargv 陣列。
  for(i = 2; i < argc && i < MAXARG; i++){
    nargv[i-2] = argv[i];
  }

  //使用 exec 系統呼叫啟動新的程序，nargv[0] 是新程序的名稱，nargv 是其參數列表。
  exec(nargv[0], nargv);
  exit(0);
}
