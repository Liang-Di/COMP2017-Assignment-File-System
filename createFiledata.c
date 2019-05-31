#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include "createFiledata.h"

void create_Filedata(char *file_data) {
	FILE *f = fopen(file_data, "wb");
	fclose(f);
}

void add_file_block(char *file_data, char *buf, int offset) {
	int fd = open(file_data, O_RDWR);
	lseek(fd, offset*256, SEEK_SET);
	write(fd, buf, 256);
	close(fd);
}

int compare_filedata(char *filedata1, char *filedata2) {
	int fd1 = open(filedata1, O_RDONLY);
	int fd2 = open(filedata2, O_RDONLY);
	char buf1[1];
	char buf2[1];
	unsigned long position = 0;
	while(read(fd1, buf1, 1) && read(fd2, buf2, 1)) {
		if(memcmp(buf1, buf2, 1) != 0) {
			printf("Expect: %d at position: %ld\n", *buf2, position);
			printf("But: %d\n", *buf1);
			return 1;
		}
		position++;
	}
	close(fd1);
	close(fd2);
	return 0;
}

