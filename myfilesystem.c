#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <limits.h>
#include <semaphore.h>
#include <math.h>
#include "myfilesystem.h"


pthread_mutex_t fdMutex = PTHREAD_MUTEX_INITIALIZER;


void * init_fs(char * f1, char * f2, char * f3, int n_processors) {
	fileSystem *fs = (fileSystem*)malloc(sizeof(fileSystem));
	fs -> file_data = malloc(strlen(f1) + 1);
	fs -> directory_table = malloc(strlen(f2) + 1);
	fs -> hash_data = malloc(strlen(f3) + 1);
	strcpy(fs -> file_data, f1);
	strcpy(fs -> directory_table, f2);
	strcpy(fs -> hash_data, f3);
	fs -> n_processors = n_processors;
	fs -> threads = malloc(n_processors * sizeof(pthread_t));
	
	//open file_data
	FILE *fd = fopen(f1, "r");
	fseek(fd, 0L, SEEK_END);
	long size = ftell(fd);
	fs -> size = size;
	fclose(fd);
	
	//open dictionary_table
	int dt = open(f2, O_RDONLY);
	char buf[72];
	file *start = NULL;
	
	// create the file linked list
	int pos = 0;
	while(read(dt, buf, 72)) {
		if(strlen(buf) != 0) {
			unsigned int current_offset = *(unsigned int*)(buf + 64);
			unsigned int current_length = *(unsigned int*)(buf + 68);
			file *newfile = (file *)malloc(sizeof(file));
			newfile -> pre = NULL;
			newfile -> next = NULL;
			newfile -> position = pos;
			newfile -> length = current_length;
			newfile -> offset = current_offset;
			if(start == NULL) {
				// no file in the sequence
				start = newfile;
			}
			else {
				file *cursor = start;
				file *pre_cursor = NULL;
				while(cursor != NULL) {
					if(cursor -> offset > current_offset) {
						break;
					}
					pre_cursor = cursor;
					cursor = cursor -> next;
				}
				if(cursor == NULL) {
					// the end of file sequence
					cursor = pre_cursor;
					cursor -> next = newfile;
					newfile -> pre = cursor;
				}
				else {
					if(cursor == start) {
						cursor -> pre = newfile;
						newfile -> next = cursor;
						start = newfile;
					}
					else {
						
						newfile -> next = cursor;
						newfile -> pre = cursor -> pre;
						cursor -> pre -> next = newfile;
						cursor -> pre = newfile;
					}
					
				}
			}
		}
		pos ++;
	}
	fs -> file_sequence = start;
	close(dt);
    return (void *)fs;
}


void close_fs(void * helper) {
	free(((fileSystem*)helper) -> file_data);
	free(((fileSystem*)helper) -> directory_table);
	free(((fileSystem*)helper) -> hash_data);
	free(((fileSystem*)helper) -> threads);
	file *cursor = ((fileSystem*)helper) -> file_sequence;
	while(cursor != NULL) {
		file *next = cursor -> next;
		free(cursor);
		cursor = next;
	}
    free(helper);
}


