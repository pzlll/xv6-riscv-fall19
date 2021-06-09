#include "kernel/types.h"
#include "user/user.h"
#include "kernel/fcntl.h"
#include "kernel/param.h"

char whitespace[] = " \t\n\r";
char* argv[2][MAXARG];
char* eargv[2][MAXARG];
int argc[2] = {0, 0};



int ispipe = 0;

int count[2] = {0, 0};
char* file[2][10];
char* efile[2][10];
int mode[2][10];
int fd[2][10];
int redir[2] = {0,0};
// int leftredir = 0;
// int rightredir = 0;
// char* file1[2];
// char* efile1[2];
// char* leftfile;
// char* leftefile;
// char* rightfile;
// char* rightefile;
// int leftmode;
// int rightmode;
// int leftfd;
// int rightfd;

int getcmd(char *buf, int nbuf)
{
    buf = gets(buf, nbuf);
    if (buf[0] == 0)
    {
        return -1;
    }
    else
    {
        return 0;
    }
}
void execcmd(int direct) {
    if(strlen(argv[direct][0]) == 0) {
        fprintf(2, "exec argument error\n");
        exit(-1);
    }
    exec(argv[direct][0], argv[direct]);
    fprintf(2, "exec fail.\n");
    exit(-1);

    
}

void redircmd(int direct) {
    int i = 0;
    for(; i < count[direct]; i++) {
        close(fd[direct][i]);
        if(open(file[direct][i], mode[direct][i]) < 0) {
            fprintf(2, "open file failed.\n");
            exit(-1);
        }
    }
    
}

void rumcmd() {
    if(ispipe) {
        int p[2];
        pipe(p);
        int pid;
        if((pid = fork()) == 0) {
            close(1);
            dup(p[1]);
            close(p[0]);
            close(p[1]);
            if(redir[0]) {
                redircmd(0);
            }
            execcmd(0);
        }
        if((pid = fork()) == 0) {
            close(0);
            dup(p[0]);
            close(p[0]);
            close(p[1]);
            if(redir[1]) {
                redircmd(1);
            }
            execcmd(1);
        }
        close(p[0]);
        close(p[1]);
        wait(0);
        wait(0);

    }else {


        int pid = fork();
        if(pid == 0) {
            if(redir[0]) {
                redircmd(0);
            }

            execcmd(0);
        }
        
        wait(0);
    }
}

int peek(char** s, char* es, char* toks) {
    char* ps = *s;
    while(ps < es && strchr(whitespace, *ps)) {
        ps++;
    }
    *s = ps;

    return *ps && (strchr(toks, *ps));
}

char gettoken(char** s, char* es, char** p, char** ep){
    char* ps = *s;
    while(ps < es && strchr(whitespace, *ps)) {
        ps++;
    }
    char tok = *ps;
    if(p) {
        *p = ps;
    }
    switch (tok)
    {
    case '\0':
        break;
    case '|':
    case '<':
    case '>':
        ps++;
        break;
    
    default:
        tok = 'a';
        while((ps < es) && !strchr(whitespace, *ps) && !strchr("|<>", *ps)) {
            // printf("s:%p es:%p\n", ps, es);

            ps++;
        }
        break;
    }
    *s = ps;
    if(ep) {
        *ep = ps;
    }
    return tok;
}

int parseredir(int direct, char **s, char* es) {
    int flag = 0;
    while(peek(s, es, "<>")) {
        char* p;
        char* ep; 
        char tok;
        

        flag = 1;

        tok = gettoken(s, es, 0, 0);
        // printf("s:%p es:%p\n", *s, es);
        gettoken(s, es, &p, &ep);

        // printf("p:%p s:%p es:%p\n", p, *s, es);
        // printf("tok:%c\n", tok);
        int i = count[direct];
        count[direct]++;
        file[direct][i] = p;
        efile[direct][i] = ep;
        // file = p;
        // efile = ep;
       
        switch (tok)
        {
        case '<':
            fd[direct][i] = 0;
            mode[direct][i] = O_RDONLY;
            break;
        case '>':
            fd[direct][i] = 1;
            mode[direct][i] = O_WRONLY | O_CREATE;
            break;
        default:
            break;
        }
    }
    return flag;
}

void parseexec(int direct, char** s, char* es) {
    while(!peek(s, es, "|")) {
        char* p;
        char* ep; 
        
        if(gettoken(s, es, &p, &ep) == 0) {
            break;
        }
        
        // printf("%c\n" , *p);
        argv[direct][argc[direct]] = p;
        eargv[direct][argc[direct]] = ep;
        
        argc[direct] = argc[direct]+1;

        if(parseredir(direct, s, es)) {
            
            redir[direct] = 1;
        }
    }
    argv[direct][argc[direct]] = 0;
    eargv[direct][argc[direct]] = 0;
}

void cleanend(int direct) {
    int i = 0;
    for(; argv[direct][i]; i++) {
        *eargv[direct][i] = 0;
    }
    if(redir[direct]) {
        for(i = 0; i < count[direct]; i++)
        efile[direct][i][0] = 0;
    }
}

void parsecmd(char* s) {
    char* es = s + strlen(s);
    parseexec(0, &s, es);
    cleanend(0);
    if(peek(&s, es, "|")) {
        gettoken(&s, es, 0, 0);
        ispipe = 1;
        parseexec(1, &s, es);
        cleanend(1);
    }

}


void main()
{
    static char buf[512];

    printf("@ ");

    while (getcmd(buf, sizeof(buf)) >= 0)
    {
        

        parsecmd(buf);

        rumcmd();
        // for(int i = 0; i < argc[0]; i++) {
        //     memset(argv[0][i], 0, strlen(argv[0][i]));
        // }
        // for(int i = 0; i < argc[1]; i++) {
        //     memset(argv[1][i], 0, strlen(argv[1][i]));
        // }

        for(int i = 0; i < 2; i++) {
            argc[i] = 0;
            redir[i] = 0;
            count[i] = 0;
        }
        ispipe = 0;


        memset(buf, 0, strlen(buf));

        printf("@ ");

        
        // printf("%s", buf);
        
    }
    exit(0);
}