#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

#define TEST(x) test(x, #x)
#include "myfilesystem.h"
#include "createDirectory.h"
#include "createFiledata.h"
#include "createHashdata.h"
#include "runtest.h"

/**************************************READ ME*************************************************************************************
 All test functions are declared in "runtest.h" and are defined below the main function.
 To run all the test cases, use command: "gcc runtest.c myfilesystem.c createFiledata.c createHashdata.c createDirectory.c -o runtest"
 to complie the program and run it with: "./runtest".
 
 There are three useful API files: createDirectory.c, createFiledata.c, createHashdata.c. These three files also have their header
 files. These three files contains functions to build the binary files and the functions to compare the two binary files.
 For comparing function of three kinds of binary files, it will give you some useful information about where you are wrong.
 
 If all the testcases are passed, it should print "Passed + function name". If fail a test, it will print "Failed + function name" and
 also print where you are wrong.
 
 All the test files are in the 'files' folder. All the files are named by their test case names. The files with suffix '_correct' are
 the correct binary file which the program should write. 
 
 All the files will be renewed when you run this 'runtest' program. 
 
 This program can be run multiple times. If you fail a test case, try to delete all files under the folder 'files', and try it again.
 
***************************************************************************************************************************************/

/* Helper function */
void test(int (*test_function) (), char * function_name) {
    int ret = test_function();
    if (ret == 0) {
        printf("Passed %s\n", function_name);
    } else {
        printf("Failed %s returned %d\n", function_name, ret);
    }
}
/************************/

int main(int argc, char * argv[]) {
    
    // no_operation test
	TEST(no_operation);
	
	// create_file function test
	TEST(normalCreate);
	TEST(insertCreate);
	TEST(createFirstAvailableInDir);
	TEST(createWithRepack);
	TEST(createExist);
	TEST(createNoplace);
	
	// resize_file function test
	TEST(resizeFile);
	TEST(resizeNotExist);
	TEST(resizeWithRepack);
	TEST(resizeNoplace);
	
	// repack function test
	TEST(repackTest);
	TEST(repackDirOutOfOrder);
	
	// delete function test
	TEST(deletefileTest);
	TEST(deletefileNotExists);
	
	// rename function test
	TEST(renameTest);
	TEST(renameNotExists);
	
	// read function test
	TEST(readTest);
	TEST(readNotExists);
	TEST(readInvalidOffset);
	TEST(readWithWrongHashValue);
	
	// write function test
	TEST(writeTest);
	TEST(writeNotExists);
	TEST(writeRenewHash);
	TEST(writeWithRepack);
	
	// file_size function test
	TEST(fileSizeTest);
	TEST(fileSizeNotExisting);
	
	// compute_hash_tree function test
	TEST(computeHashTree);
	TEST(computeHashTreeBig);
	TEST(computeHashTreeBig2);
	
	//compute_hash_block function test
	TEST(computeHashBlock);
	TEST(computeHashTreeUseComputeHashBlock);
	TEST(computeHashTreeUseComputeHashBlockBigOne);

    return 0;
}


int no_operation() {
	// no operation
	create_directory("files/Test_no_operation_dir");
	add_file("files/Test_no_operation_dir", "1.doc", 14, 0, 0);
	add_file("files/Test_no_operation_dir", "2.doc", 12, 150, 1);
	add_file("files/Test_no_operation_dir", "3.doc", 11, 200, 2);
	
	create_Filedata("files/Test_no_operation_file");
	char *block = (char *)calloc(256, 1);
	for(int i = 0; i < 4; i++) {
		add_file_block("files/Test_no_operation_file",block, i);
	}
	free(block);
	
	create_hashdata("files/Test_no_operation_hash");
	char *hash_block = (char *)calloc(16, 1);
	for(int i = 0; i < 7; i++) {
		add_hash_block("files/Test_no_operation_hash", hash_block, i);
	}
	free(hash_block);
    void * helper = init_fs("files/Test_no_operation_file", "files/Test_no_operation_dir", "files/Test_no_operation_hash", 4); // Remember you need to provide your own test files and also check their contents as part of testing
    close_fs(helper);
	
	create_directory("files/Test_no_operation_dir_correct");
	add_file("files/Test_no_operation_dir_correct", "1.doc", 14, 0, 0);
	add_file("files/Test_no_operation_dir_correct", "2.doc", 12, 150, 1);
	add_file("files/Test_no_operation_dir_correct", "3.doc", 11, 200, 2);
	create_Filedata("files/Test_no_operation_file_correct");
	block = (char *)calloc(256, 1);
	for(int i = 0; i < 4; i++) {
		add_file_block("files/Test_no_operation_file_correct",block, i);
	}
	free(block);
	
	create_hashdata("files/Test_no_operation_hash_correct");
	hash_block = (char *)calloc(16, 1);
	for(int i = 0; i < 7; i++) {
		add_hash_block("files/Test_no_operation_hash_correct", hash_block, i);
	}
	free(hash_block);
    if(compare_filedata("files/Test_no_operation_file", "files/Test_no_operation_file_correct") || compare_directory("files/Test_no_operation_dir", "files/Test_no_operation_dir_correct") || compare_hash("files/Test_no_operation_hash", "files/Test_no_operation_hash_correct")) {
		return 1;
	}
	return 0;
}

int normalCreate() {
	/* 
		This function tests create file with no repack
	*/
	create_directory("files/Test_normalCreate_dir");
	add_file("files/Test_normalCreate_dir", "1.doc", 14, 0, 0);
	add_file("files/Test_normalCreate_dir", "2.doc", 12, 150, 1);
	add_file("files/Test_normalCreate_dir", "3.doc", 11, 200, 2);
	add_file("files/Test_normalCreate_dir", "\0", 0, 0, 3);
	
	create_Filedata("files/Test_normalCreate_file");
	char *block = (char *)calloc(256, 1);
	for(int i = 0; i < 4; i++) {
		add_file_block("files/Test_normalCreate_file",block, i);
	}
	free(block);
	
	create_hashdata("files/Test_normalCreate_hash");
	char *hash_block = (char *)calloc(16, 1);
	for(int i = 0; i < 7; i++) {
		add_hash_block("files/Test_normalCreate_hash", hash_block, i);
	}
	free(hash_block);
	
	// init file system
	void *helper = init_fs("files/Test_normalCreate_file", "files/Test_normalCreate_dir", "files/Test_normalCreate_hash", 4);
	create_file("4.doc", 200, helper);
	close_fs(helper);
	
	create_directory("files/Test_normalCreate_dir_correct");
	add_file("files/Test_normalCreate_dir_correct", "1.doc", 14, 0, 0);
	add_file("files/Test_normalCreate_dir_correct", "2.doc", 12, 150, 1);
	add_file("files/Test_normalCreate_dir_correct", "3.doc", 11, 200, 2);
	add_file("files/Test_normalCreate_dir_correct", "4.doc", 200, 211, 3);
	
	create_Filedata("files/Test_normalCreate_file_correct");
	block = (char *)calloc(256, 1);
	for(int i = 0; i < 4; i++) {
		add_file_block("files/Test_normalCreate_file_correct",block, i);
	}
	free(block);
	
	create_hashdata("files/Test_normalCreate_hash_correct");
	hash_block = (char *)calloc(16, 1);
	for(int i = 0; i < 7; i++) {
		add_hash_block("files/Test_normalCreate_hash_correct", hash_block, i);
	}
	free(hash_block);
	if(compare_filedata("files/Test_normalCreate_file", "files/Test_normalCreate_file_correct") || compare_directory("files/Test_normalCreate_dir", "files/Test_normalCreate_dir_correct") || compare_hash("files/Test_normalCreate_hash", "files/Test_normalCreate_hash_correct")) {
		return 1;
	}
	return 0;
}

int insertCreate() {
	/* 
		This function tests create file with by finding an available space between files in the file data
	*/
	create_directory("files/Test_insertCreate_dir");
	add_file("files/Test_insertCreate_dir", "1.doc", 14, 0, 0);
	add_file("files/Test_insertCreate_dir", "2.doc", 12, 150, 1);
	add_file("files/Test_insertCreate_dir", "3.doc", 11, 200, 2);
	add_file("files/Test_insertCreate_dir", "\0", 0, 0, 3);
	
	create_Filedata("files/Test_insertCreate_file");
	char *block = (char *)calloc(256, 1);
	for(int i = 0; i < 4; i++) {
		add_file_block("files/Test_insertCreate_file",block, i);
	}
	free(block);
	
	create_hashdata("files/Test_insertCreate_hash");
	char *hash_block = (char *)calloc(16, 1);
	for(int i = 0; i < 7; i++) {
		add_hash_block("files/Test_insertCreate_hash", hash_block, i);
	}
	free(hash_block);
	
	// init file system
	void *helper = init_fs("files/Test_insertCreate_file", "files/Test_insertCreate_dir", "files/Test_insertCreate_hash", 4);
	create_file("4.doc", 10, helper);
	close_fs(helper);
	
	create_directory("files/Test_insertCreate_dir_correct");
	add_file("files/Test_insertCreate_dir_correct", "1.doc", 14, 0, 0);
	add_file("files/Test_insertCreate_dir_correct", "2.doc", 12, 150, 1);
	add_file("files/Test_insertCreate_dir_correct", "3.doc", 11, 200, 2);
	add_file("files/Test_insertCreate_dir_correct", "4.doc", 10, 14, 3);
	
	create_Filedata("files/Test_insertCreate_file_correct");
	block = (char *)calloc(256, 1);
	for(int i = 0; i < 4; i++) {
		add_file_block("files/Test_insertCreate_file_correct",block, i);
	}
	free(block);
	
	create_hashdata("files/Test_insertCreate_hash_correct");
	hash_block = (char *)calloc(16, 1);
	for(int i = 0; i < 7; i++) {
		add_hash_block("files/Test_insertCreate_hash_correct", hash_block, i);
	}
	free(hash_block);
	if(compare_filedata("files/Test_insertCreate_file", "files/Test_insertCreate_file_correct") || compare_directory("files/Test_insertCreate_dir", "files/Test_insertCreate_dir_correct") || compare_hash("files/Test_insertCreate_hash", "files/Test_insertCreate_hash_correct")) {
		return 1;
	}
	return 0;
}