int create_file(char * filename, size_t length, void * helper) {
	pthread_mutex_lock(&fdMutex);
	fileSystem *fs = (fileSystem*)helper;
	
	// open the dictionary table
	int dt = open(fs -> directory_table, O_RDWR);
	// check whether the file already exists
	char file_name[72];
	lseek(dt, 0, SEEK_SET);
	while(read(dt, file_name, 72)) {
		// find a valid position for file
		if(strcmp(file_name, filename) == 0) {
			close(dt);
			pthread_mutex_unlock(&fdMutex);
			return 1; // file already exists
		}
	}
	
	// open the file_data
	int fd = open(fs -> file_data, O_RDWR);
	char *file_content = (char *)calloc(72, 1);
	if(strlen(filename) + 1 < 64) {
		memcpy(file_content, filename, strlen(filename) + 1); // file name
	}
	else {
		memcpy(file_content, filename, 63); // file name truncate the characters beyonds
			   file_content[63] = '\0';
	}
	memcpy(file_content + 68, &length, sizeof(unsigned int)); // file length
	
	// file does not already exist
	unsigned int file_offset = 0; // file's offset
	file *cursor = fs -> file_sequence;
	
	if(cursor == NULL) {
		// no files in dictionary table
		file *new_file = (file *)malloc(sizeof(file));
		new_file -> pre = NULL;
		new_file -> next = NULL;
		new_file -> position = 0;
		new_file -> offset = 0;
		new_file -> length = length;
		fs -> file_sequence = new_file;
		memcpy(file_content + 64, &file_offset, sizeof(unsigned int)); // file offset
		lseek(dt, 0, SEEK_SET);
		write(dt, file_content, 72);
		char *data = (char *)calloc(length, sizeof(char));
		write(fd, data, length);
		free(data);
	}
	else {
		// we need to find the correct position for the file
		
		// check the unallocated position
		unsigned int pre_file_len = 0;
		unsigned int pre_file_offset = 0;
		file *pre_cursor = NULL;
		int flag = 0;
		while(cursor != NULL) {
			unsigned int space = cursor -> offset - pre_file_offset - pre_file_len;
			
			if(space >= length) {
				flag = 1;
				file_offset = pre_file_offset + pre_file_len;
				break;
			}
			pre_file_len = cursor -> length;
			pre_file_offset = cursor -> offset;
			pre_cursor = cursor;
			cursor = cursor -> next;
		}
		
		if(flag) {
			// find an unallocated space before end
			memcpy(file_content + 64, &file_offset, sizeof(unsigned int)); // file offset
			
			// find the place in dictionary_table
			lseek(dt, 0, SEEK_SET);
			int pos = 0; // set position to 0
			char buf[72];
			while(read(dt, buf, 72)) {
				if(buf[0] == '\0') {
					break;
				}
				pos ++;
			}
			// write dictionary_table
			lseek(dt, pos * 72, SEEK_SET);
			write(dt, file_content, 72);
			// create file object
			file *new_file = (file *)malloc(sizeof(file));
			new_file -> pre = NULL;
			new_file -> next = NULL;
			new_file -> position = pos;
			new_file -> length = length;
			new_file -> offset = file_offset;
			if(pre_cursor == NULL) {
				// should be fisrt item in the sequence
				fs -> file_sequence = new_file;
				new_file -> next = cursor;
				cursor -> pre = new_file;
			}
			else {
				pre_cursor -> next = new_file;
				new_file -> next = cursor;
				new_file -> pre = pre_cursor;
				cursor -> pre = new_file;
			}
			lseek(fd, file_offset, SEEK_SET);
			char *data = (char *)calloc(length, sizeof(char));
			write(fd, data, length);
			free(data);
		}
		else {
			// does not have valid unallocated space
			cursor = pre_cursor; // avoid null
			unsigned int last_offset = cursor -> offset;
			unsigned int last_length = cursor -> length;
			file_offset = last_offset + last_length;
			if(file_offset + length > fs -> size) {
				// no more space
				// need to implement repack
				repack_nonblocking(helper);
				cursor = fs -> file_sequence;
				while(cursor -> next != NULL) {
					cursor = cursor -> next;
				}
				if(cursor -> length + length > fs -> size) {
					free(file_content);
					pthread_mutex_unlock(&fdMutex);
					return 2;
				}
				else {
					file_offset = cursor -> offset + cursor -> length;
					memcpy(file_content + 64, &file_offset, sizeof(unsigned int)); // file offset
					int pos = 0; // set position to 0
					char buf[72];
					lseek(dt, 0, SEEK_SET);
					while(read(dt, buf, 72)) {
						if(buf[0] == '\0') {
							break;
						}
						pos ++;
					}
					file *new_file = (file *)malloc(sizeof(file));
					new_file -> next = NULL;
					new_file -> pre = cursor;
					new_file -> position = pos;
					new_file -> length = length;
					new_file -> offset = file_offset;
					cursor -> next = new_file;
					lseek(dt, pos * 72, SEEK_SET);
					write(dt, file_content, 72);

					lseek(fd, file_offset, SEEK_SET);
					char *data = (char *)calloc(length, sizeof(char));
					write(fd, data, length);
					free(data);
				}
				
			}
			else {
				memcpy(file_content + 64, &file_offset, sizeof(unsigned int)); // file offset
				// find the place in dictionary_table
				lseek(dt, 0, SEEK_SET);
				int pos = 0; // set position to 0
				char buf[72];
				while(read(dt, buf, 72)) {
					if(buf[0] == '\0') {
						break;
					}
					pos ++;
				}
				file *new_file = (file *)malloc(sizeof(file));
				new_file -> next = NULL;
				new_file -> pre = cursor;
				new_file -> position = pos;
				new_file -> length = length;
				new_file -> offset = file_offset;
				cursor -> next = new_file;

				lseek(dt, pos * 72, SEEK_SET);
				write(dt, file_content, 72);

				lseek(fd, file_offset, SEEK_SET);
				char *data = (char *)calloc(length, sizeof(char));
				write(fd, data, length);
				free(data);
			}
		}
	}
	free(file_content);
	close(dt); // close dictionary_table
	close(fd); // close file_data
	compute_hash_tree_nonblocking(helper);
	pthread_mutex_unlock(&fdMutex);
	
    return 0;
}


