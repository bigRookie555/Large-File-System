#include "mmap_file_op.h"
#include <iostream>

using namespace std;
using namespace skx;

const static LFS::MMapOption mmap_option={10240000,4096,4096};  //内存映射参数

int main(){
	int ret=0;
	
	const char* filename="mmap_file_op.txt";
	
	LFS::MMapFileOperation *mmfo=new LFS::MMapFileOperation(filename);
	
	int fd=mmfo->open_file();
	if(fd<0){
		fprintf(stderr,"open file failed. reason:%s\n",strerror(-fd));
		exit(-1);
	}
	
	ret=mmfo->mmap_file(mmap_option);
	if(ret==LFS::TFS_ERROR){
		fprintf(stderr,"mmap_file failed:%s\n",strerror(errno));
		mmfo->close_file();
		exit(-2);
	}
	
	char buffer[128];
	memset(buffer,'6',128);
	
	ret=mmfo->pwrite_file(buffer,128,8);
	if(ret<0){
		fprintf(stderr,"write file failed. reason:%s\n",strerror(-ret));
	}
	
	ret=mmfo->pread_file(buffer,128,8);
	if(ret<0){
		fprintf(stderr,"read file failed. reason:%s\n",strerror(-ret));
	}else{
		buffer[127]='\0';
		printf("read:%s\n",buffer);
	}
	
	ret=mmfo->pwrite_file(buffer,128,4096);
	
	ret=mmfo->flush_file();
	if(ret==LFS::TFS_ERROR) fprintf(stderr,"flush_file failed. reason:%s\n",strerror(errno));
	
	mmfo->munmap_file();
	
	mmfo->close_file();
	return 0;
}