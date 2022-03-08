#ifndef _MMAP_FILE_H_
#define _MMAP_FILE_H_

#include <unistd.h>
#include "common.h"

namespace skx{
	namespace LFS{
		struct MMapOption{
			int32_t max_mmap_size_;
			int32_t first_mmap_size_;
			int32_t per_mmap_size_;
		};
	
		class MMapFile{
		public:
			MMapFile();
			explicit MMapFile(const int fd);
			MMapFile(const MMapOption& mmap_option,const int fd);
			~MMapFile();
		
			bool sync_file(); //同步文件
			bool map_file(const bool write=false); //将文件映射到内存并设置访问权限
			void *get_data()const; //获取映射到内存的首地址
			int32_t get_size() const; //获取映射数据的大小
			bool munmap_file(); //解除映射
			bool remap_file(); //重新映射 
	
		private:
			bool ensure_file_size(const int32_t size);
		
			int32_t size_;
			int fd_;
			void *data_;
			MMapOption mmap_file_option_;
		};
	}
}



#endif