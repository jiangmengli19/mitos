#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int main(int argc, char *argv[]){
    int i;
    if((argc==2) && (strcmp(argv[0],"sleep")==0) ){
       i = atoi(argv[1]);
       sleep(i);
    }
    else{
       fprintf(2,"sleep %s failed \n",argv[1]);
       exit(1);
    }
    exit(0);
}