int resize_file(char * filename, size_t length, void * helper) {
	pthread_mutex_lock(&fdMutex);
	fileSystem *fs = (fileSystem*)helper;
	// open the dictionary table
	int dt = open(fs -> directory_table, O_RDWR);
	
	char current_file[72];
	int pos = 0;
	int flag = 0;
	lseek(dt, 0, SEEK_SET);
	while(read(dt, current_file, 72)) {
		if(strcmp(current_file, filename) == 0) {
			// find the position of file in the dictionary table
			flag = 1;
			break;
		}
		pos ++;
	}
	if(!flag) {
		// file does not exists
		pthread_mutex_unlock(&fdMutex);
		return 1;
	}
	
	file *cursor = fs -> file_sequence;
	while(cursor -> position != pos) {
		// find the file in the sequence
		cursor = cursor -> next;
	}
	unsigned int current_offset = *(unsigned int *)(current_file + 64);
	unsigned int current_length = *(unsigned int *)(current_file + 68);
	if(cursor -> next == NULL) {
		// current file is at the end of the file_data
		if(current_offset + length > fs -> size) {
			// need to repack
			int fd = open(fs -> file_data, O_RDWR);
			repack_nonblocking(helper);
			cursor = fs -> file_sequence;
			while(cursor -> position != pos) {
				cursor = cursor -> next;
			}
			if(cursor -> offset + length > fs -> size) {
				close(dt);
				pthread_mutex_unlock(&fdMutex);
				return 2;
			}
			unsigned int read_len = current_length;
			if(current_length >= length) {
				read_len = length;
			}
			// write file_data
			char *new_data = calloc(length, 1);
			lseek(fd, cursor -> offset, SEEK_SET);
			read(fd, new_data, read_len);
			lseek(fd, cursor -> offset, SEEK_SET);
			write(fd, new_data, length);
			close(fd);
			free(new_data);
			// write dictionary_table
			lseek(dt, pos*72 + 68, SEEK_SET);
			write(dt, (void *)&length, 4);
			cursor -> length = length;
		}
		else {
			unsigned int read_len = current_length;
			if(current_length >= length) {
				read_len = length;
			}
			// write file_data
			char *new_data = calloc(length, 1);
			int fd = open(fs -> file_data, O_RDWR);
			lseek(fd, current_offset, SEEK_SET);
			read(fd, new_data, read_len);
			lseek(fd, current_offset, SEEK_SET);
			write(fd, new_data, length);
			close(fd);
			free(new_data);
			// write dictionary_table
			lseek(dt, pos*72 + 68, SEEK_SET);
			write(dt, (void *)&length, 4);
			cursor -> length = length;
		}
	}
	else {
		unsigned int next_offset = cursor -> next -> offset;
		if(current_offset + length > next_offset) {
			// need to repack
			int fd = open(fs -> file_data, O_RDWR);
			char *new_data = calloc(length, 1);
			lseek(fd, current_offset, SEEK_SET);
			read(fd, new_data, current_length);
			// keep the current file
			file *temp = cursor;
			// delete the file which should be resized
			if(cursor -> pre != NULL) {
				cursor -> pre -> next = cursor -> next;
			}
			else {
				fs -> file_sequence = cursor -> next;
			}
			cursor -> next -> pre = cursor -> pre;
			// repack the file
			repack_nonblocking(helper);
			cursor = fs -> file_sequence;
			// find the last file
			while(cursor -> next != NULL) {
				cursor = cursor -> next;
			}
			if(cursor -> length + length > fs -> size) {
				// still not enough space
				free(new_data);
				close(fd);
				close(dt);
				pthread_mutex_unlock(&fdMutex);
				return 2;
			}
			// set the new file
			temp -> offset = cursor -> offset + cursor -> length;
			temp -> length = length;
			temp -> pre = cursor;
			temp -> next = NULL;
			cursor -> next = temp;
			// write file_data
			lseek(fd, temp -> offset, SEEK_SET);
			write(fd, new_data, length);
			close(fd);
			free(new_data);
			// write dictionary_table
			lseek(dt, pos*72 + 68, SEEK_SET);
			write(dt, (void *)&length, 4);
			lseek(dt, pos*72 + 64, SEEK_SET);
			write(dt, (void *)&(temp -> offset), 4);
		}
		else {
			unsigned int read_len = current_length;
			if(current_length >= length) {
				read_len = length;
			}
			// write file_data
			char *new_data = calloc(length, 1);
			int fd = open(fs -> file_data, O_RDWR);
			lseek(fd, current_offset, SEEK_SET);
			read(fd, new_data, read_len);
			lseek(fd, current_offset, SEEK_SET);
			write(fd, new_data, length);
			close(fd);
			free(new_data);
			// write dictionary_table
			lseek(dt, pos*72 + 68, SEEK_SET);
			write(dt, (void *)&length, 4);
			cursor -> length = length;
		}
	}
	
	close(dt);
	compute_hash_tree_nonblocking(helper);
	pthread_mutex_unlock(&fdMutex);
    return 0;
}

int resize_file_nonblocking(char * filename, size_t length, void * helper) {
	fileSystem *fs = (fileSystem*)helper;
	// open the dictionary table
	int dt = open(fs -> directory_table, O_RDWR);
	
	char current_file[72];
	int pos = 0;
	int flag = 0;
	lseek(dt, 0, SEEK_SET);
	while(read(dt, current_file, 72)) {
		if(strcmp(current_file, filename) == 0) {
			// find the position of file in the dictionary table
			flag = 1;
			break;
		}
		pos ++;
	}
	if(!flag) {
		// file does not exists
		return 1;
	}
	
	file *cursor = fs -> file_sequence;
	while(cursor -> position != pos) {
		// find the file in the sequence
		cursor = cursor -> next;
	}
	unsigned int current_offset = *(unsigned int *)(current_file + 64);
	unsigned int current_length = *(unsigned int *)(current_file + 68);
	if(cursor -> next == NULL) {
		// current file is at the end of the file_data
		if(current_offset + length > fs -> size) {
			// need to repack
			int fd = open(fs -> file_data, O_RDWR);
			repack_nonblocking(helper);
			cursor = fs -> file_sequence;
			while(cursor -> position != pos) {
				cursor = cursor -> next;
			}
			if(cursor -> offset + length > fs -> size) {
				close(dt);
				return 2;
			}
			unsigned int read_len = current_length;
			if(current_length >= length) {
				read_len = length;
			}
			// write file_data
			char *new_data = calloc(length, 1);
			lseek(fd, cursor -> offset, SEEK_SET);
			read(fd, new_data, read_len);
			lseek(fd, cursor -> offset, SEEK_SET);
			write(fd, new_data, length);
			close(fd);
			free(new_data);
			// write dictionary_table
			lseek(dt, pos*72 + 68, SEEK_SET);
			write(dt, (void *)&length, 4);
			cursor -> length = length;
		}
		else {
			unsigned int read_len = current_length;
			if(current_length >= length) {
				read_len = length;
			}
			// write file_data
			char *new_data = calloc(length, 1);
			int fd = open(fs -> file_data, O_RDWR);
			lseek(fd, current_offset, SEEK_SET);
			read(fd, new_data, read_len);
			lseek(fd, current_offset, SEEK_SET);
			write(fd, new_data, length);
			close(fd);
			free(new_data);
			// write dictionary_table
			lseek(dt, pos*72 + 68, SEEK_SET);
			write(dt, (void *)&length, 4);
			cursor -> length = length;
		}
	}
	else {
		unsigned int next_offset = cursor -> next -> offset;
		if(current_offset + length > next_offset) {
			// need to repack
			int fd = open(fs -> file_data, O_RDWR);
			char *new_data = calloc(length, 1);
			lseek(fd, current_offset, SEEK_SET);
			read(fd, new_data, current_length);
			// keep the current file
			file *temp = cursor;
			// delete the file which should be resized
			if(cursor -> pre != NULL) {
				cursor -> pre -> next = cursor -> next;
			}
			else {
				fs -> file_sequence = cursor -> next;
			}
			cursor -> next -> pre = cursor -> pre;
			// repack the file
			repack_nonblocking(helper);
			cursor = fs -> file_sequence;
			// find the last file
			while(cursor -> next != NULL) {
				cursor = cursor -> next;
			}
			if(cursor -> length + length > fs -> size) {
				// still not enough space
				free(new_data);
				close(fd);
				close(dt);
				return 2;
			}
			// set the new file
			temp -> offset = cursor -> offset + cursor -> length;
			temp -> length = length;
			temp -> pre = cursor;
			temp -> next = NULL;
			cursor -> next = temp;
			// write file_data
			lseek(fd, temp -> offset, SEEK_SET);
			write(fd, new_data, length);
			close(fd);
			free(new_data);
			// write dictionary_table
			lseek(dt, pos*72 + 68, SEEK_SET);
			write(dt, (void *)&length, 4);
			lseek(dt, pos*72 + 64, SEEK_SET);
			write(dt, (void *)&(temp -> offset), 4);
		}
		else {
			unsigned int read_len = current_length;
			if(current_length >= length) {
				read_len = length;
			}
			// write file_data
			char *new_data = calloc(length, 1);
			int fd = open(fs -> file_data, O_RDWR);
			lseek(fd, current_offset, SEEK_SET);
			read(fd, new_data, read_len);
			lseek(fd, current_offset, SEEK_SET);
			write(fd, new_data, length);
			close(fd);
			free(new_data);
			// write dictionary_table
			lseek(dt, pos*72 + 68, SEEK_SET);
			write(dt, (void *)&length, 4);
			cursor -> length = length;
		}
	}
	compute_hash_tree_nonblocking(helper);
	close(dt);
    return 0;
}


