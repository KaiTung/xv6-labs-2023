#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/param.h"

int main(int argc, char *argv[]){
    // 檢查輸入參數，至少需要一個命令
    // 如果沒有指定命令，則打印錯誤並退出
    if(argc < 2){
        fprintf(2,"need ar least one command.");
        exit(1);
    }
    
    // 準備一個陣列來存放命令和參數
    char *command_buf[MAXARG];
    int i;
    // 先將命令行中的參數複製到這個陣列中
    for (i = 1; i < argc; i++) {
        command_buf[i - 1] = argv[i];
    }
    int base_argc = argc - 1;

    // 使用一個緩衝區來存放從標準輸入讀取的行
    char buf[512];
    int buf_index = 0;
    char c;
    // 每次讀取一個字元，直到遇到換行符或結束符號
    while(read(0, &c, sizeof(c))==sizeof(c)){
        if(c == '\n'){
            // 將讀取的行字串作為新的參數附加到命令
            buf[buf_index] = 0; // 添加字串結束符
            command_buf[base_argc] = buf;
            command_buf[base_argc + 1] = 0; // 最後一個參數必須是 NULL

            // 每讀取到一行，創建一個新的子進程
            if(fork() == 0){
                // 子進程使用 exec 執行命令，將讀取的行作為附加參數
                exec(command_buf[0], command_buf);
                // 如果 exec 失敗，打印錯誤訊息並退出
                fprintf(2, "exec failed\n");
                exit(1);
            }else{
                // 父進程等待子進程完成
                wait(0);
            }
            // 重置緩衝區索引
            buf_index = 0;
        }
        else{
            // 將字元添加到緩衝區中
            buf[buf_index++] = c;
            // 檢查緩衝區是否超出大小限制
            if (buf_index >= sizeof(buf)) {
                fprintf(2, "Input line too long\n");
                exit(1);
            }
        }
    }
    exit(0);
}