int createFirstAvailableInDir() {
	/* 
		This function tests create file when it has an empty file before other files in the directory_table.
	*/
	create_directory("files/Test_createFirstAvailableInDir_dir");
	add_file("files/Test_createFirstAvailableInDir_dir", "1.doc", 14, 0, 0);
	add_file("files/Test_createFirstAvailableInDir_dir", "2.doc", 12, 150, 1);
	add_file("files/Test_createFirstAvailableInDir_dir", "\0", 0, 0, 2);
	add_file("files/Test_createFirstAvailableInDir_dir", "3.doc", 11, 200, 3);
	
	
	create_Filedata("files/Test_createFirstAvailableInDir_file");
	char *block = (char *)calloc(256, 1);
	for(int i = 0; i < 4; i++) {
		add_file_block("files/Test_createFirstAvailableInDir_file",block, i);
	}
	free(block);
	
	create_hashdata("files/Test_createFirstAvailableInDir_hash");
	char *hash_block = (char *)calloc(16, 1);
	for(int i = 0; i < 7; i++) {
		add_hash_block("files/Test_createFirstAvailableInDir_hash", hash_block, i);
	}
	free(hash_block);
	
	// init file system
	void *helper = init_fs("files/Test_createFirstAvailableInDir_file", "files/Test_createFirstAvailableInDir_dir", "files/Test_createFirstAvailableInDir_hash", 4);
	create_file("4.doc", 10, helper);
	close_fs(helper);
	
	create_directory("files/Test_createFirstAvailableInDir_dir_correct");
	add_file("files/Test_createFirstAvailableInDir_dir_correct", "1.doc", 14, 0, 0);
	add_file("files/Test_createFirstAvailableInDir_dir_correct", "2.doc", 12, 150, 1);
	add_file("files/Test_createFirstAvailableInDir_dir_correct", "4.doc", 10, 14, 2);
	add_file("files/Test_createFirstAvailableInDir_dir_correct", "3.doc", 11, 200, 3);
	
	
	create_Filedata("files/Test_createFirstAvailableInDir_file_correct");
	block = (char *)calloc(256, 1);
	for(int i = 0; i < 4; i++) {
		add_file_block("files/Test_createFirstAvailableInDir_file_correct",block, i);
	}
	free(block);
	
	create_hashdata("files/Test_createFirstAvailableInDir_hash_correct");
	hash_block = (char *)calloc(16, 1);
	for(int i = 0; i < 7; i++) {
		add_hash_block("files/Test_createFirstAvailableInDir_hash_correct", hash_block, i);
	}
	free(hash_block);
	if(compare_filedata("files/Test_createFirstAvailableInDir_file", "files/Test_createFirstAvailableInDir_file_correct") || compare_directory("files/Test_createFirstAvailableInDir_dir", "files/Test_createFirstAvailableInDir_dir_correct") || compare_hash("files/Test_createFirstAvailableInDir_hash", "files/Test_createFirstAvailableInDir_hash_correct")) {
		return 1;
	}
	return 0;
}

int createWithRepack() {
	/* 
		This function tests create file with repack
	*/
	create_directory("files/Test_createWithRepack_dir");
	add_file("files/Test_createWithRepack_dir", "1.doc", 10, 0, 0);
	add_file("files/Test_createWithRepack_dir", "2.doc", 10, 150, 1);
	add_file("files/Test_createWithRepack_dir", "3.doc", 10, 180, 2);
	add_file("files/Test_createWithRepack_dir", "\0", 0, 0, 3);
	
	
	create_Filedata("files/Test_createWithRepack_file");
	char *block = (char *)calloc(256, 1);
	for(int i = 0; i < 1; i++) {
		add_file_block("files/Test_createWithRepack_file",block, i);
	}
	free(block);
	
	create_hashdata("files/Test_createWithRepack_hash");
	char *hash_block = (char *)calloc(16, 1);
	for(int i = 0; i < 1; i++) {
		add_hash_block("files/Test_createWithRepack_hash", hash_block, i);
	}
	free(hash_block);
	
	// init file system
	void *helper = init_fs("files/Test_createWithRepack_file", "files/Test_createWithRepack_dir", "files/Test_createWithRepack_hash", 4);
	create_file("4.doc", 200, helper);
	close_fs(helper);
	
	create_directory("files/Test_createWithRepack_dir_correct");
	add_file("files/Test_createWithRepack_dir_correct", "1.doc", 10, 0, 0);
	add_file("files/Test_createWithRepack_dir_correct", "2.doc", 10, 10, 1);
	add_file("files/Test_createWithRepack_dir_correct", "3.doc", 10, 20, 2);
	add_file("files/Test_createWithRepack_dir_correct", "4.doc", 200, 30, 3);
	
	create_Filedata("files/Test_createWithRepack_file_correct");
	block = (char *)calloc(256, 1);
	for(int i = 0; i < 1; i++) {
		add_file_block("files/Test_createWithRepack_file_correct",block, i);
	}
	free(block);
	
	create_hashdata("files/Test_createWithRepack_hash_correct");
	hash_block = (char *)calloc(16, 1);
	for(int i = 0; i < 1; i++) {
		add_hash_block("files/Test_createWithRepack_hash_correct", hash_block, i);
	}
	free(hash_block);
	if(compare_filedata("files/Test_createWithRepack_file", "files/Test_createWithRepack_file_correct") || compare_directory("files/Test_createWithRepack_dir", "files/Test_createWithRepack_dir_correct") || compare_hash("files/Test_createWithRepack_hash", "files/Test_createWithRepack_hash_correct")) {
		return 1;
	}
	return 0;
}

int createExist() {
	/* 
		This function tests create an existed file.
		It should return 1
	*/
	create_directory("files/Test_createExist_dir");
	add_file("files/Test_createExist_dir", "1.doc", 10, 0, 0);
	add_file("files/Test_createExist_dir", "2.doc", 10, 150, 1);
	add_file("files/Test_createExist_dir", "3.doc", 10, 180, 2);
	add_file("files/Test_createExist_dir", "\0", 0, 0, 3);
	
	
	create_Filedata("files/Test_createExist_file");
	char *block = (char *)calloc(256, 1);
	for(int i = 0; i < 1; i++) {
		add_file_block("files/Test_createExist_file",block, i);
	}
	free(block);
	
	create_hashdata("files/Test_createExist_hash");
	char *hash_block = (char *)calloc(16, 1);
	for(int i = 0; i < 1; i++) {
		add_hash_block("files/Test_createExist_hash", hash_block, i);
	}
	free(hash_block);
	
	// init file system
	void *helper = init_fs("files/Test_createExist_file", "files/Test_createExist_dir", "files/Test_createExist_hash", 4);
	int result = create_file("1.doc", 200, helper);
	close_fs(helper);
	
	create_directory("files/Test_createExist_dir_correct");
	add_file("files/Test_createExist_dir_correct", "1.doc", 10, 0, 0);
	add_file("files/Test_createExist_dir_correct", "2.doc", 10, 150, 1);
	add_file("files/Test_createExist_dir_correct", "3.doc", 10, 180, 2);
	add_file("files/Test_createExist_dir_correct", "\0", 0, 0, 3);
	
	create_Filedata("files/Test_createExist_file_correct");
	block = (char *)calloc(256, 1);
	for(int i = 0; i < 1; i++) {
		add_file_block("files/Test_createExist_file_correct",block, i);
	}
	free(block);
	
	create_hashdata("files/Test_createExist_hash_correct");
	hash_block = (char *)calloc(16, 1);
	for(int i = 0; i < 1; i++) {
		add_hash_block("files/Test_createExist_hash_correct", hash_block, i);
	}
	free(hash_block);
	if(compare_filedata("files/Test_createExist_file", "files/Test_createExist_file_correct") || compare_directory("files/Test_createExist_dir", "files/Test_createExist_dir_correct") || compare_hash("files/Test_createExist_hash", "files/Test_createExist_hash_correct")) {
		return 1;
	}
	if(result == 1) {
		return 0;
	}
	return result;
}

int createNoplace() {
	/* 
		This function tests create an large file which the file_data does not have enough space
		Function create_file should return 2.
	*/
	create_directory("files/Test_createNoplace_dir");
	add_file("files/Test_createNoplace_dir", "1.doc", 10, 0, 0);
	add_file("files/Test_createNoplace_dir", "2.doc", 10, 150, 1);
	add_file("files/Test_createNoplace_dir", "3.doc", 10, 180, 2);
	add_file("files/Test_createNoplace_dir", "\0", 0, 0, 3);
	
	
	create_Filedata("files/Test_createNoplace_file");
	char *block = (char *)calloc(256, 1);
	for(int i = 0; i < 1; i++) {
		add_file_block("files/Test_createNoplace_file",block, i);
	}
	free(block);
	
	create_hashdata("files/Test_createNoplace_hash");
	char *hash_block = (char *)calloc(16, 1);
	for(int i = 0; i < 1; i++) {
		add_hash_block("files/Test_createNoplace_hash", hash_block, i);
	}
	free(hash_block);
	
	// init file system
	void *helper = init_fs("files/Test_createNoplace_file", "files/Test_createNoplace_dir", "files/Test_createNoplace_hash", 4);
	int result = create_file("4.doc", 256, helper);
	close_fs(helper);
	
	create_directory("files/Test_createNoplace_dir_correct");
	// file should be repacked to find whether it has enough space
	add_file("files/Test_createNoplace_dir_correct", "1.doc", 10, 0, 0);
	add_file("files/Test_createNoplace_dir_correct", "2.doc", 10, 10, 1);
	add_file("files/Test_createNoplace_dir_correct", "3.doc", 10, 20, 2);
	add_file("files/Test_createNoplace_dir_correct", "\0", 0, 0, 3);
	
	create_Filedata("files/Test_createNoplace_file_correct");
	block = (char *)calloc(256, 1);
	for(int i = 0; i < 1; i++) {
		add_file_block("files/Test_createNoplace_file_correct",block, i);
	}
	free(block);
	
	create_hashdata("files/Test_createNoplace_hash_correct");
	hash_block = (char *)calloc(16, 1);
	for(int i = 0; i < 1; i++) {
		add_hash_block("files/Test_createNoplace_hash_correct", hash_block, i);
	}
	free(hash_block);
	if(compare_filedata("files/Test_createNoplace_file", "files/Test_createNoplace_file_correct") || compare_directory("files/Test_createNoplace_dir", "files/Test_createNoplace_dir_correct") || compare_hash("files/Test_createNoplace_hash", "files/Test_createNoplace_hash_correct")) {
		return 1;
	}
	if(result == 2) {
		return 0;
	}
	return result;
}

int resizeFile() {
	/* 
		This function tests resize a normal file without special condition.
	*/
	create_directory("files/Test_resizeFile_dir");
	add_file("files/Test_resizeFile_dir", "1.doc", 10, 0, 0);
	add_file("files/Test_resizeFile_dir", "2.doc", 10, 150, 1);
	add_file("files/Test_resizeFile_dir", "3.doc", 10, 180, 2);
	add_file("files/Test_resizeFile_dir", "\0", 0, 0, 3);
	
	
	create_Filedata("files/Test_resizeFile_file");
	char *block = (char *)calloc(256, 1);
	for(int i = 0; i < 1; i++) {
		add_file_block("files/Test_resizeFile_file",block, i);
	}
	free(block);
	
	create_hashdata("files/Test_resizeFile_hash");
	char *hash_block = (char *)calloc(16, 1);
	for(int i = 0; i < 1; i++) {
		add_hash_block("files/Test_resizeFile_hash", hash_block, i);
	}
	free(hash_block);
	
	// init file system
	void *helper = init_fs("files/Test_resizeFile_file", "files/Test_resizeFile_dir", "files/Test_resizeFile_hash", 4);
	int result = resize_file("1.doc", 20, helper);
	close_fs(helper);
	
	create_directory("files/Test_resizeFile_dir_correct");
	// file should be repacked to find whether it has enough space
	add_file("files/Test_resizeFile_dir_correct", "1.doc", 20, 0, 0);
	add_file("files/Test_resizeFile_dir_correct", "2.doc", 10, 150, 1);
	add_file("files/Test_resizeFile_dir_correct", "3.doc", 10, 180, 2);
	add_file("files/Test_resizeFile_dir_correct", "\0", 0, 0, 3);
	
	create_Filedata("files/Test_resizeFile_file_correct");
	block = (char *)calloc(256, 1);
	for(int i = 0; i < 1; i++) {
		add_file_block("files/Test_resizeFile_file_correct",block, i);
	}
	free(block);
	
	create_hashdata("files/Test_resizeFile_hash_correct");
	hash_block = (char *)calloc(16, 1);
	for(int i = 0; i < 1; i++) {
		add_hash_block("files/Test_resizeFile_hash_correct", hash_block, i);
	}
	free(hash_block);
	if(compare_filedata("files/Test_resizeFile_file", "files/Test_resizeFile_file_correct") || compare_directory("files/Test_resizeFile_dir", "files/Test_resizeFile_dir_correct") || compare_hash("files/Test_resizeFile_hash", "files/Test_resizeFile_hash_correct")) {
		return 1;
	}
	if(result == 0) {
		return 0;
	}
	return result;
}