void repack(void * helper) {
	pthread_mutex_lock(&fdMutex);
	fileSystem *fs = (fileSystem*)helper;
	
	// open the dictionary table & file_data
	int dt = open(fs -> directory_table, O_RDWR); 
	int fd = open(fs -> file_data, O_RDWR);
	
	unsigned int pre_file_len = 0;
	unsigned int pre_file_offset = 0;
	file *cursor = fs -> file_sequence;
	while(cursor != NULL) {
		if(cursor -> offset - pre_file_offset - pre_file_len > 0 ){
			// find a hole
			unsigned int new_offset = pre_file_offset + pre_file_len;
			char *file_content = malloc(cursor -> length);
			lseek(fd, cursor -> offset, SEEK_SET);
			read(fd, file_content, cursor -> length);
			lseek(fd, new_offset, SEEK_SET);
			write(fd, file_content, cursor -> length);
			cursor -> offset = new_offset;
			lseek(dt, cursor -> position * 72 + 64, SEEK_SET);
			write(dt, &new_offset, sizeof(unsigned int));
			free(file_content);
		}
		pre_file_len = cursor -> length;
		pre_file_offset = cursor -> offset;
		cursor = cursor -> next;
	}
	close(fd);
	close(dt);
	compute_hash_tree_nonblocking(helper);
	pthread_mutex_unlock(&fdMutex);
    return;
}

void repack_nonblocking(void * helper) {
	fileSystem *fs = (fileSystem*)helper;
	
	// open the dictionary table & file_data
	int dt = open(fs -> directory_table, O_RDWR); 
	int fd = open(fs -> file_data, O_RDWR);
	
	unsigned int pre_file_len = 0;
	unsigned int pre_file_offset = 0;
	file *cursor = fs -> file_sequence;
	while(cursor != NULL) {
		if(cursor -> offset - pre_file_offset - pre_file_len > 0 ){
			// find a hole
			unsigned int new_offset = pre_file_offset + pre_file_len;
			char *file_content = malloc(cursor -> length);
			lseek(fd, cursor -> offset, SEEK_SET);
			read(fd, file_content, cursor -> length);
			lseek(fd, new_offset, SEEK_SET);
			write(fd, file_content, cursor -> length);
			cursor -> offset = new_offset;
			lseek(dt, cursor -> position * 72 + 64, SEEK_SET);
			write(dt, &new_offset, sizeof(unsigned int));
			free(file_content);
		}
		pre_file_len = cursor -> length;
		pre_file_offset = cursor -> offset;
		cursor = cursor -> next;
	}
	close(fd);
	close(dt);
	compute_hash_tree_nonblocking(helper);
    return;
}


int delete_file(char * filename, void * helper) {
	pthread_mutex_lock(&fdMutex);
	fileSystem *fs = (fileSystem*)helper;
	
	// open the dictionary table
	int dt = open(fs -> directory_table, O_RDWR);
	
	char buf[72];
	int pos = 0;
	lseek(dt, 0, SEEK_SET);
	while(read(dt, buf, 72)) {
		if(strcmp(buf, filename) == 0) {
			
			// find the file in file sequence
			file *cursor = fs -> file_sequence;
			while(cursor -> position != pos) {
				cursor = cursor -> next;
			}
			// delete file in the sequence
			if(cursor -> pre == NULL) {
				// file is the first item in the sequence
				if(cursor -> next != NULL) {
					cursor -> next ->pre = NULL;
				}
				fs -> file_sequence = cursor -> next;
			}
			else {
				cursor -> pre -> next = cursor -> next;
				if(cursor -> next != NULL) {
					cursor -> next -> pre = cursor -> pre;
				}
			}
			free(cursor);
			lseek(dt, pos*72, SEEK_SET);
			char *newfile = (char *)calloc(72, 1);
			write(dt, newfile, 72);
			free(newfile);
			close(dt);
			pthread_mutex_unlock(&fdMutex);
			return 0;
		}
		pos ++;
	}
	close(dt);
	pthread_mutex_unlock(&fdMutex);
    return 1;
}


