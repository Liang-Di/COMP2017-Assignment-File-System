#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include "createHashdata.h"

void create_hashdata(char *hashdata) {
	FILE *f = fopen(hashdata, "wb+");
	fclose(f);
}

void add_hash_block(char *hashdata, char *block, int offset) {
	int hd = open(hashdata, O_RDWR);
	lseek(hd, offset * 16, SEEK_SET);
	write(hd, block, 16);
	close(hd);
}

void display_hash(char *hashdata) {
	int hd = open(hashdata, O_RDONLY);
	char buf[16];
	while(read(hd, buf, 16)) {
		printf("block: %d, %d, %d, %d\n",  *(int *)buf, *(int *)(buf + 4), *(int *)(buf + 8), *(int *)(buf + 12));
	}
	close(hd);
}

int compare_hash(char *hashdata1, char *hashdata2) {
	int hd1 = open(hashdata1, O_RDONLY);
	int hd2 = open(hashdata2, O_RDONLY);
	char buf1[16];
	char buf2[16];
	while(read(hd1, buf1, 16) && read(hd2, buf2, 16)) {
		if(memcmp(buf1, buf2, 16) != 0) {
			printf("Expect: %d %d %d %d\n", *(int *)buf2, *(int *)(buf2 + 4), *(int *)(buf2 + 8), *(int *)(buf2 + 12));
			printf("But: %d %d %d %d\n", *(int *)buf1, *(int *)(buf1 + 4), *(int *)(buf1 + 8), *(int *)(buf1 + 12));
			return 1;
		}
	}
	close(hd1);
	close(hd2);
	return 0;
}