int resizeNotExist() {
	/* 
		This function tests resize a file which does not exist.
		Function resize_file should return 1.
	*/
	create_directory("files/Test_resizeNotExist_dir");
	add_file("files/Test_resizeNotExist_dir", "1.doc", 10, 0, 0);
	add_file("files/Test_resizeNotExist_dir", "2.doc", 10, 150, 1);
	add_file("files/Test_resizeNotExist_dir", "3.doc", 10, 180, 2);
	add_file("files/Test_resizeNotExist_dir", "\0", 0, 0, 3);
	
	
	create_Filedata("files/Test_resizeNotExist_file");
	char *block = (char *)calloc(256, 1);
	for(int i = 0; i < 1; i++) {
		add_file_block("files/Test_resizeNotExist_file",block, i);
	}
	free(block);
	
	create_hashdata("files/Test_resizeNotExist_hash");
	char *hash_block = (char *)calloc(16, 1);
	for(int i = 0; i < 1; i++) {
		add_hash_block("files/Test_resizeNotExist_hash", hash_block, i);
	}
	free(hash_block);
	
	// init file system
	void *helper = init_fs("files/Test_resizeNotExist_file", "files/Test_resizeNotExist_dir", "files/Test_resizeNotExist_hash", 4);
	int result = resize_file("5.doc", 20, helper);
	close_fs(helper);
	
	create_directory("files/Test_resizeNotExist_dir_correct");
	// file should be repacked to find whether it has enough space
	add_file("files/Test_resizeNotExist_dir_correct", "1.doc", 10, 0, 0);
	add_file("files/Test_resizeNotExist_dir_correct", "2.doc", 10, 150, 1);
	add_file("files/Test_resizeNotExist_dir_correct", "3.doc", 10, 180, 2);
	add_file("files/Test_resizeNotExist_dir_correct", "\0", 0, 0, 3);
	
	create_Filedata("files/Test_resizeNotExist_file_correct");
	block = (char *)calloc(256, 1);
	for(int i = 0; i < 1; i++) {
		add_file_block("files/Test_resizeNotExist_file_correct",block, i);
	}
	free(block);
	
	create_hashdata("files/Test_resizeNotExist_hash_correct");
	hash_block = (char *)calloc(16, 1);
	for(int i = 0; i < 1; i++) {
		add_hash_block("files/Test_resizeNotExist_hash_correct", hash_block, i);
	}
	free(hash_block);
	if(compare_filedata("files/Test_resizeNotExist_file", "files/Test_resizeNotExist_file_correct") || compare_directory("files/Test_resizeNotExist_dir", "files/Test_resizeNotExist_dir_correct") || compare_hash("files/Test_resizeNotExist_hash", "files/Test_resizeNotExist_hash_correct")) {
		return 1;
	}
	if(result == 1) {
		return 0;
	}
	return result;
}

int resizeWithRepack() {
	/* 
		This function tests resize a file which needs to repack file_data.
	*/
	create_directory("files/Test_resizeWithRepack_dir");
	add_file("files/Test_resizeWithRepack_dir", "1.doc", 10, 0, 0);
	add_file("files/Test_resizeWithRepack_dir", "2.doc", 10, 150, 1);
	add_file("files/Test_resizeWithRepack_dir", "3.doc", 10, 180, 2);
	add_file("files/Test_resizeWithRepack_dir", "\0", 0, 0, 3);
	
	
	create_Filedata("files/Test_resizeWithRepack_file");
	char *block = (char *)calloc(256, 1);
	for(int i = 0; i < 1; i++) {
		add_file_block("files/Test_resizeWithRepack_file",block, i);
	}
	free(block);
	
	create_hashdata("files/Test_resizeWithRepack_hash");
	char *hash_block = (char *)calloc(16, 1);
	for(int i = 0; i < 1; i++) {
		add_hash_block("files/Test_resizeWithRepack_hash", hash_block, i);
	}
	free(hash_block);
	
	// init file system
	void *helper = init_fs("files/Test_resizeWithRepack_file", "files/Test_resizeWithRepack_dir", "files/Test_resizeWithRepack_hash", 4);
	int result = resize_file("3.doc", 200, helper);
	close_fs(helper);
	
	create_directory("files/Test_resizeWithRepack_dir_correct");
	// file should be repacked to find whether it has enough space
    add_file("files/Test_resizeWithRepack_dir_correct", "1.doc", 10, 0, 0);
	add_file("files/Test_resizeWithRepack_dir_correct", "2.doc", 10, 10, 1);
	add_file("files/Test_resizeWithRepack_dir_correct", "3.doc", 200, 20, 2);
	add_file("files/Test_resizeWithRepack_dir_correct", "\0", 0, 0, 3);
	
	create_Filedata("files/Test_resizeWithRepack_file_correct");
	block = (char *)calloc(256, 1);
	for(int i = 0; i < 1; i++) {
		add_file_block("files/Test_resizeWithRepack_file_correct",block, i);
	}
	free(block);
	
	create_hashdata("files/Test_resizeWithRepack_hash_correct");
	hash_block = (char *)calloc(16, 1);
	for(int i = 0; i < 1; i++) {
		add_hash_block("files/Test_resizeWithRepack_hash_correct", hash_block, i);
	}
	free(hash_block);
	if(compare_filedata("files/Test_resizeWithRepack_file", "files/Test_resizeWithRepack_file_correct") || compare_directory("files/Test_resizeWithRepack_dir", "files/Test_resizeWithRepack_dir_correct") || compare_hash("files/Test_resizeWithRepack_hash", "files/Test_resizeWithRepack_hash_correct")) {
		return 1;
	}
	if(result == 0) {
		return 0;
	}
	return result;
}

int resizeNoplace() {
	/* 
		This function tests resize a file which is too big to resize.
		Function resize_file should return 2.
	*/
	create_directory("files/Test_resizeNoplace_dir");
	add_file("files/Test_resizeNoplace_dir", "1.doc", 10, 0, 0);
	add_file("files/Test_resizeNoplace_dir", "2.doc", 20, 150, 1);
	add_file("files/Test_resizeNoplace_dir", "3.doc", 10, 180, 2);
	add_file("files/Test_resizeNoplace_dir", "\0", 0, 0, 3);
	
	
	create_Filedata("files/Test_resizeNoplace_file");
	char *block = (char *)calloc(256, 1);
	for(int i = 0; i < 1; i++) {
		add_file_block("files/Test_resizeNoplace_file",block, i);
	}
	free(block);
	
	create_hashdata("files/Test_resizeNoplace_hash");
	char *hash_block = (char *)calloc(16, 1);
	for(int i = 0; i < 1; i++) {
		add_hash_block("files/Test_resizeNoplace_hash", hash_block, i);
	}
	free(hash_block);
	
	// init file system
	void *helper = init_fs("files/Test_resizeNoplace_file", "files/Test_resizeNoplace_dir", "files/Test_resizeNoplace_hash", 4);
	int result = resize_file("3.doc", 227, helper);
	close_fs(helper);
	
	create_directory("files/Test_resizeNoplace_dir_correct");
	// file should be repacked to find whether it has enough space
    add_file("files/Test_resizeNoplace_dir_correct", "1.doc", 10, 0, 0);
	add_file("files/Test_resizeNoplace_dir_correct", "2.doc", 20, 10, 1);
	add_file("files/Test_resizeNoplace_dir_correct", "3.doc", 10, 30, 2);
	add_file("files/Test_resizeNoplace_dir_correct", "\0", 0, 0, 3);
	
	create_Filedata("files/Test_resizeNoplace_file_correct");
	block = (char *)calloc(256, 1);
	for(int i = 0; i < 1; i++) {
		add_file_block("files/Test_resizeNoplace_file_correct",block, i);
	}
	free(block);
	
	create_hashdata("files/Test_resizeNoplace_hash_correct");
	hash_block = (char *)calloc(16, 1);
	for(int i = 0; i < 1; i++) {
		add_hash_block("files/Test_resizeNoplace_hash_correct", hash_block, i);
	}
	free(hash_block);
	if(compare_filedata("files/Test_resizeNoplace_file", "files/Test_resizeNoplace_file_correct") || compare_directory("files/Test_resizeNoplace_dir", "files/Test_resizeNoplace_dir_correct") || compare_hash("files/Test_resizeNoplace_hash", "files/Test_resizeNoplace_hash_correct")) {
		return 1;
	}
	if(result == 2) {
		return 0;
	}
	return result;
}


int repackTest() {
	/* 
		This function tests resize a file which is too big to resize.
	*/
	create_directory("files/Test_repackTest_dir");
	add_file("files/Test_repackTest_dir", "1.doc", 10, 0, 0);
	add_file("files/Test_repackTest_dir", "2.doc", 10, 150, 1);
	add_file("files/Test_repackTest_dir", "3.doc", 10, 180, 2);
	add_file("files/Test_repackTest_dir", "\0", 0, 0, 3);
	
	
	create_Filedata("files/Test_repackTest_file");
	char *block = (char *)calloc(256, 1);
	for(int i = 0; i < 1; i++) {
		add_file_block("files/Test_repackTest_file",block, i);
	}
	free(block);
	
	create_hashdata("files/Test_repackTest_hash");
	char *hash_block = (char *)calloc(16, 1);
	for(int i = 0; i < 1; i++) {
		add_hash_block("files/Test_repackTest_hash", hash_block, i);
	}
	free(hash_block);
	
	// init file system
	void *helper = init_fs("files/Test_repackTest_file", "files/Test_repackTest_dir", "files/Test_repackTest_hash", 4);
	repack(helper);
	close_fs(helper);
	
	create_directory("files/Test_repackTest_dir_correct");
    add_file("files/Test_repackTest_dir_correct", "1.doc", 10, 0, 0);
	add_file("files/Test_repackTest_dir_correct", "2.doc", 10, 10, 1);
	add_file("files/Test_repackTest_dir_correct", "3.doc", 10, 20, 2);
	add_file("files/Test_repackTest_dir_correct", "\0", 0, 0, 3);
	
	create_Filedata("files/Test_repackTest_file_correct");
	block = (char *)calloc(256, 1);
	for(int i = 0; i < 1; i++) {
		add_file_block("files/Test_repackTest_file_correct",block, i);
	}
	free(block);
	
	create_hashdata("files/Test_repackTest_hash_correct");
	hash_block = (char *)calloc(16, 1);
	for(int i = 0; i < 1; i++) {
		add_hash_block("files/Test_repackTest_hash_correct", hash_block, i);
	}
	free(hash_block);
	if(compare_filedata("files/Test_repackTest_file", "files/Test_repackTest_file_correct") || compare_directory("files/Test_repackTest_dir", "files/Test_repackTest_dir_correct") || compare_hash("files/Test_repackTest_hash", "files/Test_repackTest_hash_correct")) {
		return 1;
	}
	return 0;
}