int rename_file(char * oldname, char * newname, void * helper) {
	pthread_mutex_lock(&fdMutex);
	fileSystem *fs = (fileSystem*)helper;
	
	// open the dictionary table
	int dt = open(fs -> directory_table, O_RDWR);
	
	// check whether the file already exists
	char file_name[72];
	lseek(dt, 0, SEEK_SET);
	while(read(dt, (void *)file_name, 72) != 0) {
		// find a valid position for file
		if(strcmp(file_name, newname) == 0) {
			pthread_mutex_unlock(&fdMutex);
			return 1; // file already exists
		}
	}
	
	char buf[72];
	int count = 0;
	lseek(dt, 0, SEEK_SET);
	while(read(dt, buf, 72)) {
		if(strcmp(buf, oldname) == 0) {
			lseek(dt, count*72, SEEK_SET);
			char *newfile = (char *)calloc(72, 1);
			memcpy(newfile, newname, strlen(newname) + 1);
			memcpy(newfile + 64, buf + 64, 8);
			write(dt, newfile, 72);
			free(newfile);
			close(dt);
			pthread_mutex_unlock(&fdMutex);
			return 0;
		}
		count ++;
	}
	close(dt);
	pthread_mutex_unlock(&fdMutex);
    return 1;
}


int read_file(char * filename, size_t offset, size_t count, void * buf, void * helper) {
	pthread_mutex_lock(&fdMutex);
	fileSystem *fs = (fileSystem*)helper;
	int dt = open(fs -> directory_table, O_RDONLY);
	// read the dictionary table
	char file_name[72];
	unsigned int file_offset = -1;
	unsigned int file_szie = 0;
	while(read(dt, (void *)file_name, 72) != 0) {
		// find a valid position for file
		if(strcmp(file_name, filename) == 0) {
			file_offset = *(unsigned int *)((char *)file_name + 64);
			file_szie = *(unsigned int *)((char *)file_name + 68);
		}
		
	}
	if(file_offset == -1) { // file not exists
		pthread_mutex_unlock(&fdMutex);
		return 1;
	}
	if(offset + count > file_szie) { // exceeds the file's length
		pthread_mutex_unlock(&fdMutex);
		return 2;
	}
	int fd = open(fs -> file_data, O_RDONLY);
	int hd = open(fs -> hash_data, O_RDONLY);
	lseek(fd, file_offset + offset, SEEK_SET);
	read(fd, buf, count);
	unsigned int block_offset_start = (file_offset + offset) / 256;
	unsigned int block_offset_end =  (file_offset + offset + count) / 256;
	for(int i = block_offset_start; i <= block_offset_end; i++) {
		if(check_hash(i, helper)) {
			pthread_mutex_unlock(&fdMutex);
			return 3;
		}
	}
	close(dt);
	close(fd);
	close(hd);
	pthread_mutex_unlock(&fdMutex);
    return 0;
}


int write_file(char * filename, size_t offset, size_t count, void * buf, void * helper) {
	pthread_mutex_lock(&fdMutex);
	fileSystem *fs = (fileSystem*)helper;
	int dt = open(fs -> directory_table, O_RDONLY);
	// read the dictionary table
	char file_name[72];
	unsigned int file_offset = -1;
	unsigned int file_szie = 0;
	int pos = 0;
	while(read(dt, (void *)file_name, 72) != 0) {
		// find a valid position for file
		if(strcmp(file_name, filename) == 0) {
			file_offset = *(unsigned int *)((char *)file_name + 64);
			file_szie = *(unsigned int *)((char *)file_name + 68);
			break;
		}
		pos++;
	}
	if(file_offset == -1) { // file not exists
		pthread_mutex_unlock(&fdMutex);
		return 1;
	}
	if(offset > file_szie) { // exceeds the file's length
		pthread_mutex_unlock(&fdMutex);
		return 2;
	}
	unsigned int new_length = count + offset;
	file *cursor = fs -> file_sequence;
	while(cursor -> position != pos) {
		cursor = cursor -> next;
	}
	if(new_length <= cursor -> length) {
		int fd = open(fs -> file_data, O_WRONLY);
		lseek(fd, cursor -> offset + offset, SEEK_SET);
		write(fd, buf, count);
		close(dt);
		close(fd);
	}
	else {
		int flag = resize_file_nonblocking(filename, new_length, helper);
		if(flag == 0) {
			// can be resize
			int fd = open(fs -> file_data, O_WRONLY);
			lseek(fd, cursor -> offset + offset, SEEK_SET);
			write(fd, buf, count);
			close(dt);
			close(fd);
		}
		else {
			pthread_mutex_unlock(&fdMutex);
			return 3;
		}
	}
	compute_hash_tree_nonblocking(helper);
	pthread_mutex_unlock(&fdMutex);
    return 0;
}


