#include "mlmalloc.c"
#include <stdio.h>

int main(int argc, char *argv[])
{

	MLON();

	int i, j;
	char *buf = (char *)malloc(128);
	if(!buf) {
		printf("No mem?\n");
		return 1;
	}
	printf("yey, we got mem!\n");

	printf("try to read\n");
	for(i=0;i<128;i++)
		j = buf[i];
	printf("try to read/write\n");

	for(i=0;i<128;i++){
		buf[i]++;
	}
	free(buf);
	return 0;
}
