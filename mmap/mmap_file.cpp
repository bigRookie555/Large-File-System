#include "mmap_file.h"
#include <stdio.h>

static int debug=1;

namespace LFS{
	MMapFile::MMapFile():size_(0),fd_(-1),data_(NULL){}
	
	MMapFile::MMapFile(const int fd):size_(0),fd_(-1),data_(NULL){}
	
	MMapFile::MMapFile(const MMapOption& mmap_option,const int fd):size_(0),fd_(fd),data_(NULL){
		mmap_file_option_.max_mmap_size_=mmap_option.max_mmap_size_;
		mmap_file_option_.first_mmap_size_=mmap_option.first_mmap_size_;
		mmap_file_option_.per_mmap_size_=mmap_option.per_mmap_size_;
	}
	
	MMapFile::~MMapFile(){
		if(data_){
			if(debug){
				printf("mmap file destruct,fd=%d,maped size=%d,data addr=%p\n",fd_,size_,data_);
			}
			msync(data_,size_,MS_SYNC); //同步到磁盘， 以同步的方式
			munmap(data_,size_);  //释放空间
			
			size_=0;
			data_=NULL;
			fd_=-1;
			mmap_file_option_.max_mmap_size_=0;
			mmap_file_option_.first_mmap_size_=0;
			mmap_file_option_.per_mmap_size_=0;
		}
	}
	
	bool MMapFile::sync_file(){
		if(NULL!=data_&&size_>0){
			return msync(data_,size_,MS_ASYNC)==0;
		}
		
		return true;
	}
	
	bool MMapFile::map_file(const bool write){ //参数用来判断是否可写
		int flag=PROT_READ;
		
		if(write){
			flag|=PROT_WRITE;
		}
		
		//防卫性编程
		if(fd_<0){
			return false;
		}
		
		if(mmap_file_option_.max_mmap_size_==0){
			return false;
		}
		
		if(size_<=mmap_file_option_.max_mmap_size_){
			size_=mmap_file_option_.first_mmap_size_;
		}
		else{
			size_=mmap_file_option_.max_mmap_size_;
		}
		
		if(!ensure_file_size(size_)){
			fprintf(stderr,"ensure file size failed in map_file,size=%d\n",size_);
			return false;
		}
		
		data_=mmap(NULL,size_,flag,MAP_SHARED,fd_,0);
		
		if(MAP_FAILED==data_){
			fprintf(stderr,"map file failed:%s",strerror(errno));
			size_=0;
			fd_=-1;
			data_=NULL;
			return false;
		}
		
		if(debug) printf("mmap file successed,fd=%d,maped size=%d,data addr=%p\n",fd_,size_,data_);
		
		return true;
	}
	
	void *MMapFile::get_data()const{
		return data_;
	}
	
	int32_t MMapFile::get_size() const{ return size_; }
	
	bool MMapFile::munmap_file(){
		if(munmap(data_,size_)==0) return true;
		else return false;
	}
	
	bool MMapFile::ensure_file_size(const int32_t size){  //确保文件大小与内存映射一致
		struct stat s;
		if(fstat(fd_,&s)<0){
			fprintf(stderr,"fstat error:%s\n",strerror(errno));
			return false;
		}
		
		if(s.st_size<size){
			if(ftruncate(fd_,size)<0){
				fprintf(stderr,"truncate error:%s\n",strerror(errno));
				return false;
			}
		}
		
		return true;
	}
	
	bool MMapFile::remap_file(){
		//防御性编程
		if(fd_<0||data_==NULL){
			fprintf(stderr,"mremap not mapped yet\n");
			return false;
		}
		
		if(size_==mmap_file_option_.max_mmap_size_){
			fprintf(stderr,"already mapped max size:%d,max size:%d\n",size_,mmap_file_option_.max_mmap_size_);
			return false;
		}
		
		int32_t new_size=size_+mmap_file_option_.per_mmap_size_;
		if(new_size>mmap_file_option_.max_mmap_size_){   //新size是否比maxsize大？
			new_size=mmap_file_option_.max_mmap_size_;
		}
		
		//文件与内存映射大小必须一致
		if(!ensure_file_size(new_size)){
			fprintf(stderr,"ensure file size failed in remap_file,new_size=%d\n",new_size);
			return false;
		}
		
		if(debug) printf("mremap start. fd:%d,now size%d,new size:%d,old data:%p\n",fd_,size_,new_size,data_);
		
		void *new_map_data=mremap(data_,size_,new_size,MREMAP_MAYMOVE);  //重新申请更大的内存
		
		if(new_map_data==MAP_FAILED){
			fprintf(stderr,"mremap failed,fd:%d,new size:%d,error desc:%s\n",fd_,new_size,strerror(errno));
			return false;
		}
		else{
			if(debug) printf("mremap success. fd:%d,now size%d,new size:%d,old data:%p,new data:%p\n",fd_,size_,new_size,data_,new_map_data);
		}
		
		data_=new_map_data;
		size_=new_size;
		
		return true;
	}
}