#ifndef _INDEX_HANDLE_H_
#define _INDEX_HANDLE_H_

#include "common.h"
#include "mmap_file_op.h"

namespace skx{
	namespace LFS{
		struct IndexHeader{
		public:
			IndexHeader(){
				memset(this,0,sizeof(IndexHeader));
			}
			
			BlockInfo block_info_; //块信息
			int32_t bucket_size_; //哈希桶大小
			int32_t data_file_offset_;// 块中下一个可写部分的偏移量
			int32_t index_file_size_; // IndexHeader+所有哈希桶   的  偏移
			int32_t free_head_offset_; //可重用的结点
		};
		
		class IndexHandle{
		public:
			IndexHandle(const std::string& base_path,const uint32_t main_block_id);
			~IndexHandle();
			
			int create(const uint32_t logic_block_id,const int32_t bucket_size,const MMapOption map_option);
			
			int load(const uint32_t logic_block_id,const int32_t bucket_size,const MMapOption map_option);
			
			int remove(const uint32_t logic_block_id); //munmap  &  unlink file
			
			int flush();
			
			IndexHeader *index_header(){  //用来访问内存映射区的 indexheader
				return reinterpret_cast<IndexHeader*>(file_op_->get_map_data()); //强制类型转换
			} // 返回内存映射的地址
			
			int update_block_info(const OperType oper_type,const uint32_t modify_size);
			
			BlockInfo *block_info(){  //用来访问内存映射区的 block info
				return reinterpret_cast<BlockInfo *>(file_op_->get_map_data()); //强制类型转换
			} // 返回内存映射的地址
			
			int32_t* bucket_slot(){
				return reinterpret_cast<int32_t*>(reinterpret_cast<char*>(file_op_->get_map_data())+sizeof(IndexHeader)); //获得哈希数组的首地址
			}
			
			int32_t bucket_size() const{
				return reinterpret_cast<IndexHeader*>(file_op_->get_map_data())->bucket_size_;
			}
			
			int32_t get_block_data_offset() const{
				return reinterpret_cast<IndexHeader*>(file_op_->get_map_data())->data_file_offset_;
			}
			
			void commit_block_data_offset(const int file_size){
				reinterpret_cast<IndexHeader*>(file_op_->get_map_data())->data_file_offset_+=file_size;
			}
			
			int32_t write_segment_meta(const uint64_t key,MetaInfo &meta);
			
			int32_t hash_find(const uint64_t key,int32_t &current_offset,int32_t& previous_offset);
			
			int32_t hash_insert(const uint64_t key,int32_t previous_offset,MetaInfo &meta);
		private:
			bool hash_compare(const uint64_t left_key,const uint64_t right_key) {
				return left_key==right_key;
			}
		
			MMapFileOperation *file_op_;
			bool is_load_;
		};
	}
}


#endif  //_INDEX_HANDLE_H_