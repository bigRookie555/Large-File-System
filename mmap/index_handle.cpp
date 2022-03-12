#include "common.h"
#include "index_handle.h"

static int debug=1;

namespace skx{
	namespace LFS{
		IndexHandle::IndexHandle(const std::string& base_path,const uint32_t main_block_id){
			//create file_op_handle object
			std::stringstream tmp_stream;
			tmp_stream<<base_path<<INDEX_DIR_PREFIX<<main_block_id;  
			
			std::string index_path;
			tmp_stream>>index_path;  //index文件路径
			
			file_op_=new MMapFileOperation(index_path,O_CREAT | O_RDWR | O_LARGEFILE);
			is_load_=false;
		}
		
		IndexHandle::~IndexHandle(){
			if(file_op_){
				delete file_op_;
				file_op_=NULL;
			}
		}
		
		int IndexHandle::create(const uint32_t logic_block_id,const int32_t bucket_size,const MMapOption map_option){
			int ret=TFS_SUCCESS;
			if(debug) printf("creat index,block_id:%u , bucket size: %d ,max_mmap_size :%d , first map size:%d, per map size:%d\n",logic_block_id,bucket_size,map_option.max_mmap_size_,map_option.first_mmap_size_,map_option.per_mmap_size_);
			
			if(is_load_){
				return EXIT_INDEX_ALREADY_LOADED_ERROR;
			}
			
			int64_t file_size=file_op_->get_file_size();
			if(file_size<0) return TFS_ERROR;
			else if(file_size==0){
				IndexHeader i_Header;
				i_Header.block_info_.block_id_=logic_block_id;
				i_Header.block_info_.seq_no_=1;
				i_Header.bucket_size_=bucket_size;
				i_Header.index_file_size_=sizeof(IndexHeader) + bucket_size*sizeof(int32_t);
				
				char *ini_data = new char[i_Header.index_file_size_];  //将索引数据放在此处
				memcpy(ini_data,&i_Header,sizeof(IndexHeader));
				memset(ini_data+sizeof(IndexHeader),0,i_Header.index_file_size_-sizeof(IndexHeader));
				
				//将索引头和哈希桶写入索引文件
				ret = file_op_->pwrite_file(ini_data,i_Header.index_file_size_,0);
				
				delete ini_data;
				ini_data=NULL;
				
				if(ret!=TFS_SUCCESS) return ret;
				
				ret=file_op_->flush_file();
				
				if(ret!=TFS_SUCCESS) return ret;
			}
			else{
				return EXIT_META_UNEXPECT_FOUND_ERROR;
			}
			
			ret=file_op_->mmap_file(map_option);
			
			if(ret!=TFS_SUCCESS) return ret;
			
			is_load_=true;
			if(debug) printf("create success\n");
			
			//if(debug) printf("init blockid: %d index successful. data file size: %d,index file size: %d, bucket_size: %d,free head offset: %d,seq_no: %d,size: %d, filecount :%d, del_size: %d, del_file_count: %d,version: %d\n")
			
			return TFS_SUCCESS;
		}
		
		int IndexHandle::load(const uint32_t logic_block_id,const int32_t bucket_size,const MMapOption map_option){
			if(is_load_) return EXIT_INDEX_ALREADY_LOADED_ERROR;
			
			int ret = TFS_SUCCESS;
			
			int64_t file_size=file_op_->get_file_size();
			
			if(file_size<0) return file_size;
			else if(file_size==0) return EXIT_INDEX_CORRUPT_ERROR; //empty file
			
			MMapOption tmp_map_option = map_option;
			
			if(file_size>tmp_map_option.first_mmap_size_&&file_size<=tmp_map_option.max_mmap_size_){
				tmp_map_option.first_mmap_size_=file_size;
			}
			
			ret = file_op_->mmap_file(tmp_map_option);
			
			if(ret!= TFS_SUCCESS) return ret;
			
			if(this->bucket_size()==0||block_info()->block_id_==0) {
				fprintf(stderr,"Index corrupt error. blockid:%u,bucket size: %d\n",block_info()->block_id_,this->bucket_size());
				return EXIT_INDEX_CORRUPT_ERROR;
			}
			
			//检查文件大小
			int32_t index_file_size = sizeof(IndexHeader) + this->bucket_size()*sizeof(int32_t);
			
			if(file_size<index_file_size){//可能索引文件损坏
				fprintf(stderr,"Index corrupt error. blockid:%u,bucket size: %d,file size:%"PRId64",index file size:%d\n",block_info()->block_id_,this->bucket_size(),file_size,index_file_size);
				return EXIT_INDEX_CORRUPT_ERROR;
			}
			
			if(logic_block_id!=block_info()->block_id_){
				fprintf(stderr,"Block id conflict. blockid:%u,index blockid: %u\n",logic_block_id,block_info()->block_id_);
				return EXIT_BLOCKID_CONFLICT_ERROR;
			}
			
			//检查bucketsize
			if(bucket_size!=this->bucket_size()){
				fprintf(stderr,"Index configure error. old bucket size:%d,new bucket size: %d\n",this->bucket_size(),bucket_size);
				return EXIT_BUCKET_CONFIGURE_ERROR;
			}
			
			is_load_=true;
			
			if(debug) printf("load success\n");
			
			return TFS_SUCCESS;
		}
		
		int IndexHandle::remove(const uint32_t logic_block_id){
			if(is_load_){
				if(logic_block_id!=block_info()->block_id_){
					fprintf(stderr,"block id conflict. blockid:%d,index blockid:%d\n",logic_block_id,block_info()->block_id_);
					return EXIT_BLOCKID_CONFLICT_ERROR;
				}
			}
			
		}
	}
}