int repackDirOutOfOrder() {
	/* 
		This function tests repack file_data with a directory_table out of order.
	*/
	create_directory("files/Test_repackDirOutOfOrder_dir");
    add_file("files/Test_repackDirOutOfOrder_dir", "2.doc", 10, 150, 1);
	add_file("files/Test_repackDirOutOfOrder_dir", "1.doc", 10, 0, 0);
	add_file("files/Test_repackDirOutOfOrder_dir", "3.doc", 10, 180, 2);
	add_file("files/Test_repackDirOutOfOrder_dir", "\0", 0, 0, 3);
	
	
	create_Filedata("files/Test_repackDirOutOfOrder_file");
	char *block = (char *)calloc(256, 1);
	for(int i = 0; i < 1; i++) {
		add_file_block("files/Test_repackDirOutOfOrder_file",block, i);
	}
	free(block);
	
	create_hashdata("files/Test_repackDirOutOfOrder_hash");
	char *hash_block = (char *)calloc(16, 1);
	for(int i = 0; i < 1; i++) {
		add_hash_block("files/Test_repackDirOutOfOrder_hash", hash_block, i);
	}
	free(hash_block);
	
	// init file system
	void *helper = init_fs("files/Test_repackDirOutOfOrder_file", "files/Test_repackDirOutOfOrder_dir", "files/Test_repackDirOutOfOrder_hash", 4);
	repack(helper);
	close_fs(helper);
	
	create_directory("files/Test_repackDirOutOfOrder_dir_correct");
	// file should be repacked to find whether it has enough space
    add_file("files/Test_repackDirOutOfOrder_dir_correct", "2.doc", 10, 10, 1);
    add_file("files/Test_repackDirOutOfOrder_dir_correct", "1.doc", 10, 0, 0);
	add_file("files/Test_repackDirOutOfOrder_dir_correct", "3.doc", 10, 20, 2);
	add_file("files/Test_repackDirOutOfOrder_dir_correct", "\0", 0, 0, 3);
	
	create_Filedata("files/Test_repackDirOutOfOrder_file_correct");
	block = (char *)calloc(256, 1);
	for(int i = 0; i < 1; i++) {
		add_file_block("files/Test_repackDirOutOfOrder_file_correct",block, i);
	}
	free(block);
	
	create_hashdata("files/Test_repackDirOutOfOrder_hash_correct");
	hash_block = (char *)calloc(16, 1);
	for(int i = 0; i < 1; i++) {
		add_hash_block("files/Test_repackDirOutOfOrder_hash_correct", hash_block, i);
	}
	free(hash_block);
	if(compare_filedata("files/Test_repackDirOutOfOrder_file", "files/Test_repackDirOutOfOrder_file_correct") || compare_directory("files/Test_repackDirOutOfOrder_dir", "files/Test_repackDirOutOfOrder_dir_correct") || compare_hash("files/Test_repackDirOutOfOrder_hash", "files/Test_repackDirOutOfOrder_hash_correct")) {
		return 1;
	}
	return 0;
}

int deletefileTest() {
	/* 
		This function tests delete file.
	*/
	create_directory("files/Test_deletefileTest_dir");
	add_file("files/Test_deletefileTest_dir", "1.doc", 10, 0, 0);
    add_file("files/Test_deletefileTest_dir", "2.doc", 10, 150, 1);
	add_file("files/Test_deletefileTest_dir", "3.doc", 10, 180, 2);
	add_file("files/Test_deletefileTest_dir", "\0", 0, 0, 3);
	
	
	create_Filedata("files/Test_deletefileTest_file");
	char *block = (char *)calloc(256, 1);
	for(int i = 0; i < 1; i++) {
		add_file_block("files/Test_deletefileTest_file",block, i);
	}
	free(block);
	
	create_hashdata("files/Test_deletefileTest_hash");
	char *hash_block = (char *)calloc(16, 1);
	for(int i = 0; i < 1; i++) {
		add_hash_block("files/Test_deletefileTest_hash", hash_block, i);
	}
	free(hash_block);
	
	// init file system
	void *helper = init_fs("files/Test_deletefileTest_file", "files/Test_deletefileTest_dir", "files/Test_deletefileTest_hash", 4);
	int result = delete_file("3.doc", helper);
	close_fs(helper);
	
	create_directory("files/Test_deletefileTest_dir_correct");
    add_file("files/Test_deletefileTest_dir_correct", "1.doc", 10, 0, 0);
    add_file("files/Test_deletefileTest_dir_correct", "2.doc", 10, 150, 1);
	add_file("files/Test_deletefileTest_dir_correct", "\0", 0, 0, 2);
	add_file("files/Test_deletefileTest_dir_correct", "\0", 0, 0, 3);
	
	create_Filedata("files/Test_deletefileTest_file_correct");
	block = (char *)calloc(256, 1);
	for(int i = 0; i < 1; i++) {
		add_file_block("files/Test_deletefileTest_file_correct",block, i);
	}
	free(block);
	
	create_hashdata("files/Test_deletefileTest_hash_correct");
	hash_block = (char *)calloc(16, 1);
	for(int i = 0; i < 1; i++) {
		add_hash_block("files/Test_deletefileTest_hash_correct", hash_block, i);
	}
	free(hash_block);
	if(compare_filedata("files/Test_deletefileTest_file", "files/Test_deletefileTest_file_correct") || compare_directory("files/Test_deletefileTest_dir", "files/Test_deletefileTest_dir_correct") || compare_hash("files/Test_deletefileTest_hash", "files/Test_deletefileTest_hash_correct")) {
		return 1;
	}
    if(result == 0) {
        return 0;
    }
	return result;
}


int deletefileNotExists() {
	/*
		This function tests delete the file which is not existed.
        Function delete_file should return 1.
	*/
	create_directory("files/Test_deletefileNotExists_dir");
	add_file("files/Test_deletefileNotExists_dir", "1.doc", 10, 0, 0);
    add_file("files/Test_deletefileNotExists_dir", "2.doc", 10, 150, 1);
	add_file("files/Test_deletefileNotExists_dir", "3.doc", 10, 180, 2);
	add_file("files/Test_deletefileNotExists_dir", "\0", 0, 0, 3);


	create_Filedata("files/Test_deletefileNotExists_file");
	char *block = (char *)calloc(256, 1);
	for(int i = 0; i < 1; i++) {
		add_file_block("files/Test_deletefileNotExists_file",block, i);
	}
	free(block);

	create_hashdata("files/Test_deletefileNotExists_hash");
	char *hash_block = (char *)calloc(16, 1);
	for(int i = 0; i < 1; i++) {
		add_hash_block("files/Test_deletefileNotExists_hash", hash_block, i);
	}
	free(hash_block);

	// init file system
	void *helper = init_fs("files/Test_deletefileNotExists_file", "files/Test_deletefileNotExists_dir", "files/Test_deletefileNotExists_hash", 4);
	int result = delete_file("4.doc", helper);
	close_fs(helper);

	create_directory("files/Test_deletefileNotExists_dir_correct");
    add_file("files/Test_deletefileNotExists_dir_correct", "1.doc", 10, 0, 0);
    add_file("files/Test_deletefileNotExists_dir_correct", "2.doc", 10, 150, 1);
	add_file("files/Test_deletefileNotExists_dir_correct", "3.doc", 10, 180, 2);
	add_file("files/Test_deletefileNotExists_dir_correct", "\0", 0, 0, 3);

	create_Filedata("files/Test_deletefileNotExists_file_correct");
	block = (char *)calloc(256, 1);
	for(int i = 0; i < 1; i++) {
		add_file_block("files/Test_deletefileNotExists_file_correct",block, i);
	}
	free(block);

	create_hashdata("files/Test_deletefileNotExists_hash_correct");
	hash_block = (char *)calloc(16, 1);
	for(int i = 0; i < 1; i++) {
		add_hash_block("files/Test_deletefileNotExists_hash_correct", hash_block, i);
	}
	free(hash_block);
	if(compare_filedata("files/Test_deletefileNotExists_file", "files/Test_deletefileNotExists_file_correct") || compare_directory("files/Test_deletefileNotExists_dir", "files/Test_deletefileNotExists_dir_correct") || compare_hash("files/Test_deletefileNotExists_hash", "files/Test_deletefileNotExists_hash_correct")) {
		return 1;
	}
    if(result == 1) {
        return 0;
    }
	return result;
}

int renameTest() {
	/*
		This function tests rename file.
	*/
	create_directory("files/Test_renameTest_dir");
	add_file("files/Test_renameTest_dir", "1.doc", 10, 0, 0);
    add_file("files/Test_renameTest_dir", "2.doc", 10, 150, 1);
	add_file("files/Test_renameTest_dir", "3.doc", 10, 180, 2);
	add_file("files/Test_renameTest_dir", "\0", 0, 0, 3);


	create_Filedata("files/Test_renameTest_file");
	char *block = (char *)calloc(256, 1);
	for(int i = 0; i < 1; i++) {
		add_file_block("files/Test_renameTest_file",block, i);
	}
	free(block);

	create_hashdata("files/Test_renameTest_hash");
	char *hash_block = (char *)calloc(16, 1);
	for(int i = 0; i < 1; i++) {
		add_hash_block("files/Test_renameTest_hash", hash_block, i);
	}
	free(hash_block);

	// init file system
	void *helper = init_fs("files/Test_renameTest_file", "files/Test_renameTest_dir", "files/Test_renameTest_hash", 4);
	int result = rename_file("3.doc", "4.doc", helper);
	close_fs(helper);

	create_directory("files/Test_renameTest_dir_correct");
    add_file("files/Test_renameTest_dir_correct", "1.doc", 10, 0, 0);
    add_file("files/Test_renameTest_dir_correct", "2.doc", 10, 150, 1);
	add_file("files/Test_renameTest_dir_correct", "4.doc", 10, 180, 2);
	add_file("files/Test_renameTest_dir_correct", "\0", 0, 0, 3);

	create_Filedata("files/Test_renameTest_file_correct");
	block = (char *)calloc(256, 1);
	for(int i = 0; i < 1; i++) {
		add_file_block("files/Test_renameTest_file_correct",block, i);
	}
	free(block);

	create_hashdata("files/Test_renameTest_hash_correct");
	hash_block = (char *)calloc(16, 1);
	for(int i = 0; i < 1; i++) {
		add_hash_block("files/Test_renameTest_hash_correct", hash_block, i);
	}
	free(hash_block);
	if(compare_filedata("files/Test_renameTest_file", "files/Test_renameTest_file_correct") || compare_directory("files/Test_renameTest_dir", "files/Test_renameTest_dir_correct") || compare_hash("files/Test_renameTest_hash", "files/Test_renameTest_hash_correct")) {
		return 1;
	}
    if(result == 0) {
        return 0;
    }
	return result;
}

