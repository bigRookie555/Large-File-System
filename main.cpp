#include <iostream>
#include "mmap_file.h"
#include <string>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>

using namespace std;
using namespace LFS;

static const mode_t OPEN_MODE=0644;
const static MMapOption mmap_option={10240000,4096,4096};  //内存映射参数

int open_file(string file_name,int open_flags){
	int fd=open(file_name.c_str(),open_flags,OPEN_MODE);
	if(fd<0){
		return -errno;
	}
	return fd;
}


int main(){
	const char *filename="./mapfile_test.txt";
	
	int fd=open_file(filename,O_RDWR|O_CREAT|O_LARGEFILE);    
	
	if(fd<0) {
		return -1;
		fprintf(stderr,"open file failed. filename:%s,error desc:%s\n",filename,strerror(-fd));
	}                                                                                      
	
	MMapFile *map_file=new MMapFile(mmap_option,fd);
	
	bool is_mapped = map_file->map_file(true);
	if(is_mapped){
		map_file->remap_file();
		memset(map_file->get_data(),'8',map_file->get_size());
		map_file->sync_file();
		map_file->munmap_file();
	}
	else{
		fprintf(stderr,"map file failed\n");
	}
	
	close(fd);
	
	return 0;
}