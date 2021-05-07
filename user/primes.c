#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

void printprime(int array[],int length);

int main(int argc, char* argv[]){
    int arrayprime[34];
    for(int i=0;i<=33;i++){
        arrayprime[i]=i+2;
    }
    printprime(arrayprime,34);
    exit(0);
}
void printprime(int array[],int length){
    int p[2];
    pipe(p);
    int *renewlength;
    int k = 0;
    renewlength = &k;
    int currentprime = array[0];
    int buf[length];
    printf("prime %d\n",currentprime);
    if(length==1){

        exit(0);
    }
    else{
        if(fork()==0){
            close(0);
            dup(p[0]);
            close(p[0]);
            close(p[1]);
            while(read(0,renewlength,sizeof(int))==sizeof(int)){
                break;
            }

            while(read(0,buf,sizeof(int)*(*renewlength))!=0){
                continue;
            }
            printprime(buf,*renewlength);
            exit(0);
        }
        else{
            close(1);
            dup(p[1]);
            close(p[0]);
            close(p[1]);
            for(int i = 1;i<=length-1;i++){
                if(array[i]%currentprime!=0){
                    *renewlength = (*renewlength)+1;
                    buf[*renewlength-1]=array[i];
                }
            }
            write(1,renewlength,sizeof(int));
            write(1,buf,(*renewlength)*sizeof(int));
            close(1);
            wait(0);
            exit(0);
        }
    }

}


/*
void printprime(int array[],int length){
    int p[2];
    pipe(p);





    int *renewlength;
    int k = 0;
    renewlength = &k;


    int currentprime = array[0];
    //think of how to optimize length;
    int buf[length];
    if(length==1){
        printf("prime %d\n",currentprime);
    }
    else{
        if(fork()==0){



            close(p[1]);

            while(read(p[0],renewlength,sizeof(int))== sizeof(int)){
                printf("come to break here");
                break;
            }
            while(read(p[0],buf,(sizeof(array[0]))*length)!=0){
                printf("come to buf read here");
                continue;
            };


            printf("buf1 %d\n",buf[0]);


            close(p[0]);



            printf("in childprocess, renewlength is %d\n",*renewlength);

            printprime(buf,*renewlength);

        }
        else{



            close(p[0]);



            printf("prime %d\n",array[0]);
            for(int i = 1;i <= length-1;i++){
                if(array[i]%array[0]!=0){
                    *renewlength = (*renewlength)+1;
                    buf[(*renewlength)-1]= array[i];
                }

            }





            printf("come to here");
            printf(" in parent process %d\n",*renewlength);
            write(p[1],renewlength,sizeof(int));
            write(p[1],buf,(sizeof(array[0]))*(*renewlength));



            close(p[1]);
            wait(0);
            exit(0);

        }
    }
    exit(0);
}
 */

/*
#define R 0
#define W 1

// Redirect standard input or standard output to pipe
void redirect(int k, int p[])
{
    close(k);
    dup(p[k]);
    close(p[R]);
    close(p[W]);
}

void filter(int p)
{
    int n;

    while (read(R, &n, sizeof(n)))
    {
        if (n % p != 0)
        {
            write(W, &n, sizeof(n));
        }
    }
    close(W);
    // very tricky, or the main primes would return after the second process exit.
    wait(0);
    exit(0);
}

// The last process, waitting for a number
void waitForNumber()
{
    int pd[2];
    int p;

    if (read(R, &p, sizeof(p)))
    {
        printf("prime %d\n", p);
        pipe(pd);
        if (fork() == 0)    // child
        {
            //redirect(R, pd);
            close(0);
            dup(pd[0]);
            close(pd[0]);
            close(pd[1]);
            waitForNumber();
        }
        else    // parent
        {
            //redirect(W, pd);
            close(1);
            dup(pd[1]);
            close(pd[0]);
            close(pd[1]);
            filter(p);
        }
    }
    exit(0);
}

int main(int argc, char *argv[])
{
    int pd[2];

    pipe(pd);

    if (fork() == 0)    //child
    {
        redirect(R, pd);
        waitForNumber();
    }
    else    // parent
    {
        redirect(W, pd);
        for (int i = 2; i < 36; i++)
        {
            write(W, &i, sizeof(i));
        }
        // close the write side of pipe, cause read returns 0
        close(W);
        // the main primes process should wait until all the other primes process exited
        wait(0);
        exit(0);
    }
    return 0;
}
 */
/*
void printprime(int array[], int length){
    int *publicnum;
    int num = 0;
    publicnum = &num;
    int buf[length];
    int p[2];
    pipe(p);
    if(length == 1){
        printf("primenum is %d\n",array[0]);
        exit(0);
    }
    if(fork()==0){
        close(p[1]);
        close(0);
        dup(p[0]);
        close(p[0]);
        while(read(0,publicnum,sizeof(int))==sizeof(int)){
            break;
        }
        while(read(0,buf,sizeof(int)*length)!=0){
            continue;
        }
        close(0);
        for(int j = 0;j<=(*publicnum)-1;j++){
            printf("the value is %d\n",*(buf+j));
        }
        printprime(buf,*publicnum);

        exit(0);
    }
    else{

        close(p[0]);
        close(1);
        dup(p[1]);
        close(p[1]);
        for(int j = 1;j<=length-1;j++){
            if(array[j]%array[0]!=0){
                buf[*publicnum]=array[j];
                *(publicnum)=*(publicnum)+1;
            }
        }
        write(1,publicnum,sizeof(int));
        write(1,buf,sizeof(int)*(*publicnum));

        close(1);
        wait(0);
        exit(0);
    }
}
 */
