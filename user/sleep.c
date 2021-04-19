#include "kernel/types.h"
#include "user/user.h"
#include "kernel/fcntl.h"

int 
main(int argc, char *argv[]) {
	if(argc == 1) {
		printf("you should give a sleep time!\n");
		exit(1);	
	}else{
		int time = atoi(argv[1]);
		sleep(time);
	}

	exit(0);
}