ssize_t file_size(char * filename, void * helper) {
	fileSystem *fs = (fileSystem*)helper;
	
	// open the dictionary table
	int dt = open(fs -> directory_table, O_RDWR);
	
	// check whether the file already exists
	char file[72];
	lseek(dt, 0, SEEK_SET);
	ssize_t size = -1;
	while(read(dt, (void *)file, 72) != 0) {
		// find a valid position for file
		if(strcmp(file, filename) == 0) {
			size = *(unsigned int*)(file + 68); // file already exists
			break;
		}
	}
	if(size == -1) {
		// file not exists
		return -1;
	}
	
	close(dt);
    return size;
}



void fletcher(uint8_t * buf, size_t length, uint8_t * output) {
	size_t pad_len = 0;
	if(length % 4 != 0) {
		pad_len = 4 - (length % 4);
	}
	uint32_t *data = (uint32_t *)calloc(length + pad_len, sizeof(uint8_t));
	memcpy(data, buf, length * sizeof(uint8_t));
	uint64_t a = 0; //Treat 4 byte blocks as little-endian integers
	uint64_t b = 0;
	uint64_t c = 0;
	uint64_t d = 0;
	for (int i = 0; i < (length + pad_len) / 4; i++) {
		a = (a + data[i]) % ((uint64_t)pow(2, 32) - 1);
		b = (b + a) % ((uint64_t)powl(2, 32) - 1);
		c = (c + b) % ((uint64_t)powl(2, 32) - 1);
		d = (d + c) % ((uint64_t)powl(2, 32) - 1);
	}
	
	uint32_t *result = (uint32_t *)output;
	uint32_t aConvert = (uint32_t)a;
	uint32_t bConvert = (uint32_t)b;
	uint32_t cConvert = (uint32_t)c;
	uint32_t dConvert = (uint32_t)d;
	memcpy(result + 0, &aConvert, sizeof(uint32_t));
	memcpy(result + 1, &bConvert, sizeof(uint32_t));
	memcpy(result + 2, &cConvert, sizeof(uint32_t));
	memcpy(result + 3, &dConvert, sizeof(uint32_t));
	
	free(data);
    return;
}

/********** multithread vertion of compute_hash_tree **************
 I tried to use multithreads to speed up my function.
 It can work on my own computer but failed on ed. 
 Just comment this part.
 *****************************************************************/

// void *threadHashBase(void *arg) {
// 	hashThreadArgs *args = (hashThreadArgs *)arg;
// 	int d = open(args -> filename, O_RDONLY);
// 	lseek(d, args -> offset, SEEK_SET);
// 	uint8_t *buf = (uint8_t *)calloc(256, 1);
// 	read(d, buf, 256);
// 	fletcher(buf, 256, args -> output);
// 	free(buf);
// 	close(d);
// 	pthread_exit(NULL);
// }


// void *threadHashLevel(void *arg){
// 	hashThreadArgs *args = (hashThreadArgs *)arg;
// 	uint8_t *hash_buf = (uint8_t *)malloc(32*sizeof(uint8_t));
// 	memcpy(hash_buf, args -> hash_data + args -> offset * 16, 16);
// 	memcpy(hash_buf + 16, args -> hash_data + (args -> offset + 1) * 16, 16);
// 	fletcher(hash_buf, 32, args -> hash_data + ((args -> offset) / 2)*16);
// 	free(hash_buf);
// 	pthread_exit(NULL);
// }


// void compute_hash_tree(void * helper) {
// 	pthread_mutex_lock(&fdMutex);
// 	fileSystem *fs = (fileSystem*)helper;
// 	int fd = open(fs -> file_data, O_RDONLY);
// 	int hd = open(fs -> hash_data, O_WRONLY);
// 	long nodeNum = fs -> size / 256;
	
// 	// multithread
// 	hashThreadArgs *args = (hashThreadArgs *)malloc(fs -> n_processors * sizeof(hashThreadArgs));
	
// 	uint8_t *hash_data = (uint8_t *)malloc(nodeNum * 16);
// 	unsigned int counter = 0;
// 	long nodesLeft;
// 	while(counter < nodeNum) {
// 		nodesLeft = nodeNum - counter;
// 		if(nodesLeft >= fs -> n_processors) {
// 			for(int i = 0; i < fs -> n_processors; i++) {
// 				args[i].offset = counter*256;
// 				args[i].output = hash_data + counter*16;
// 				args[i].filename = fs -> file_data;
// 				pthread_create(&(fs -> threads[i]), NULL, threadHashBase, (void *)(&(args[i])));
// 				// printf("flecher: %d, %d, %d, %d\n",  *(int *)(hash_data + counter*16), *(int *)((hash_data + counter*16) + 4), *(int *)((hash_data + counter*16) + 8), *(int *)((hash_data + counter*16) + 12));
// 				counter++;
// 			}
// 			for(int i = 0; i < fs -> n_processors; i++) {
// 				pthread_join(fs -> threads[i], NULL);
// 			}
// 		}
// 		else {
// 			for(int i = 0; i < nodesLeft; i++) {
// 				args[i].offset = counter*256;
// 				args[i].output = hash_data + counter*16;
// 				args[i].filename = fs -> file_data;
// 				pthread_create(&(fs -> threads[i]), NULL, threadHashBase, (void *)(&(args[i])));
// 				// printf("flecher: %d, %d, %d, %d\n",  *(int *)(hash_data + counter*16), *(int *)((hash_data + counter*16) + 4), *(int *)((hash_data + counter*16) + 8), *(int *)((hash_data + counter*16) + 12));
// 				counter++;
// 			}
// 			for(int i = 0; i < nodesLeft; i++) {
// 				pthread_join(fs -> threads[i], NULL);
// 			}
// 		}
		
		
// 	}
// 	// write in the hash_data
// 	lseek(hd, (nodeNum - 1) * 16, SEEK_SET);
// 	write(hd, hash_data, nodeNum * 16);
	
	
// 	// recursive part to build the whole tree
// 	int cursor = nodeNum - 1 - nodeNum / 2;
// 	long level_nodes = nodeNum;
// 	while(level_nodes != 1) {
// 		counter = 0;
// 		while(counter < level_nodes / 2) {
// 			nodesLeft = level_nodes / 2 - counter;
// 			if(nodesLeft >= fs -> n_processors) {
				
