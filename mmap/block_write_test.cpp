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
	
	
	
	//1.加载索引文件
	LFS::IndexHandle *index_handle = new LFS::IndexHandle(".",block_id);  //索引文件句柄
	
	if(debug) printf("load index...\n");
	
	ret=index_handle->load(block_id,bucket_size,mmap_option);
	
	if(ret!=LFS::TFS_SUCCESS){
		fprintf(stderr,"load index %d failed\n",block_id);
		delete index_handle;
		exit(-2);
	}
	
	//2.写入文件到主块文件
	std::stringstream tmp_stream;
	tmp_stream<<"."<<LFS::MAINBLOCK_DIR_PREFIX<<block_id;
	tmp_stream>>mainblock_path;
	
	LFS::FileOperation *mainblock = new LFS::FileOperation(mainblock_path,O_RDWR | O_LARGEFILE | O_CREAT);
	
	char buffer[4096];
	memset(buffer,'6',sizeof buffer);
	
	int32_t data_offset = index_handle->get_block_data_offset(); //块偏移
	
	uint32_t file_no = index_handle->block_info()->seq_no_;
	
	if(ret=mainblock->pwrite_file(buffer,sizeof(buffer),data_offset)!= TFS_SUCCESS){
		fprintf(stderr,"write to main block failed. ret: %d, reason: %s\n",ret,strerror(errno));
		
		mainblock->close_file();
		
		delete mainblock;
		delete index_handle;
		exit(-3);
	}
	
	//3.索引文件中写入metainfo
	LFS::Metainfo meta;
	meta.set_file_id(file_no);
	meta.set_offset(data_offset);
	meta.set_size(sizeof(buffer));
	
	
	delete mainblock;
	delete index_handle;
	
	return 0;
}