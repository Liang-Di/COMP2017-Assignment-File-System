#ifndef MYFILESYSTEM_H
#define MYFILESYSTEM_H
#include <sys/types.h>
#include <stdint.h>
#include <pthread.h>
#include <semaphore.h>

typedef struct hashThreadArgs {
	uint8_t * output;
	unsigned int offset;
	char *filename;
	uint8_t *hash_data;
}hashThreadArgs;

typedef struct file {
	struct file* pre;
	struct file* next;
	unsigned int offset;
	unsigned int length;
	unsigned int position; // the position in dictionary_table
}file;


typedef struct fileSystem {
	char *file_data;
	char *directory_table;
	char *hash_data;
	file *file_sequence;
	int n_processors;
	long size;
	pthread_t *threads;
	pthread_mutex_t *fdmutex;
	pthread_mutex_t *dtmutex;
	pthread_mutex_t *hdmutex;
} fileSystem;



void * init_fs(char * f1, char * f2, char * f3, int n_processors);

void close_fs(void * helper);

int create_file(char * filename, size_t length, void * helper);

int resize_file(char * filename, size_t length, void * helper);

int resize_file_nonblocking(char * filename, size_t length, void * helper);

void repack(void * helper);

void repack_nonblocking(void * helper);

int delete_file(char * filename, void * helper);

int rename_file(char * oldname, char * newname, void * helper);

int read_file(char * filename, size_t offset, size_t count, void * buf, void * helper);

int write_file(char * filename, size_t offset, size_t count, void * buf, void * helper);

ssize_t file_size(char * filename, void * helper);

void fletcher(uint8_t * buf, size_t length, uint8_t * output);

void compute_hash_tree(void * helper);

void compute_hash_tree_nonblocking(void * helper);

void compute_hash_block(size_t block_offset, void * helper);

void compute_hash_block_nonblocking(size_t block_offset, void * helper);

int check_hash(size_t block_offset, void * helper);

#endif