// 				for(int i = 0; i < fs -> n_processors; i++) {
// 					args[i].hash_data = hash_data;
// 					args[i].offset = counter * 2;
// 					pthread_create(&(fs -> threads[i]), NULL, threadHashLevel, (void *)(&(args[i])));
// 					// printf("flecher: %d, %d, %d, %d\n",  *(int *)(hash_data + counter*16), *(int *)((hash_data + counter*16) + 4), *(int *)((hash_data + counter*16) + 8), *(int *)((hash_data + counter*16) + 12));
// 					counter++;
// 				}
// 				for(int i = 0; i < fs -> n_processors; i++) {
// 					pthread_join(fs -> threads[i], NULL);
// 				}
// 			}
// 			else {
				
// 				for(int i = 0; i < nodesLeft; i++) {
// 					args[i].hash_data = hash_data;
// 					args[i].offset = counter * 2;
// 					pthread_create(&(fs -> threads[i]), NULL, threadHashLevel, (void *)(&(args[i])));
// 					// printf("flecher: %d, %d, %d, %d\n",  *(int *)(hash_data + counter*16), *(int *)((hash_data + counter*16) + 4), *(int *)((hash_data + counter*16) + 8), *(int *)((hash_data + counter*16) + 12));
// 					counter++;
// 				}
// 				for(int i = 0; i < nodesLeft; i++) {
// 					pthread_join(fs -> threads[i], NULL);
// 				}
// 			}


// 		}
		
// 		// write the hash_data
// 		lseek(hd, cursor * 16, SEEK_SET);
// 		write(hd, hash_data, level_nodes * 8);
		
// 		level_nodes = level_nodes / 2;
// 		cursor = cursor - level_nodes / 2;
		
		
// 	}
// 	free(args);
// 	free(hash_data);
// 	close(fd);
// 	close(hd);
// 	pthread_mutex_unlock(&fdMutex);
//  return;
// }


void compute_hash_tree(void * helper) {
	
	pthread_mutex_lock(&fdMutex);
	fileSystem *fs = (fileSystem*)helper;
	
	int fd = open(fs -> file_data, O_RDONLY);
	int hd = open(fs -> hash_data, O_WRONLY);
	long nodeNum = fs -> size / 256;
	uint8_t *hash_data = (uint8_t *)malloc(nodeNum * 16);
	unsigned int counter = 0;
	uint8_t *buf = (uint8_t *)calloc(256, 1);
	lseek(fd, 0, SEEK_SET);
	while(read(fd, buf, 256)) {
		fletcher(buf, 256, hash_data + counter*16);
		counter++;
	}
	lseek(hd, (nodeNum - 1) * 16, SEEK_SET);
	write(hd, hash_data, nodeNum * 16);
	free(buf);
	
	// recursive part to build the whole tree
	uint8_t *hash_buf = (uint8_t *)malloc(32*sizeof(uint8_t));
	int cursor = nodeNum - 1 - nodeNum / 2;
	long level_nodes = nodeNum;
	while(level_nodes != 1) {
		for(int i = 0; i < level_nodes; i += 2) {
			memcpy(hash_buf, hash_data + i * 16, 16);
			memcpy(hash_buf + 16, hash_data + (i + 1) * 16, 16);
			fletcher(hash_buf, 32, hash_data + (i / 2)*16);
		}
		lseek(hd, cursor * 16, SEEK_SET);
		write(hd, hash_data, (level_nodes / 2)*16);
		level_nodes = level_nodes / 2;
		cursor = cursor - level_nodes / 2;
	}
	free(hash_buf);
	free(hash_data);
	close(fd);
	close(hd);
	pthread_mutex_unlock(&fdMutex);
    return;
}

void compute_hash_tree_nonblocking(void * helper) {
	fileSystem *fs = (fileSystem*)helper;
	
	int fd = open(fs -> file_data, O_RDONLY);
	int hd = open(fs -> hash_data, O_WRONLY);
	long nodeNum = fs -> size / 256;
	uint8_t *hash_data = (uint8_t *)malloc(nodeNum * 16);
	unsigned int counter = 0;
	uint8_t *buf = (uint8_t *)calloc(256, 1);
	lseek(fd, 0, SEEK_SET);
	while(read(fd, buf, 256)) {
		fletcher(buf, 256, hash_data + counter*16);
		counter++;
	}
	lseek(hd, (nodeNum - 1) * 16, SEEK_SET);
	write(hd, hash_data, nodeNum * 16);
	free(buf);
	
	// recursive part to build the whole tree
	uint8_t *hash_buf = (uint8_t *)malloc(32*sizeof(uint8_t));
	int cursor = nodeNum - 1 - nodeNum / 2;
	long level_nodes = nodeNum;
	while(level_nodes != 1) {
		for(int i = 0; i < level_nodes; i += 2) {
			memcpy(hash_buf, hash_data + i * 16, 16);
			memcpy(hash_buf + 16, hash_data + (i + 1) * 16, 16);
			fletcher(hash_buf, 32, hash_data + (i / 2)*16);
		}
		lseek(hd, cursor * 16, SEEK_SET);
		write(hd, hash_data, (level_nodes / 2)*16);
		level_nodes = level_nodes / 2;
		cursor = cursor - level_nodes / 2;
	}
	free(hash_buf);
	free(hash_data);
	close(fd);
	close(hd);
    return;
}