int renameNotExists() {
	/*
		This function tests rename file that does not existed.
        Function rename_file should return 1.
	*/
	create_directory("files/Test_renameNotExists_dir");
	add_file("files/Test_renameNotExists_dir", "1.doc", 10, 0, 0);
    add_file("files/Test_renameNotExists_dir", "2.doc", 10, 150, 1);
	add_file("files/Test_renameNotExists_dir", "3.doc", 10, 180, 2);
	add_file("files/Test_renameNotExists_dir", "\0", 0, 0, 3);


	create_Filedata("files/Test_renameNotExists_file");
	char *block = (char *)calloc(256, 1);
	for(int i = 0; i < 1; i++) {
		add_file_block("files/Test_renameNotExists_file",block, i);
	}
	free(block);

	create_hashdata("files/Test_renameNotExists_hash");
	char *hash_block = (char *)calloc(16, 1);
	for(int i = 0; i < 1; i++) {
		add_hash_block("files/Test_renameNotExists_hash", hash_block, i);
	}
	free(hash_block);

	// init file system
	void *helper = init_fs("files/Test_renameNotExists_file", "files/Test_renameNotExists_dir", "files/Test_renameNotExists_hash", 4);
	int result = rename_file("4.doc", "5.doc", helper);
	close_fs(helper);

	create_directory("files/Test_renameNotExists_dir_correct");
    add_file("files/Test_renameNotExists_dir_correct", "1.doc", 10, 0, 0);
    add_file("files/Test_renameNotExists_dir_correct", "2.doc", 10, 150, 1);
	add_file("files/Test_renameNotExists_dir_correct", "3.doc", 10, 180, 2);
	add_file("files/Test_renameNotExists_dir_correct", "\0", 0, 0, 3);

	create_Filedata("files/Test_renameNotExists_file_correct");
	block = (char *)calloc(256, 1);
	for(int i = 0; i < 1; i++) {
		add_file_block("files/Test_renameNotExists_file_correct",block, i);
	}
	free(block);

	create_hashdata("files/Test_renameNotExists_hash_correct");
	hash_block = (char *)calloc(16, 1);
	for(int i = 0; i < 1; i++) {
		add_hash_block("files/Test_renameNotExists_hash_correct", hash_block, i);
	}
	free(hash_block);
	if(compare_filedata("files/Test_renameNotExists_file", "files/Test_renameNotExists_file_correct") || compare_directory("files/Test_renameNotExists_dir", "files/Test_renameNotExists_dir_correct") || compare_hash("files/Test_renameNotExists_hash", "files/Test_renameNotExists_hash_correct")) {
		return 1;
	}
    if(result == 1) {
        return 0;
    }
	return result;
}

int readTest() {
	/* 
		This function tests read file.
	*/
	create_directory("files/readTest_dir");
	add_file("files/readTest_dir", "2.doc", 10, 150, 0);
	add_file("files/readTest_dir", "3.doc", 10, 180, 1);
	add_file("files/readTest_dir", "1.doc", 10, 0, 2);
	add_file("files/readTest_dir", "\0", 0, 0, 3);
	
	
	create_Filedata("files/readTest_file");
	char *block = (char *)calloc(256, 1);
	for(int i = 0; i < 1; i++) {
		add_file_block("files/readTest_file",block, i);
	}
	free(block);
	
	create_hashdata("files/readTest_hash");
	char *hash_block = (char *)calloc(16, 1);
	for(int i = 0; i < 1; i++) {
		add_hash_block("files/readTest_hash", hash_block, i);
	}
	free(hash_block);
	
	// init file system
	void *helper = init_fs("files/readTest_file", "files/readTest_dir", "files/readTest_hash", 4);
	char *buf = "hello!";
	compute_hash_tree(helper);
	write_file("1.doc", 0, strlen(buf) + 1, buf, helper);
	char check[10];
	int result = read_file("1.doc", 0, strlen(buf) + 1, check, helper);
	close_fs(helper);
	if(result) {
		display_hash("files/readTest_hash");
		return result;
	}
	if(strcmp(buf, check)) {
		printf("%s\n", check);
		return 1;
	}
  
	return 0;
}

int readNotExists() {
	/* 
		This function tests read the file tht does not exist.
	*/
	create_directory("files/readNotExists_dir");
	add_file("files/readNotExists_dir", "2.doc", 10, 150, 0);
	add_file("files/readNotExists_dir", "3.doc", 10, 180, 1);
	add_file("files/readNotExists_dir", "1.doc", 10, 0, 2);
	add_file("files/readNotExists_dir", "\0", 0, 0, 3);
	
	
	create_Filedata("files/readNotExists_file");
	char *block = (char *)calloc(256, 1);
	for(int i = 0; i < 1; i++) {
		add_file_block("files/readNotExists_file",block, i);
	}
	free(block);
	
	create_hashdata("files/readNotExists_hash");
	char *hash_block = (char *)calloc(16, 1);
	for(int i = 0; i < 1; i++) {
		add_hash_block("files/readNotExists_hash", hash_block, i);
	}
	free(hash_block);
	
	// init file system
	void *helper = init_fs("files/readNotExists_file", "files/readNotExists_dir", "files/readNotExists_hash", 4);
	char *buf = "hello!";
	compute_hash_tree(helper);
	write_file("4.doc", 0, strlen(buf) + 1, buf, helper);
	char check[10];
	int result = read_file("1.doc", 0, strlen(buf) + 1, check, helper);
	close_fs(helper);
	if(result != 1) {
		return result;
	}
  
	return 0;
}

int readInvalidOffset() {
	/* 
		This function tests read file with invalid offset.
	*/
	create_directory("files/readInvalidOffset_dir");
	add_file("files/readInvalidOffset_dir", "2.doc", 10, 150, 0);
	add_file("files/readInvalidOffset_dir", "3.doc", 10, 180, 1);
	add_file("files/readInvalidOffset_dir", "1.doc", 10, 0, 2);
	add_file("files/readInvalidOffset_dir", "\0", 0, 0, 3);
	
	
	create_Filedata("files/readInvalidOffset_file");
	char *block = (char *)calloc(256, 1);
	for(int i = 0; i < 1; i++) {
		add_file_block("files/readInvalidOffset_file",block, i);
	}
	free(block);
	
	create_hashdata("files/readInvalidOffset_hash");
	char *hash_block = (char *)calloc(16, 1);
	for(int i = 0; i < 1; i++) {
		add_hash_block("files/readInvalidOffset_hash", hash_block, i);
	}
	free(hash_block);
	
	// init file system
	void *helper = init_fs("files/readInvalidOffset_file", "files/readInvalidOffset_dir", "files/readInvalidOffset_hash", 4);
	char *buf = "hello!";
	compute_hash_tree(helper);
	write_file("1.doc", 0, strlen(buf) + 1, buf, helper);
	char check[10];
	int result = read_file("1.doc", 11, strlen(buf) + 1, check, helper);
	close_fs(helper);
	if(result != 2) {
		return result;
	}
  
	return 0;
}

int readWithWrongHashValue() {
	/* 
		This function tests read file with wrong hash_data.
	*/
	create_directory("files/readWithWrongHashValue_dir");
	add_file("files/readWithWrongHashValue_dir", "2.doc", 10, 150, 0);
	add_file("files/readWithWrongHashValue_dir", "3.doc", 10, 180, 1);
	add_file("files/readWithWrongHashValue_dir", "1.doc", 10, 0, 2);
	add_file("files/readWithWrongHashValue_dir", "\0", 0, 0, 3);
	
	
	create_Filedata("files/readWithWrongHashValue_file");
	char *block = (char *)calloc(256, 1);
	for(int i = 0; i < 1; i++) {
		add_file_block("files/readWithWrongHashValue_file",block, i);
	}
	free(block);
	
	create_hashdata("files/readWithWrongHashValue_hash");
	char *hash_block = (char *)calloc(16, 1);
	for(int i = 0; i < 1; i++) {
		add_hash_block("files/readWithWrongHashValue_hash", hash_block, i);
	}
	
	
	// init file system
	void *helper = init_fs("files/readWithWrongHashValue_file", "files/readWithWrongHashValue_dir", "files/readWithWrongHashValue_hash", 4);
	char *buf = "hello!";
	write_file("1.doc", 0, strlen(buf) + 1, buf, helper);
	char check[10];
	// make the hash_data wrong
	add_hash_block("files/readWithWrongHashValue_hash", hash_block, 0);
	free(hash_block);
	int result = read_file("1.doc", 0, strlen(buf) + 1, check, helper);
	close_fs(helper);
	if(result != 3) {
		printf("result: %d\n", result);
        display_hash("files/readWithWrongHashValue_hash");
		return result;
	}
  	
	return 0;
}

int writeTest() {
	/* 
		This function tests write file.
	*/
	create_directory("files/Test_writeTest_dir");
	add_file("files/Test_writeTest_dir", "1.doc", 10, 0, 0);
    add_file("files/Test_writeTest_dir", "2.doc", 10, 150, 1);
	add_file("files/Test_writeTest_dir", "3.doc", 10, 180, 2);
	add_file("files/Test_writeTest_dir", "\0", 0, 0, 3);
	
	
	create_Filedata("files/Test_writeTest_file");
	char *block = (char *)calloc(256, 1);
	for(int i = 0; i < 1; i++) {
		add_file_block("files/Test_writeTest_file",block, i);
	}
	free(block);
	
	create_hashdata("files/Test_writeTest_hash");
	char *hash_block = (char *)calloc(16, 1);
	for(int i = 0; i < 1; i++) {
		add_hash_block("files/Test_writeTest_hash", hash_block, i);
	}
	free(hash_block);
	
	// init file system
	void *helper = init_fs("files/Test_writeTest_file", "files/Test_writeTest_dir", "files/Test_writeTest_hash", 4);
    char *buf = "hello!";
	compute_hash_tree(helper);
	int result = write_file("1.doc", 0, strlen(buf) + 1, buf, helper);
	close_fs(helper);
	
	create_directory("files/Test_writeTest_dir_correct");
    add_file("files/Test_writeTest_dir_correct", "1.doc", 10, 0, 0);
    add_file("files/Test_writeTest_dir_correct", "2.doc", 10, 150, 1);
	add_file("files/Test_writeTest_dir_correct", "3.doc", 10, 180, 2);
	add_file("files/Test_writeTest_dir_correct", "\0", 0, 0, 3);
	
	create_Filedata("files/Test_writeTest_file_correct");
	block = (char *)calloc(256, 1);
	for(int i = 0; i < 1; i++) {
		add_file_block("files/Test_writeTest_file_correct",block, i);
	}
	int fd = open("files/Test_writeTest_file_correct", O_WRONLY);
    write(fd, buf, strlen(buf) + 1);
    char hash_data[16];
    memcpy(block, buf, strlen(buf) + 1);
    fletcher((uint8_t *)block, 256, (uint8_t *)hash_data);
    close(fd);
    free(block);
	create_hashdata("files/Test_writeTest_hash_correct");
	hash_block = (char *)calloc(16, 1);
	for(int i = 0; i < 1; i++) {
		add_hash_block("files/Test_writeTest_hash_correct", hash_block, i);
	}
    int hd = open("files/Test_writeTest_hash_correct", O_WRONLY);
    write(hd, hash_data, 16);
    close(hd);
	free(hash_block);
    
	if(compare_filedata("files/Test_writeTest_file", "files/Test_writeTest_file_correct") || compare_directory("files/Test_writeTest_dir", "files/Test_writeTest_dir_correct") || compare_hash("files/Test_writeTest_hash", "files/Test_writeTest_hash_correct")) {
		return 1;
	}
    if(result == 0) {
        return 0;
    }
	return result;
}

