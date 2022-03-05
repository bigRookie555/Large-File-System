#include "file_op.h"
#include "common.h"

namespace skx{
	namespace LFS{
		FileOperation::FileOperation(const std::string &file_name,const int open_flags):fd_(-1),open_flags_(open_flags){
			file_name_=strdup(file_name.c_str());
		}
		
		FileOperation::~FileOperation(){
			if(fd>=0){
				::close(fd_);
			}
			
			if(file_name_!=NULL){
				free(file_name_);
				file_name_=NULL;
			}
		}
		
		int FileOperation::open_file(){
			if(fd_>0){
				close(fd_);
				fd_=-1;
			}
			
			fd_=::open(file_name_,open_flags_,OPEN_MODE);
			if(fd_<0){
				return -errno;
			}
			
			return fd_;
		}
		
		void FileOperation::close_file(){
			if(fd_<0) return ;
			
			::close(fd_);
			fd_=-1;
		}
		
		int64_t FileOperation::get_file_size(){
			int fd=check_file();
			
			if(fd_<0) return -1;
			
			struct stat statbuf;
			if(fstat(fd,&statbuf)!=0){
				return -1
			}
			
			return statbuf.st_size;
		}
		
		int FileOperation::check_file(){
			if(fd_<0){
				fd_=open_file();
			}
			
			return fd_;
		}
		
		int FileOperation::ftruncate_file(const int64_t length){
			int fd = check_file();
			
			if(fd<0) return fd;
			
			return ftruncate(fd,length);
		}
		
		int FileOperation::seek_file(const int64_t offset){
			int fd = check_file();
			
			if(fd<0) return fd;
			
			return lseek(fd,offset,SEEK_SET);
		}
		
		int FileOperation::flush_file(){
			if(open_flags_&O_SYNC){ //若以同步方式则没必要flush
				return 0;
			}
			
			int fd=check_file();
			if(fd<0) return fd;
			
			return fsync(fd);
		}
		
		int FileOperation::unlink_file(){ //删除文件
			close_file();
			return ::unlink(file_name_);
		}
	}
}