void compute_hash_block(size_t block_offset, void * helper) {
	pthread_mutex_lock(&fdMutex);
	fileSystem *fs = (fileSystem*)helper;
	int fd = open(fs -> file_data, O_RDONLY);
	int hd = open(fs -> hash_data, O_RDWR);
	long nodeNum = fs -> size / 256;
	size_t pos = block_offset;
	uint8_t *buf = (uint8_t *)malloc(256);
	uint8_t *hash_buf = (uint8_t *)malloc(32);
	lseek(fd, block_offset * 256, SEEK_SET);
	read(fd, buf, 256);
	if(pos % 2 == 0) {
		fletcher(buf, 256, hash_buf);
		lseek(hd, (nodeNum - 1 + block_offset) * 16, SEEK_SET);
		write(hd, hash_buf, 16);
		lseek(hd, (nodeNum - 1 + (pos + 1)) * 16, SEEK_SET);
		read(hd, hash_buf + 16, 16);
	}
	else{
		fletcher(buf, 256, hash_buf + 16);
		lseek(hd, (nodeNum - 1 + block_offset) * 16, SEEK_SET);
		write(hd, hash_buf + 16, 16);
		lseek(hd, (nodeNum - 1 + (pos - 1)) * 16, SEEK_SET);
		read(hd, hash_buf, 16);
	}
	free(buf);
	
	
	pos /= 2;
	long level_nodes = nodeNum / 2;
	int cursor = nodeNum - 1 - nodeNum / 2;
	while(level_nodes != 1 && level_nodes > 0 ){
		if(pos % 2 == 0) {
			fletcher(hash_buf, 32, hash_buf);
			lseek(hd, (cursor + pos) * 16, SEEK_SET);
			write(hd, hash_buf, 16);
			lseek(hd, (cursor + (pos + 1))* 16, SEEK_SET);
			read(hd, hash_buf + 16, 16);
		}
		else {
			fletcher(hash_buf, 32, hash_buf + 16);
			lseek(hd, (cursor + pos) * 16, SEEK_SET);
			write(hd, hash_buf + 16, 16);
			lseek(hd, (cursor + (pos - 1))* 16, SEEK_SET);
			read(hd, hash_buf, 16);
		}
		level_nodes = level_nodes / 2;
		cursor = cursor - level_nodes;
		pos /= 2;
	}
	if(level_nodes == 1) {
		fletcher(hash_buf, 32, hash_buf);
		lseek(hd, 0, SEEK_SET);
		write(hd, hash_buf, 16);
	}
	free(hash_buf);
	close(fd);
	close(hd);
	pthread_mutex_unlock(&fdMutex);
    return;
}

// This function will be used in read_file() function to check the hash_data
int check_hash(size_t block_offset, void * helper){
	fileSystem *fs = (fileSystem*)helper;
	int fd = open(fs -> file_data, O_RDONLY);
	int hd = open(fs -> hash_data, O_RDONLY);
	long nodeNum = fs -> size / 256;
	size_t pos = block_offset;
	uint8_t *buf = (uint8_t *)malloc(256);
	uint8_t *hash_buf = (uint8_t *)malloc(32);
	uint8_t *hash_read = (uint8_t *)malloc(16);
	int result = 0;
	lseek(fd, block_offset * 256, SEEK_SET);
	read(fd, buf, 256);
	if(pos % 2 == 0) {
		fletcher(buf, 256, hash_buf);
		lseek(hd, (nodeNum - 1 + block_offset) * 16, SEEK_SET);
		read(hd, hash_read, 16);
		if(memcmp(hash_buf, hash_read, 16) != 0) {
			result = 1;
		}
		lseek(hd, (nodeNum - 1 + (pos + 1)) * 16, SEEK_SET);
		read(hd, hash_buf + 16, 16);
	}
	else{
		fletcher(buf, 256, hash_buf + 16);
		lseek(hd, (nodeNum - 1 + block_offset) * 16, SEEK_SET);
		read(hd, hash_read, 16);
		if(memcmp(hash_buf + 16, hash_read, 16) != 0) {
			result = 1;
		}
		lseek(hd, (nodeNum - 1 + (pos - 1)) * 16, SEEK_SET);
		read(hd, hash_buf, 16);
	}
	free(buf);
	
	
	pos /= 2;
	long level_nodes = nodeNum ;
	int cursor = nodeNum - 1 - nodeNum / 2;
	while(level_nodes != 1 && level_nodes > 0){
		if(pos % 2 == 0) {
			fletcher(hash_buf, 32, hash_buf);
			lseek(hd, (cursor + pos)* 16, SEEK_SET);
			read(hd, hash_read, 16);
			if(memcmp(hash_buf, hash_read, 16) != 0) {
				result = 1;
				break;
			}
			lseek(hd, (cursor + (pos + 1))* 16, SEEK_SET);
			read(hd, hash_buf + 16, 16);
		}
		else {
			fletcher(hash_buf, 32, hash_buf + 16);
			lseek(hd, (cursor + pos) * 16, SEEK_SET);
			read(hd, hash_read, 16);
			if(memcmp(hash_buf + 16, hash_read, 16) != 0) {
				result = 1;
				break;
			}
			lseek(hd, (cursor + (pos - 1))* 16, SEEK_SET);
			read(hd, hash_buf + 16, 16);
		}
		level_nodes = level_nodes / 2;
		cursor = cursor - level_nodes / 2;
		pos /= 2;
	}
	free(hash_buf);
	free(hash_read);
	close(fd);
	close(hd);
    return result;
}
