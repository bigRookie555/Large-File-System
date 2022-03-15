#include "common.h"
#include "file_op.h"
#include "index_handle.h"
#include <sstream>

using namespace std;
using namespace skx;

static int debug=1;

const static LFS::MMapOption mmap_option={1024000,4096,4096};  //内存映射参数
const static uint32_t main_blocksize = 1024*1024*64; //主块文件大小
const static uint32_t bucket_size = 1000; //哈希桶的大小
uint32_t block_id = 1;

int main(int argc,char *argv[]){
	std::string mainblock_path;
	std::string index_path;
	int32_t ret=LFS::TFS_SUCCESS;

	cout<<"Type your blockid:"<<endl;
	cin>>block_id;
	
	if(block_id<1) {
		cerr<<"Invalid blockid,exit."<<endl;
		exit(-1);
	}
	
	
	
	//1.创建索引文件
	LFS::IndexHandle *index_handle = new LFS::IndexHandle(".",block_id);  //索引文件句柄
	
	if(debug) printf("init index...\n");
	
	ret=index_handle->create(block_id,bucket_size,mmap_option);
	
	if(ret!=LFS::TFS_SUCCESS){
		fprintf(stderr,"init index %d failed\n",block_id);
		delete index_handle;
		exit(-3);
	}
	
	//2.生成主块文件
	std::stringstream tmp_stream;
	tmp_stream<<"."<<LFS::MAINBLOCK_DIR_PREFIX<<block_id;
	tmp_stream>>mainblock_path;
	
	LFS::FileOperation *mainblock = new LFS::FileOperation(mainblock_path,O_RDWR | O_LARGEFILE | O_CREAT);
	
	ret=mainblock->ftruncate_file(main_blocksize);
	
	if(ret!=0){
		fprintf(stderr,"create main block %s failed. reason: %s\n",mainblock_path.c_str(),strerror(errno));
		delete mainblock;
		index_handle->remove(block_id);
		exit(-2);
	}
	
	//其他操作
	mainblock->close_file();
	index_handle->flush();
	
	delete mainblock;
	delete index_handle;
	
	return 0;
}