int writeNotExists() {
	/* 
		This function tests write file which does not exist.
	*/
	create_directory("files/Test_writeNotExists_dir");
	add_file("files/Test_writeNotExists_dir", "1.doc", 10, 0, 0);
    add_file("files/Test_writeNotExists_dir", "2.doc", 10, 150, 1);
	add_file("files/Test_writeNotExists_dir", "3.doc", 10, 180, 2);
	add_file("files/Test_writeNotExists_dir", "\0", 0, 0, 3);
	
	
	create_Filedata("files/Test_writeNotExists_file");
	char *block = (char *)calloc(256, 1);
	for(int i = 0; i < 1; i++) {
		add_file_block("files/Test_writeNotExists_file",block, i);
	}
	free(block);
	
	create_hashdata("files/Test_writeNotExists_hash");
	char *hash_block = (char *)calloc(16, 1);
	for(int i = 0; i < 1; i++) {
		add_hash_block("files/Test_writeNotExists_hash", hash_block, i);
	}
	free(hash_block);
	
	// init file system
	void *helper = init_fs("files/Test_writeNotExists_file", "files/Test_writeNotExists_dir", "files/Test_writeNotExists_hash", 4);
    char *buf = "hello!";
	compute_hash_tree(helper);
	int result = write_file("4.doc", 0, strlen(buf) + 1, buf, helper);
	close_fs(helper);
	
	create_directory("files/Test_writeNotExists_dir_correct");
    add_file("files/Test_writeNotExists_dir_correct", "1.doc", 10, 0, 0);
    add_file("files/Test_writeNotExists_dir_correct", "2.doc", 10, 150, 1);
	add_file("files/Test_writeNotExists_dir_correct", "3.doc", 10, 180, 2);
	add_file("files/Test_writeNotExists_dir_correct", "\0", 0, 0, 3);
	
	create_Filedata("files/Test_writeNotExists_file_correct");
	block = (char *)calloc(256, 1);
	for(int i = 0; i < 1; i++) {
		add_file_block("files/Test_writeNotExists_file_correct",block, i);
	}
    free(block);
	create_hashdata("files/Test_writeNotExists_hash_correct");
	hash_block = (char *)calloc(16, 1);
	for(int i = 0; i < 1; i++) {
		add_hash_block("files/Test_writeNotExists_hash_correct", hash_block, i);
	}
	free(hash_block);
    
	if(compare_filedata("files/Test_writeNotExists_file", "files/Test_writeNotExists_file_correct") || compare_directory("files/Test_writeNotExists_dir", "files/Test_writeNotExists_dir_correct") || compare_hash("files/Test_writeNotExists_hash", "files/Test_writeNotExists_hash_correct")) {
		return 1;
	}
    if(result == 1) {
        return 0;
    }
	return result;
}

int writeRenewHash() {
	/* 
		This function tests write a file and renew the hash_data at the same time.
	*/
	create_directory("files/Test_writeRenewHash_dir");
	add_file("files/Test_writeRenewHash_dir", "1.doc", 10, 0, 0);
    add_file("files/Test_writeRenewHash_dir", "2.doc", 10, 150, 1);
	add_file("files/Test_writeRenewHash_dir", "3.doc", 10, 180, 2);
	add_file("files/Test_writeRenewHash_dir", "\0", 0, 0, 3);
	
	
	create_Filedata("files/Test_writeRenewHash_file");
	char *block = (char *)calloc(256, 1);
	for(int i = 0; i < 1; i++) {
		add_file_block("files/Test_writeRenewHash_file",block, i);
	}
	free(block);
	
	create_hashdata("files/Test_writeRenewHash_hash");
	char *hash_block = (char *)calloc(16, 1);
	for(int i = 0; i < 1; i++) {
		add_hash_block("files/Test_writeRenewHash_hash", hash_block, i);
	}
	free(hash_block);
	
	// init file system
	void *helper = init_fs("files/Test_writeRenewHash_file", "files/Test_writeRenewHash_dir", "files/Test_writeRenewHash_hash", 4);
    char *buf = "Yes sir!";
	compute_hash_tree(helper);
	int result = write_file("1.doc", 0, strlen(buf) + 1, buf, helper);
	close_fs(helper);
	
	create_directory("files/Test_writeRenewHash_dir_correct");
    add_file("files/Test_writeRenewHash_dir_correct", "1.doc", 10, 0, 0);
    add_file("files/Test_writeRenewHash_dir_correct", "2.doc", 10, 150, 1);
	add_file("files/Test_writeRenewHash_dir_correct", "3.doc", 10, 180, 2);
	add_file("files/Test_writeRenewHash_dir_correct", "\0", 0, 0, 3);
	
	create_Filedata("files/Test_writeRenewHash_file_correct");
	block = (char *)calloc(256, 1);
	for(int i = 0; i < 1; i++) {
		add_file_block("files/Test_writeRenewHash_file_correct",block, i);
	}
	int fd = open("files/Test_writeRenewHash_file_correct", O_WRONLY);
    write(fd, buf, strlen(buf) + 1);
    char hash_data[16];
    memcpy(block, buf, strlen(buf) + 1);
    fletcher((uint8_t *)block, 256, (uint8_t *)hash_data);
    close(fd);
    free(block);
	create_hashdata("files/Test_writeRenewHash_hash_correct");
	hash_block = (char *)calloc(16, 1);
	for(int i = 0; i < 1; i++) {
		add_hash_block("files/Test_writeRenewHash_hash_correct", hash_block, i);
	}
    int hd = open("files/Test_writeRenewHash_hash_correct", O_WRONLY);
    write(hd, hash_data, 16);
    close(hd);
	free(hash_block);
    
	if(compare_filedata("files/Test_writeRenewHash_file", "files/Test_writeRenewHash_file_correct") || compare_directory("files/Test_writeRenewHash_dir", "files/Test_writeRenewHash_dir_correct") || compare_hash("files/Test_writeRenewHash_hash", "files/Test_writeRenewHash_hash_correct")) {
		return 1;
	}
    if(result == 0) {
        return 0;
    }
	return result;
}

int writeWithRepack() {
	/* 
		This function tests write a file which needs more space and needs to be repacked.
	*/
	create_directory("files/Test_writeWithRepack_dir");
	add_file("files/Test_writeWithRepack_dir", "1.doc", 10, 0, 0);
    add_file("files/Test_writeWithRepack_dir", "2.doc", 10, 12, 1);
	add_file("files/Test_writeWithRepack_dir", "3.doc", 10, 180, 2);
	add_file("files/Test_writeWithRepack_dir", "\0", 0, 0, 3);
	
	
	create_Filedata("files/Test_writeWithRepack_file");
	char *block = (char *)calloc(256, 1);
	for(int i = 0; i < 1; i++) {
		add_file_block("files/Test_writeWithRepack_file",block, i);
	}
	free(block);
	
	create_hashdata("files/Test_writeWithRepack_hash");
	char *hash_block = (char *)calloc(16, 1);
	for(int i = 0; i < 1; i++) {
		add_hash_block("files/Test_writeWithRepack_hash", hash_block, i);
	}
	free(hash_block);
	
	// init file system
	void *helper = init_fs("files/Test_writeWithRepack_file", "files/Test_writeWithRepack_dir", "files/Test_writeWithRepack_hash", 4);
    char *buf = "hello world!";
	compute_hash_tree(helper);
	int result = write_file("1.doc", 0, strlen(buf) + 1, buf, helper);
	close_fs(helper);
	
	create_directory("files/Test_writeWithRepack_dir_correct");
    add_file("files/Test_writeWithRepack_dir_correct", "1.doc", 13, 20, 0);
    add_file("files/Test_writeWithRepack_dir_correct", "2.doc", 10, 0, 1);
	add_file("files/Test_writeWithRepack_dir_correct", "3.doc", 10, 10, 2);
	add_file("files/Test_writeWithRepack_dir_correct", "\0", 0, 0, 3);
	
	create_Filedata("files/Test_writeWithRepack_file_correct");
	block = (char *)calloc(256, 1);
	for(int i = 0; i < 1; i++) {
		add_file_block("files/Test_writeWithRepack_file_correct",block, i);
	}
	int fd = open("files/Test_writeWithRepack_file_correct", O_WRONLY);
	lseek(fd, 20, SEEK_SET);
    write(fd, buf, strlen(buf) + 1);
    char hash_data[16];
    memcpy(block + 20, buf, strlen(buf) + 1);
    fletcher((uint8_t *)block, 256, (uint8_t *)hash_data);
    close(fd);
    free(block); 
	create_hashdata("files/Test_writeWithRepack_hash_correct");
	hash_block = (char *)calloc(16, 1);
	for(int i = 0; i < 1; i++) {
		add_hash_block("files/Test_writeWithRepack_hash_correct", hash_block, i);
	}
    int hd = open("files/Test_writeWithRepack_hash_correct", O_WRONLY);
    write(hd, hash_data, 16);
    close(hd);
	free(hash_block);
    
	if(compare_filedata("files/Test_writeWithRepack_file", "files/Test_writeWithRepack_file_correct") || compare_directory("files/Test_writeWithRepack_dir", "files/Test_writeWithRepack_dir_correct") || compare_hash("files/Test_writeWithRepack_hash", "files/Test_writeWithRepack_hash_correct")) {
		return 1;
	}
    if(result == 0) {
        return 0;
    }
	return result;
}

int fileSizeTest() {
	/* 
		This function tests file_size file.
	*/
	create_directory("files/fileSizeTest_dir");
	add_file("files/fileSizeTest_dir", "2.doc", 10, 150, 0);
	add_file("files/fileSizeTest_dir", "3.doc", 10, 180, 1);
	add_file("files/fileSizeTest_dir", "1.doc", 10, 0, 2);
	add_file("files/fileSizeTest_dir", "\0", 0, 0, 0);
	
	
	create_Filedata("files/fileSizeTest_file");
	char *block = (char *)calloc(256, 1);
	for(int i = 0; i < 1; i++) {
		add_file_block("files/fileSizeTest_file",block, i);
	}
	free(block);
	
	create_hashdata("files/fileSizeTest_hash");
	char *hash_block = (char *)calloc(16, 1);
	for(int i = 0; i < 1; i++) {
		add_hash_block("files/fileSizeTest_hash", hash_block, i);
	}
	free(hash_block);
	
	// init file system
	void *helper = init_fs("files/fileSizeTest_file", "files/fileSizeTest_dir", "files/fileSizeTest_hash", 4);
	ssize_t size = file_size("1.doc", helper);
	close_fs(helper);
	if(size != 10) {
		return size;
  }
	return 0;
}

int fileSizeNotExisting() {
	/* 
		This function tests delete file.
	*/
	create_directory("files/fileSizeNotExisting_dir");
	add_file("files/fileSizeNotExisting_dir", "2.doc", 10, 150, 0);
	add_file("files/fileSizeNotExisting_dir", "3.doc", 10, 180, 1);
	add_file("files/fileSizeNotExisting_dir", "1.doc", 10, 0, 2);
	add_file("files/fileSizeNotExisting_dir", "\0", 0, 0, 3);
	
	
	create_Filedata("files/fileSizeNotExisting_file");
	char *block = (char *)calloc(256, 1);
	for(int i = 0; i < 1; i++) {
		add_file_block("files/fileSizeNotExisting_file",block, i);
	}
	free(block);
	
	create_hashdata("files/fileSizeNotExisting_hash");
	char *hash_block = (char *)calloc(16, 1);
	for(int i = 0; i < 1; i++) {
		add_hash_block("files/fileSizeNotExisting_hash", hash_block, i);
	}
	free(hash_block);
	
	// init file system
	void *helper = init_fs("files/fileSizeNotExisting_file", "files/fileSizeNotExisting_dir", "files/fileSizeNotExisting_hash", 4);
	ssize_t size = file_size("5.doc", helper);
	close_fs(helper);
	if(size == -1) {
		return 0;
	}
	return size;
}

