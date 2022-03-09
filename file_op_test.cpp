#include "common.h"
#include "file_op.h"

using namespace std;
using namespace skx;

int main(){
	const char* file_name="file_op.txt";
	
	LFS::FileOperation *fileOP = new LFS::FileOperation(file_name,O_RDWR|O_LARGEFILE|O_CREAT);
	
	int fd=fileOP->open_file();
	
	if(fd<0){
		fprintf(stderr,"open file failed. reason:%s\n",strerror(-fd));
		exit(-1);
	}
	
	char buffer[65];
	
	memset(buffer,'8',64);
	
	int ret=fileOP->pwrite_file(buffer,64,0);
	if(ret<0){
		fprintf(stderr,"write file failed. reason:%s\n",strerror(-ret));
	}
	
	ret=fileOP->pread_file(buffer,64,0);
	if(ret<0){
		fprintf(stderr,"read file failed. reason:%s\n",strerror(-ret));
	}else{
		buffer[64]='\0';
		printf("read:%s\n",buffer);
	}
	
	memset(buffer,'9',64);
	ret=fileOP->write_file(buffer,64);
	if(ret<0){
		fprintf(stderr,"write_file file failed. reason:%s\n",strerror(-ret));
	}
	
	fileOP->close_file();
	
	return 0;
}