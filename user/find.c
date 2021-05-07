#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"

#define STDDER_FILENO 2
#define O_RDONLY 0
char* fmtname(char * path)
{
    //static char buf[DIRSIZ+1];
    char *p;

    for (p = path + strlen(path); p >= path && *p != '/'; p--);
    p ++;

    return p;
}


void
find(char *path, char* file)
{
    char buf[512], *p;
    int fd;
    struct dirent de;
    struct stat st;

    if((fd = open(path, 0)) < 0){
        fprintf(2, "find: cannot open %s\n", path);
        return;
    }

    if(fstat(fd, &st) < 0){
        fprintf(2, "find: cannot stat %s\n", path);
        close(fd);
        return;
    }

    switch(st.type){
        case T_FILE:
            if(strcmp(fmtname(path),file)==0){
                //printf("we come to the compare point here\n");
                printf("%s\n",path);
            }
            break;

        case T_DIR:
            if(strlen(path) + 1 + DIRSIZ + 1 > sizeof buf){
                printf("find: path too long\n");
                break;
            }
            strcpy(buf, path);
            p = buf+strlen(buf);
            *p++ = '/';
            while(read(fd, &de, sizeof(de)) == sizeof(de)){
                if(de.inum == 0)
                    continue;
                memmove(p, de.name, DIRSIZ);
                p[DIRSIZ] = 0;
                if(!strcmp(de.name,".")||!strcmp(de.name,"..")){
                    continue;
                }
                /*
                if(stat(buf, &st) < 0){
                    printf("find: cannot stat %s\n", buf);
                    continue;
                }
                 */
               find(buf,file);
            }
            break;
    }
    close(fd);
}
int main(int argc, char* argv[]){
    if(argc!=3){
        fprintf(2,"the format of find need three input find .. ..");
        exit(1);
    }

    find(argv[1],argv[2]);

    exit(0);
}

