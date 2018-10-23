#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <ctype.h>

volatile int stop = 0;

void sig_handler(int signum) {
	printf("you've killed me!");
	stop = 1;
}

int main() {

	signal(SIGINT, sig_handler);	

	FILE *fp =  fopen("test.csv", "w");

	fprintf(fp, "this is a test!\n");
	for(int i = 0; i <= 4; i++) {
		fprintf(fp, "%d,", i);
	}
	fprintf(fp, "\nthats it!");
	while(!stop);
	fclose(fp);
	return 0;
	
}
