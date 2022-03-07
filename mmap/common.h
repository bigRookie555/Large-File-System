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

namespace skx{
	namespace LFS{
		const int32_t EXIT_DISK_OPER_INCOMPLETE = -8012; //读或写的长度小于要求长度
		const int32_t TFS_SUCCESS = 0;
	}
}

#endif  //_COMMON_H_INCLUDE_
