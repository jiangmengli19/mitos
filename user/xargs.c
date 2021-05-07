#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"
#include "kernel/param.h"


int main(int argc, char* argv[]){


    char c;
    //char cfork;
    char s[512];

    int i = 0;
    int j = 0;

    /*
    while(read(0,&c,1)==1||read(0,&c,1)==0){
        printf("we come to read here\n");
        printf("This is what we read %c\n",c);
        if(c=='\n'){
            printf("we come to enter here\n");
            //(buf1[i]+j)=0;
            //argvb[argc-(k+2)]=buf;
            //exec(argvb[0],argvb);
            //printf("\n");
            i++;
            j = 0;
            break;
        }
        if(c=='\0'){
            //(buf1[i]+j)=0;
            //argvb[argc-(k+2)]=buf;
            //exec(argvb[0],argvb);
            printf("we come to the last char\n");
            j = 0;
            break;
        }
        //(buf1[i]+j)=c;
        j++;
    }
 */


  char* argvimp[MAXARG];
  for(int r = 1;r<=argc-1;r++){
      argvimp[r-1]=argv[r];
      //printf("check the copy here %s\n",argvimp[r-1]);
  }


  //int stopend = 1;
  //why is equal is ok but not equal is wrong here?
  while(read(0,&c,1)!=0){
      //s = bufa[i];
      if(c=='\n'||c=='\0'){


          *(s+j)='\0';
          argvimp[argc-1]=s;

          if(fork()==0){

              /*
              close(1);
              dup(p[1]);
              close(p[0]);
              close(p[1]);
              */
              exec(argvimp[0],argvimp);
              //close(1);
              exit(0);
          }
          else{
              /*
              close(p[1]);
              while(read(p[0],&cfork,1)!=0){
                  printf("we come to here in fork parent\n");
                  write(1,&cfork,1);
              }
              close(p[0]);
              */

              wait(0);

          }

          //exec(argvimp[0],argvimp);
          //i++;
          i++;
          j =0;

      }
      else {
          *(s+j)=c;
         //printf("char is %c\n",c);
          //j=0;
          j++;
          //printf("i here is %d\n",i);
          //printf("j here is %d\n",j);
          //continue;
      }
  }


     exit(0);
}
