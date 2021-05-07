#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int main(int argc, char* argv[]){
  /*
    int p[2];
    pipe(p);
    char buf[10];
    int pid = fork();
    if(pid>0){

        write(p[1],"b",1);
        close(p[1]);
        wait(0);

        while(read(p[0],buf,1)==1){
            printf("%d: received pong\n",getpid());
            break;
        }
        close(p[0]);

    }
    else{
        while(read(p[0],buf,1)==1){
            printf("%d: received ping\n",getpid());
            break;
        }
        close(p[0]);
        write(p[1],"b",1);
        close(p[1]);
    }
     exit(0);
     */

    char buf[2];
    int child_to_parentpipe[2];
    int parent_to_childpipe[2];
    pipe(parent_to_childpipe);
    pipe(child_to_parentpipe);
    if(fork()==0){
        close(parent_to_childpipe[1]);
        close(child_to_parentpipe[0]);
        while(read(parent_to_childpipe[0],buf,1)==1){
            printf("%d: received ping\n",getpid());
            break;
        }
        close(parent_to_childpipe[0]);
        write(child_to_parentpipe[1],"b",1);
        close(child_to_parentpipe[1]);
        exit(0);
    }
    else{
        close(child_to_parentpipe[1]);
        close(parent_to_childpipe[0]);
        write(parent_to_childpipe[1],"b",1);
        close(parent_to_childpipe[1]);
        while(read(child_to_parentpipe[0],buf,1)==1){
            printf("%d: received pong\n",getpid());
            break;
        }
        close(child_to_parentpipe[0]);
        exit(0);
    }








}