int computeHashTree() {
	// build three files
	create_directory("files/Test_computeHashTree_dir");
	add_file("files/Test_computeHashTree_dir", "1.doc", 10, 0, 0);
	add_file("files/Test_computeHashTree_dir", "2.doc", 10, 150, 1);
	add_file("files/Test_computeHashTree_dir", "3.doc", 10, 180, 2);
	add_file("files/Test_computeHashTree_dir", "\0", 0, 0, 3);
	
	create_Filedata("files/Test_computeHashTree_file");
	uint8_t *block = (uint8_t *)calloc(256, 1);
	for(int i = 0; i < 64; i++) {
		((int *)block)[i] = 1;
	}
	for(int i = 0; i < 4; i++) {
		add_file_block("files/Test_computeHashTree_file", (char *)block, i);
	}
	free(block);
	
	create_hashdata("files/Test_computeHashTree_hash");
	uint8_t *hash_block = (uint8_t *)calloc(16, 1);
	for(int i = 0; i < 7; i++) {
		add_hash_block("files/Test_computeHashTree_hash", (char *)hash_block, i);
	}
	
	create_hashdata("files/Test_computeHashTree_hash_correct");
	for(int i = 0; i < 7; i++) {
		add_hash_block("files/Test_computeHashTree_hash_correct", (char *)hash_block, i);
	}
	free(hash_block);
	
	block = (uint8_t *)calloc(256, 1);
	for(int i = 0; i < 64; i++) {
		((int *)block)[i] = 1;
	}
	hash_block = (uint8_t *)calloc(16, 1);
	uint8_t *hash_buf = (uint8_t *)calloc(7 * 16, 1);
	
	for(int i = 0; i < 4; i++) {
		fletcher(block, 256, hash_buf + (i + 3) * 16);
	}
	for(int i = 0; i < 2; i++) {
		fletcher(hash_buf + (2 * i + 3) * 16, 32, hash_buf + (i + 1) * 16);
	}
	for(int i = 0; i < 1; i++) {
		fletcher(hash_buf + (i * 2 + 1) * 16, 32, hash_buf);
	}
	int temp = open("files/Test_computeHashTree_hash_correct", O_WRONLY);
	lseek(temp, 0, SEEK_SET);
	write(temp, hash_buf, 7 * 16);
	free(block);
	free(hash_buf);
	free(hash_block);
	
	// init file system
	void *helper = init_fs("files/Test_computeHashTree_file", "files/Test_computeHashTree_dir", "files/Test_computeHashTree_hash", 24);
	compute_hash_tree(helper);
	close_fs(helper);
	if(compare_hash("files/Test_computeHashTree_hash", "files/Test_computeHashTree_hash_correct") != 0) {
		printf("Yours:\n");
		display_hash("files/Test_computeHashTree_hash");
		printf("Correct:\n");
		display_hash("files/Test_computeHashTree_hash_correct");
		return 1;
	}
	
	return 0;
}

int computeHashTreeBig() {
	// build three files
	create_directory("files/Test_computeHashTreeBig_dir");
	add_file("files/Test_computeHashTreeBig_dir", "1.doc", 10, 0, 0);
	add_file("files/Test_computeHashTreeBig_dir", "2.doc", 10, 150, 1);
	add_file("files/Test_computeHashTreeBig_dir", "3.doc", 10, 180, 2);
	add_file("files/Test_computeHashTreeBig_dir", "\0", 0, 0, 3);
	
	create_Filedata("files/Test_computeHashTreeBig_file");
	uint8_t *block = (uint8_t *)calloc(256, 1);
	for(int i = 0; i < 64; i++) {
		add_file_block("files/Test_computeHashTreeBig_file", (char *)block, i);
	}
	free(block);
	
	create_hashdata("files/Test_computeHashTreeBig_hash");
	uint8_t *hash_block = (uint8_t *)calloc(16, 1);
	for(int i = 0; i < 127; i++) {
		add_hash_block("files/Test_computeHashTreeBig_hash", (char *)hash_block, i);
	}
	
	create_hashdata("files/Test_computeHashTreeBig_hash_correct");
	for(int i = 0; i < 127; i++) {
		add_hash_block("files/Test_computeHashTreeBig_hash_correct", (char *)hash_block, i);
	}
	free(hash_block);
	
	block = (uint8_t *)calloc(256, 1);
	for(int i = 0; i < 64; i++) {
		((int *)block)[i] = 1;
	}
	hash_block = (uint8_t *)calloc(16, 1);
	uint8_t *hash_buf = (uint8_t *)calloc(127 * 16, 1);
	
	for(int i = 0; i < 64; i++) {
		fletcher(block, 256, hash_buf + (i + 63) * 16);
	}
	for(int i = 0; i < 32; i++) {
		fletcher(hash_buf + (2 * i + 63) * 16, 32, hash_buf + (i + 31) * 16);
	}
	for(int i = 0; i < 16; i++) {
		fletcher(hash_buf + (2 * i + 31) * 16, 32, hash_buf + (i + 15) * 16);
	}
	for(int i = 0; i < 8; i++) {
		fletcher(hash_buf + (2 * i + 15) * 16, 32, hash_buf + (i + 7) * 16);
	}
	for(int i = 0; i < 4; i++) {
		fletcher(hash_buf + (2 * i + 7) * 16, 32, hash_buf + (i + 3) * 16);
	}
	for(int i = 0; i < 2; i++) {
		fletcher(hash_buf + (2 * i + 3) * 16, 32, hash_buf + (i + 1) * 16);
	}
	for(int i = 0; i < 1; i++) {
		fletcher(hash_buf + (i * 2 + 1) * 16, 32, hash_buf);
	}
	int temp = open("files/Test_computeHashTreeBig_hash_correct", O_WRONLY);
	lseek(temp, 0, SEEK_SET);
	write(temp, hash_buf, 127 * 16);
	free(block);
	free(hash_buf);
	free(hash_block);
	
	// init file system
	void *helper = init_fs("files/Test_computeHashTreeBig_file", "files/Test_computeHashTreeBig_dir", "files/Test_computeHashTreeBig_hash", 24);
	block = (uint8_t *)calloc(256, 1);
	for(int i = 0; i < 64; i++) {
		((int *)block)[i] = 1;
	}
	int fd = open("files/Test_computeHashTreeBig_file", O_RDWR);
	lseek(fd, 0, SEEK_SET);
	for(int i = 0; i < 64; i++) {
		write(fd, block, 256);
	}
	close(fd);
	free(block);
	compute_hash_tree(helper);
	close_fs(helper);
	if(compare_hash("files/Test_computeHashTreeBig_hash", "files/Test_computeHashTreeBig_hash_correct") != 0) {
		printf("Yours:\n");
		display_hash("files/Test_computeHashTreeBig_hash");
		printf("Correct:\n");
		display_hash("files/Test_computeHashTreeBig_hash_correct");
		return 1;
	}
	return 0;
}

int computeHashTreeBig2() {
	// build three files
	create_directory("files/Test_computeHashTreeBig2_dir");
	add_file("files/Test_computeHashTreeBig2_dir", "1.doc", 10, 0, 0);
	add_file("files/Test_computeHashTreeBig2_dir", "2.doc", 10, 150, 1);
	add_file("files/Test_computeHashTreeBig2_dir", "3.doc", 10, 180, 2);
	add_file("files/Test_computeHashTreeBig2_dir", "\0", 0, 0, 3);
	
	create_Filedata("files/Test_computeHashTreeBig2_file");
	uint8_t *block = (uint8_t *)calloc(256, 1);
	for(int i = 0; i < 128; i++) {
		add_file_block("files/Test_computeHashTreeBig2_file", (char *)block, i);
	}
	free(block);
	
	create_hashdata("files/Test_computeHashTreeBig2_hash");
	uint8_t *hash_block = (uint8_t *)calloc(16, 1);
	for(int i = 0; i < 255; i++) {
		add_hash_block("files/Test_computeHashTreeBig2_hash", (char *)hash_block, i);
	}
	
	create_hashdata("files/Test_computeHashTreeBig2_hash_correct");
	for(int i = 0; i < 255; i++) {
		add_hash_block("files/Test_computeHashTreeBig2_hash_correct", (char *)hash_block, i);
	}
	free(hash_block);
	
	block = (uint8_t *)calloc(256, 1);
	for(int i = 0; i < 64; i++) {
		((int *)block)[i] = 1;
	}
	hash_block = (uint8_t *)calloc(16, 1);
	uint8_t *hash_buf = (uint8_t *)calloc(255 * 16, 1);
	
	for(int i = 0; i < 128; i++) {
		fletcher(block, 256, hash_buf + (i + 127) * 16);
	}
	
	for(int i = 0; i < 64; i++) {
		fletcher(hash_buf + (2 * i + 127) * 16, 32, hash_buf + (i + 63) * 16);
	}
	for(int i = 0; i < 32; i++) {
		fletcher(hash_buf + (2 * i + 63) * 16, 32, hash_buf + (i + 31) * 16);
	}
	for(int i = 0; i < 16; i++) {
		fletcher(hash_buf + (2 * i + 31) * 16, 32, hash_buf + (i + 15) * 16);
	}
	for(int i = 0; i < 8; i++) {
		fletcher(hash_buf + (2 * i + 15) * 16, 32, hash_buf + (i + 7) * 16);
	}
	for(int i = 0; i < 4; i++) {
		fletcher(hash_buf + (2 * i + 7) * 16, 32, hash_buf + (i + 3) * 16);
	}
	for(int i = 0; i < 2; i++) {
		fletcher(hash_buf + (2 * i + 3) * 16, 32, hash_buf + (i + 1) * 16);
	}
	for(int i = 0; i < 1; i++) {
		fletcher(hash_buf + (i * 2 + 1) * 16, 32, hash_buf);
	}
	int temp = open("files/Test_computeHashTreeBig2_hash_correct", O_WRONLY);
	lseek(temp, 0, SEEK_SET);
	write(temp, hash_buf, 255 * 16);
	free(block);
	free(hash_buf);
	free(hash_block);
	
	// init file system
	void *helper = init_fs("files/Test_computeHashTreeBig2_file", "files/Test_computeHashTreeBig2_dir", "files/Test_computeHashTreeBig2_hash", 1);
	block = (uint8_t *)calloc(256, 1);
	for(int i = 0; i < 64; i++) {
		((int *)block)[i] = 1;
	}
	int fd = open("files/Test_computeHashTreeBig2_file", O_RDWR);
	lseek(fd, 0, SEEK_SET);
	for(int i = 0; i < 128; i++) {
		write(fd, block, 256);
	}
	close(fd);
	free(block);
	compute_hash_tree(helper);
	close_fs(helper);
	if(compare_hash("files/Test_computeHashTreeBig2_hash", "files/Test_computeHashTreeBig2_hash_correct") != 0) {
		printf("Yours:\n");
		display_hash("files/Test_computeHashTreeBig2_hash");
		printf("Correct:\n");
		display_hash("files/Test_computeHashTreeBig2_hash_correct");
		return 1;
	}
	return 0;
}

