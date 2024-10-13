#include "kernel/types.h"
#include "user/user.h"

int main(int argc, char *argv[]){
    int sleep_sec;
    if (argc < 2){  // 检查参数个数，如果小于2，说明没有指定休眠的秒数
        fprintf(2, "Usage: sleep seconds\n");
        exit(0);
    }

    sleep_sec = atoi(argv[1]);
    if (sleep_sec > 0){
		sleep(sleep_sec);
	} else {
		fprintf(2, "Invalid interval %s\n", argv[1]);
	}

    exit(0);
}