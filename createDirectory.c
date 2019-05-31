#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include "createDirectory.h"

void create_directory(char *directory) {
	FILE *f = fopen(directory, "wb");
	fclose(f);
}

void add_file(char *directory, char *filename, size_t length, size_t offset, int index) {
	int dt = open(directory, O_RDWR);
	lseek(dt, index * 72, SEEK_SET);
	char *buf = (char *)calloc(72, 1);
	if(strlen(filename) == 0) {
		write(dt, buf, 72);
	}
	else {
		memcpy(buf, filename, strlen(filename) + 1); // file name
		memcpy(buf + 64, &offset, sizeof(unsigned int)); // file offset
		memcpy(buf + 68, &length, sizeof(unsigned int)); // file length
		write(dt, buf, 72);
	}
	close(dt);
	free(buf);
}

void display_directory(char *directory_table) {
	int dt = open(directory_table, O_RDONLY);
	char buf[72];
	while(read(dt, buf, 72)) {
		printf("File name: %s, %u, %u\n",  buf, *(unsigned int *)(buf + 64), *(unsigned int *)(buf + 68));
	}
	close(dt);
}

int compare_directory(char *directory1, char *directory2) {
	int dt1 = open(directory1, O_RDONLY);
	int dt2 = open(directory2, O_RDONLY);
	char buf1[72];
	char buf2[72];
	while(read(dt1, buf1, 72) && read(dt2, buf2, 72)) {
		if(memcmp(buf1, buf2, 72) != 0) {
			printf("Expect: %s offset: %d  length: %d\n", buf2, *(unsigned int *)(buf2 + 64), *(unsigned int *)(buf2 + 68));
			printf("But: %s offset: %d  length: %d\n", buf1, *(unsigned int *)(buf1 + 64), *(unsigned int *)(buf1 + 68));
			return 1;
		}
	}
	close(dt1);
	close(dt2);
	return 0;
}

