#include "kernel/types.h"
#include "user/user.h"
#include "kernel/fcntl.h"

int main(void) {
	int p1[2];
	int p2[2];
	char buf[512];
	int pid;
	pipe(p1);
	pipe(p2);
	if(fork() == 0) {
		read(p1[0], buf, sizeof buf);
		pid = getpid();
		printf("%d: received %s\n", pid, buf);
		write(p2[1], "pong", 4);
		close(p1[0]);
		close(p1[1]);
		close(p2[0]);
		close(p2[1]);
				
	}else {
		
		write(p1[1], "ping", 4);
		wait(0);
		read(p2[0], buf, sizeof buf);
		pid = getpid();
		printf("%d: received %s\n", pid, buf);
		close(p1[0]);
		close(p1[1]);
		close(p2[0]);
		close(p2[1]);

				
	}
	exit(0);
}
