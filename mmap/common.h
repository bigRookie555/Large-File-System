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
	}
}

#endif  //_COMMON_H_INCLUDE_
