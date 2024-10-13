#include "kernel/types.h"
#include "user/user.h"

int main(){
    int p2c[2]; // Pipe for parent to child
    int c2p[2]; // Pipe for child to parent
    char message = 'A';

    if(pipe(p2c) < 0){
        fprintf(2,"pipe failed.\n");
    }
    if(pipe(c2p) < 0){
        fprintf(2,"pipe failed.\n");
    }


    int pid = fork();
    if(pid < 0){
        fprintf(2,"fork failed.\n");
    }

    if(pid == 0){
        close(p2c[1]);
        close(c2p[0]);

        read(p2c[0],&message,1);
        fprintf(1,"%d: received ping\n",getpid());
        write(c2p[1],&message,1);
    }else{
        close(p2c[0]);
        close(c2p[1]);
        
        write(p2c[1],&message,1);
        wait(0);
        read(c2p[0],&message,1);
        fprintf(1,"%d: received pong\n",getpid());
    }
    exit(0);
}