int computeHashBlock() {
	// build three files
	create_directory("files/Test_computeHashBlock_dir");
	add_file("files/Test_computeHashBlock_dir", "1.doc", 10, 0, 0);
	add_file("files/Test_computeHashBlock_dir", "2.doc", 10, 150, 1);
	add_file("files/Test_computeHashBlock_dir", "3.doc", 10, 180, 2);
	add_file("files/Test_computeHashBlock_dir", "\0", 0, 0, 3);
	
	create_Filedata("files/Test_computeHashBlock_file");
	uint8_t *block = (uint8_t *)calloc(256, 1);
	add_file_block("files/Test_computeHashBlock_file", (char *)block, 0);
	for(int i = 0; i < 64; i++) {
		((int *)block)[i] = 1;
	}
	for(int i = 0; i < 3; i++) {
		add_file_block("files/Test_computeHashBlock_file", (char *)block, i + 1);
	}
	free(block);
	
	create_hashdata("files/Test_computeHashBlock_hash");
	uint8_t *hash_block = (uint8_t *)calloc(16, 1);
	for(int i = 0; i < 7; i++) {
		add_hash_block("files/Test_computeHashBlock_hash", (char *)hash_block, i);
	}
	
	create_hashdata("files/Test_computeHashBlock_hash_correct");
	for(int i = 0; i < 7; i++) {
		add_hash_block("files/Test_computeHashBlock_hash_correct", (char *)hash_block, i);
	}
	free(hash_block);
	
	block = (uint8_t *)calloc(256, 1);
	for(int i = 0; i < 64; i++) {
		((int *)block)[i] = 1;
	}
	hash_block = (uint8_t *)calloc(16, 1);
	uint8_t *hash_buf = (uint8_t *)calloc(7 * 16, 1);
	
	for(int i = 0; i < 1; i++) {
		fletcher(block, 256, hash_buf + (i + 3) * 16);
	}
	for(int i = 0; i < 2; i++) {
		fletcher(hash_buf + (2 * i + 3) * 16, 32, hash_buf + (i + 1) * 16);
	}
	for(int i = 0; i < 1; i++) {
		fletcher(hash_buf + (i * 2 + 1) * 16, 32, hash_buf);
	}
	int temp = open("files/Test_computeHashBlock_hash_correct", O_WRONLY);
	lseek(temp, 0, SEEK_SET);
	write(temp, hash_buf, 7 * 16);
	free(block);
	free(hash_buf);
	free(hash_block);
	
	// init file system
	void *helper = init_fs("files/Test_computeHashBlock_file", "files/Test_computeHashBlock_dir", "files/Test_computeHashBlock_hash", 4);
	block = (uint8_t *)calloc(256, 1);
	for(int i = 0; i < 64; i++) {
		((int *)block)[i] = 1;
	}
	int fd = open("files/Test_computeHashBlock_file", O_RDWR);
	lseek(fd, 0, SEEK_SET);
	write(fd, block, 256);
	close(fd);
	free(block);
	compute_hash_block(0, helper);
	close_fs(helper);
	if(compare_hash("files/Test_computeHashBlock_hash", "files/Test_computeHashBlock_hash_correct") != 0) {
		printf("Yours:\n");
		display_hash("files/Test_computeHashBlock_hash");
		printf("Correct:\n");
		display_hash("files/Test_computeHashBlock_hash_correct");
		return 1;
	}
	
	return 0;
}



int computeHashTreeUseComputeHashBlock() {
	// build three files
	create_directory("files/Test_computeHashTreeUseComputeHashBlock_dir");
	add_file("files/Test_computeHashTreeUseComputeHashBlock_dir", "1.doc", 10, 0, 0);
	add_file("files/Test_computeHashTreeUseComputeHashBlock_dir", "2.doc", 10, 150, 1);
	add_file("files/Test_computeHashTreeUseComputeHashBlock_dir", "3.doc", 10, 180, 2);
	add_file("files/Test_computeHashTreeUseComputeHashBlock_dir", "\0", 0, 0, 3);
	
	create_Filedata("files/Test_computeHashTreeUseComputeHashBlock_file");
	uint8_t *block = (uint8_t *)calloc(256, 1);
	for(int i = 0; i < 4; i++) {
		add_file_block("files/Test_computeHashTreeUseComputeHashBlock_file", (char *)block, i);
	}
	free(block);
	
	create_hashdata("files/Test_computeHashTreeUseComputeHashBlock_hash");
	uint8_t *hash_block = (uint8_t *)calloc(16, 1);
	for(int i = 0; i < 7; i++) {
		add_hash_block("files/Test_computeHashTreeUseComputeHashBlock_hash", (char *)hash_block, i);
	}
	
	create_hashdata("files/Test_computeHashTreeUseComputeHashBlock_hash_correct");
	for(int i = 0; i < 7; i++) {
		add_hash_block("files/Test_computeHashTreeUseComputeHashBlock_hash_correct", (char *)hash_block, i);
	}
	free(hash_block);
	
	block = (uint8_t *)calloc(256, 1);
	for(int i = 0; i < 64; i++) {
		((int *)block)[i] = 1;
	}
	hash_block = (uint8_t *)calloc(16, 1);
	uint8_t *hash_buf = (uint8_t *)calloc(7 * 16, 1);
	
	for(int i = 0; i < 4; i++) {
		fletcher(block, 256, hash_buf + (i + 3) * 16);
	}
	for(int i = 0; i < 2; i++) {
		fletcher(hash_buf + (2 * i + 3) * 16, 32, hash_buf + (i + 1) * 16);
	}
	for(int i = 0; i < 1; i++) {
		fletcher(hash_buf + (i * 2 + 1) * 16, 32, hash_buf);
	}
	int temp = open("files/Test_computeHashTreeUseComputeHashBlock_hash_correct", O_WRONLY);
	lseek(temp, 0, SEEK_SET);
	write(temp, hash_buf, 7 * 16);
	free(block);
	free(hash_buf);
	free(hash_block);
	
	// init file system
	void *helper = init_fs("files/Test_computeHashTreeUseComputeHashBlock_file", "files/Test_computeHashTreeUseComputeHashBlock_dir", "files/Test_computeHashTreeUseComputeHashBlock_hash", 4);
	block = (uint8_t *)calloc(256, 1);
	for(int i = 0; i < 64; i++) {
		((int *)block)[i] = 1;
	}
	int fd = open("files/Test_computeHashTreeUseComputeHashBlock_file", O_RDWR);
	lseek(fd, 0, SEEK_SET);
	for(int i = 0; i < 4; i++) {
		write(fd, block, 256);
	}
	close(fd);
	free(block);
	for(int i = 0; i < 4; i ++) {
		compute_hash_block(i, helper);
	}
	close_fs(helper);
	if(compare_hash("files/Test_computeHashTreeUseComputeHashBlock_hash", "files/Test_computeHashTreeUseComputeHashBlock_hash_correct") != 0) {
		printf("Yours:\n");
		display_hash("files/Test_computeHashTreeUseComputeHashBlock_hash");
		printf("Correct:\n");
		display_hash("files/Test_computeHashTreeUseComputeHashBlock_hash_correct");
		return 1;
	}
	
	return 0;
}

int computeHashTreeUseComputeHashBlockBigOne() {
	// build three files
	create_directory("files/Test_computeHashTreeUseComputeHashBlockBigOne_dir");
	add_file("files/Test_computeHashTreeUseComputeHashBlockBigOne_dir", "1.doc", 10, 0, 0);
	add_file("files/Test_computeHashTreeUseComputeHashBlockBigOne_dir", "2.doc", 10, 150, 1);
	add_file("files/Test_computeHashTreeUseComputeHashBlockBigOne_dir", "3.doc", 10, 180, 2);
	add_file("files/Test_computeHashTreeUseComputeHashBlockBigOne_dir", "\0", 0, 0, 3);
	
	create_Filedata("files/Test_computeHashTreeUseComputeHashBlockBigOne_file");
	uint8_t *block = (uint8_t *)calloc(256, 1);
	for(int i = 0; i < 16; i++) {
		add_file_block("files/Test_computeHashTreeUseComputeHashBlockBigOne_file", (char *)block, i);
	}
	free(block);
	
	create_hashdata("files/Test_computeHashTreeUseComputeHashBlockBigOne_hash");
	uint8_t *hash_block = (uint8_t *)calloc(16, 1);
	for(int i = 0; i < 31; i++) {
		add_hash_block("files/Test_computeHashTreeUseComputeHashBlockBigOne_hash", (char *)hash_block, i);
	}
	
	create_hashdata("files/Test_computeHashTreeUseComputeHashBlockBigOne_hash_correct");
	for(int i = 0; i < 31; i++) {
		add_hash_block("files/Test_computeHashTreeUseComputeHashBlockBigOne_hash_correct", (char *)hash_block, i);
	}
	free(hash_block);
	
	block = (uint8_t *)calloc(256, 1);
	for(int i = 0; i < 64; i++) {
		((int *)block)[i] = 1;
	}
	hash_block = (uint8_t *)calloc(16, 1);
	uint8_t *hash_buf = (uint8_t *)calloc(31 * 16, 1);
	
	for(int i = 0; i < 16; i++) {
		fletcher(block, 256, hash_buf + (i + 15) * 16);
	}
	for(int i = 0; i < 8; i++) {
		fletcher(hash_buf + (2 * i + 15) * 16, 32, hash_buf + (i + 7) * 16);
	}
	for(int i = 0; i < 4; i++) {
		fletcher(hash_buf + (2 * i + 7) * 16, 32, hash_buf + (i + 3) * 16);
	}
	for(int i = 0; i < 2; i++) {
		fletcher(hash_buf + (2 * i + 3) * 16, 32, hash_buf + (i + 1) * 16);
	}
	for(int i = 0; i < 1; i++) {
		fletcher(hash_buf + (i * 2 + 1) * 16, 32, hash_buf);
	}
	int temp = open("files/Test_computeHashTreeUseComputeHashBlockBigOne_hash_correct", O_WRONLY);
	lseek(temp, 0, SEEK_SET);
	write(temp, hash_buf, 31 * 16);
	free(block);
	free(hash_buf);
	free(hash_block);
	
	// init file system
	void *helper = init_fs("files/Test_computeHashTreeUseComputeHashBlockBigOne_file", "files/Test_computeHashTreeUseComputeHashBlockBigOne_dir", "files/Test_computeHashTreeUseComputeHashBlockBigOne_hash", 24);
	block = (uint8_t *)calloc(256, 1);
	for(int i = 0; i < 64; i++) {
		((int *)block)[i] = 1;
	}
	int fd = open("files/Test_computeHashTreeUseComputeHashBlockBigOne_file", O_RDWR);
	lseek(fd, 0, SEEK_SET);
	for(int i = 0; i < 16; i++) {
		write(fd, block, 256);
	}
	close(fd);
	free(block);
	for(int i = 0; i < 16; i ++) {
		compute_hash_block(i, helper);
	}
	close_fs(helper);
	if(compare_hash("files/Test_computeHashTreeUseComputeHashBlockBigOne_hash", "files/Test_computeHashTreeUseComputeHashBlockBigOne_hash_correct") != 0) {
		printf("Yours:\n");
		display_hash("files/Test_computeHashTreeUseComputeHashBlockBigOne_hash");
		printf("Correct:\n");
		display_hash("files/Test_computeHashTreeUseComputeHashBlockBigOne_hash_correct");
		return 1;
	}
	
	return 0;
}
