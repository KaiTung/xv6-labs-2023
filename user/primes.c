#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

#define READ 0
#define WRITE 1

void filter(int *left_pipe) {
    close(left_pipe[WRITE]);
    int prime;
    read(left_pipe[READ], &prime ,sizeof(prime));
    printf("prime %d\n", prime);

    int i;
    if(read(left_pipe[READ], &i,sizeof(i))!=0){
        int right_pipe[2];
        pipe(right_pipe);

        // 创建新的进程来处理筛选
        if (fork() == 0) {
            close(right_pipe[WRITE]); // 子进程不需要写入
            filter(right_pipe); // 递归处理
        } else {
            close(right_pipe[READ]); // 父进程不需要读取
            int num;

            if(i%prime!=0)//注意已经读取了一个i，这里要判断一下
            {
                write(right_pipe[WRITE],&i,sizeof(int));
            }

            // 从左边的管道读取数字，并筛选掉能被当前素数整除的数字
            while (read(left_pipe[READ], &num, sizeof(num)) > 0) {
                if (num % prime != 0) {
                    write(right_pipe[WRITE], &num, sizeof(num));
                }
            }

            // 关闭管道和等待子进程结束
            close(left_pipe[READ]);
            close(right_pipe[WRITE]);
            wait(0);
        }
    }
}

int main(){
    int p[2];
    pipe(p);

    // 創建一個子進程來進行篩選
    if (fork() == 0) {
        close(p[WRITE]); // 子進程不需要寫入
        filter(p);
    } else {
        close(p[READ]); // 父進程不需要讀取
        // 將 2 到 35 的數字寫入管道
        for (int i = 2; i <= 35; i++) {
            write(p[WRITE], &i, sizeof(i));
        }
        close(p[WRITE]); // 關閉管道寫入端
        wait(0); // 等待子進程結束
    }

    exit(0);
}

