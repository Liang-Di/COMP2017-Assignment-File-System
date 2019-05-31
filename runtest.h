#ifndef TESTH
#define TESTH

int no_operation();

// create_file function test
int normalCreate();
int insertCreate();
int createFirstAvailableInDir();
int createWithRepack();
int createExist();
int createNoplace();

// resize_file function test
int resizeFile();
int resizeNotExist();
int resizeWithRepack();
int resizeNoplace();
	
// repack function test
int repackTest();
int repackDirOutOfOrder();
	
// delete function test
int deletefileTest();
int deletefileNotExists();

// rename function test
int renameTest();
int renameNotExists();

// read function test
int readTest();
int readNotExists();
int readInvalidOffset();
int readWithWrongHashValue();

// write function test
int writeTest();
int writeNotExists();
int writeRenewHash();
int writeWithRepack();

// file_size function test
int fileSizeTest();
int fileSizeNotExisting();

// compute_hash_tree function test
int computeHashTree();
int computeHashTreeBig();
int computeHashTreeBig2();

//compute_hash_block function test
int computeHashBlock();
int computeHashTreeUseComputeHashBlock();
int computeHashTreeUseComputeHashBlockBigOne();

#endif
