#ifndef _COMMON_H_INCLUDE_
#define _COMMON_H_INCLUDE_

#include <iostream>
//#include "mmap_file.h"
#include <string>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdint.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <sys/mman.h>
#include <unistd.h>
#include <stdlib.h>
#include <inttypes.h>

namespace skx{
	namespace LFS{
		const int32_t EXIT_DISK_OPER_INCOMPLETE = -8012; //读或写的长度小于要求长度
		const int32_t TFS_SUCCESS = 0;
		const int32_t TFS_ERROR = -1;
		const int32_t EXIT_INDEX_ALREADY_LOADED_ERROR=-8013;
		const int32_t EXIT_META_UNEXPECT_FOUND_ERROR = -8014;  
		const int32_t EXIT_INDEX_CORRUPT_ERROR = -8015; 
		const int32_t EXIT_BLOCKID_CONFLICT_ERROR=-8016;
		const int32_t EXIT_BUCKET_CONFIGURE_ERROR=-8017;
		
		static const std::string MAINBLOCK_DIR_PREFIX = "/mainblock/";
		static const std::string INDEX_DIR_PREFIX = "/index/";
		static const mode_t DIR_MODE = 0755;  //目录权限
		
		struct MMapOption{
			int32_t max_mmap_size_;
			int32_t first_mmap_size_;
			int32_t per_mmap_size_;
		};  //文件映射选项
		
		//块信息
		struct BlockInfo
		{
			uint32_t block_id_; //块编号
			int32_t version_;  //块当前版本号
			int32_t file_count_;   //当前已保存的文件数
			int32_t size_t_;  //当前已保存的文件数据大小
			int32_t del_file_count_;  //已删除的文件数量
			int32_t del_size_;  //已删除的文件数据总大小
			uint32_t seq_no_;  //下一个可分配的文件编号
			
			BlockInfo(){
				memset(this,0,sizeof(BlockInfo));
			}
			
			inline bool operator==(const BlockInfo& rhs) const{
				return block_id_==rhs.block_id_&&version_==rhs.version_&&file_count_==rhs.file_count_&&size_t_==rhs.size_t_&&del_file_count_==rhs.del_file_count_&&del_size_==rhs.del_size_&&seq_no_==rhs.seq_no_;
			}
		};
		
		struct MetaInfo
		{
			MetaInfo(){
				init();
			}
			
			MetaInfo(const uint64_t file_id,const int32_t in_offset,const int32_t file_size,const int32_t next_meta_offset){
				fileid_=file_id;
				location_.inner_offset_=in_offset;
				location_.size_=file_size;
				next_meta_offset_=next_meta_offset;
			}
			
			MetaInfo(const MetaInfo& meta_info){
				memcpy(this,&meta_info,sizeof(MetaInfo));
			}
			
			MetaInfo& operator=(const MetaInfo& meta_info){
				if(this==&meta_info) return *this;
				
				fileid_=meta_info.fileid_;
				location_.inner_offset_=meta_info.location_.inner_offset_;
				location_.size_=meta_info.location_.size_;
				next_meta_offset_=meta_info.next_meta_offset_;
				
				//return *this;
			}
			
			MetaInfo& clone(const MetaInfo& meta_info){
				assert(this!=&meta_info);
				
				fileid_=meta_info.fileid_;
				location_.inner_offset_=meta_info.location_.inner_offset_;
				location_.size_=meta_info.location_.size_;
				next_meta_offset_=meta_info.next_meta_offset_;
				
				return *this;
			}
			
			bool operator==(const MetaInfo &rhs) const{
				return fileid_==rhs.fileid_&&location_.inner_offset_==rhs.location_.inner_offset_&&location_.size_==rhs.location_.size_&&next_meta_offset_==rhs.next_meta_offset_;
			}
			
			uint64_t get_key() const{
				return fileid_;
			}
			
			void set_key(const uint64_t key){
				fileid_=key;
			}
			
			uint64_t get_file_id() const{
				return fileid_;
			}
			
			void set_file_id(const uint64_t file_id){
				fileid_=file_id;
			}
			
			int32_t get_offset() const{
				return location_.inner_offset_;
			}
			
			void set_offset(const int32_t offset){
				location_.inner_offset_=offset;
			}
			
			int32_t get_size() const{
				return location_.size_;
			}
			
			void set_size(const int32_t file_size){
				location_.size_=file_size;
			}
			
			int32_t get_next_meta_offset() const{
				return next_meta_offset_;
			}
			
			void set_next_meta_offset(const int32_t offset){
				next_meta_offset_=offset;
			}
		
		private:
			uint64_t fileid_; //文件编号
			
			struct{
				int32_t inner_offset_;
				int32_t size_;
			}location_;
			
			//RawMeta raw_meta_;  //文件元数据
			int32_t next_meta_offset_; //当前哈希链下一个结点在索引文件中的偏移量	
		
			void init(){
				fileid_=0;
				location_.inner_offset_=0;
				location_.size_=0;
				next_meta_offset_=0;
			}
		};
	}
}

#endif  //_COMMON_H_INCLUDE_
