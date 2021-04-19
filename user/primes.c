#include "kernel/types.h"
#include "user/user.h"
#include "kernel/fcntl.h"
void prime(int l);

int main(void) {
    int l[2];
    int r[2];
    pipe(r);
    char* ch;
    for(int i=2; i <= 35; i++) {
        char c = (char)i;
        ch = &c;
        write(r[1], ch, 1);
    }
    l[0] = dup(r[0]);
    close(r[0]);
    close(r[1]);
    
    prime(l[0]);
    
    exit(0);
}

void prime(int l) {
    char a;
    char b;
    char* val = &a;
    int pid = 0;
    int r[2];
    if(pid == 0) {
        
        if(read(l, val, 1) == 0) {
            printf("%d aaa\n", val[0]);
            return;
        } 
        pid = fork();
        printf("%d aaa\n", l);
        if(pid == 0) {
            printf("%d\n", val[0]);
            pipe(r);
            char* t = &b;
            while(read(l, t, 1) != 0) {
                int div = (int) val[0];
                int bdiv = (int) t[0];

                if(bdiv % div != 0) {
                    write(r[1], t, 1);
                }
                
            }
            close(l);
            l = dup(r[0]);
            
            close(r[0]);
            close(r[1]);
            prime(l);
        }else if(pid > 0) {
            close(l);
            wait(0);
        }else {
            printf("fork error.\n");
        }
    }
}
