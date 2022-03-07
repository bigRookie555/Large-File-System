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
		
		int FileOperation::pread_file(char *buf,const int32_t nbytes,const int64_t offset){
			int32_t left=nbytes;
			int64_t read_offset=offset;
			char *p_tmp=buf;
			int32_t read_len=0;
			
			int i=0;
			
			while(left>0){
				++i;
				
				if(i>=MAX_DISK_TIMES) break;
				
				if(check_files()<0) return -errno;
				
				read_len=::pread64(fd_,p_tmp,left,read_offset);
				
				if(read_len<0){
					read_len=-errno;
					
					if(-read_len==EINTR||-read_len==EAGAIN){
						continue;
					}
					else if(-read_len==EBADF){
						fd_=-1;
						continue;
					}
					else return read_len;
				}
				else if(read_len==0) break;
				
				left-=read_len;
				p_tmp+=read_len;
				read_offset+=read_len;
			}
			
			if(left) return EXIT_DISK_OPER_INCOMPLETE;
			
			return TFS_SUCCESS;
		}
		
		int FileOperation::pwrite_file(const char *buf,const int32_t nbytes,const int64_t offset){
			int32_t left=nbytes;
			int64_t write_offset=offset;
			char *p_tmp=buf;
			int32_t write_len=0;
			
			int i=0;
			
			while(left>0){
				++i;
				
				if(i>=MAX_DISK_TIMES) break;
				
				if(check_files()<0) return -errno;
				
				write_len=::pwrite64(fd_,p_tmp,left,write_offset);
				
				if(write_len<0){
					write_len=-errno; //防止多线程环境将errno更改
					
					if(-write_len==EINTR||-write_len==EAGAIN){
						continue;
					}
					else if(-write_len==EBADF){
						fd_=-1;
						continue;
					}
					else return write_len;
				}
				
				left-=write_len;
				p_tmp+=write_len;
				write_offset+=write_len;
			}
			
			if(left) return EXIT_DISK_OPER_INCOMPLETE;
			
			return TFS_SUCCESS;
		}
		
		int FileOperation::write_file(const char *buf,const int32_t nbytes){
			int32_t left=nbytes;
			char *p_tmp=buf;
			int32_t write_len=0;
			
			while(left>0){
				++i;
				
				if(i>=MAX_DISK_TIMES) break;
				
				if(check_files()<0) return -errno;
				
				write_len=::write(fd_,p_tmp,left);
				
				if(write_len<0){
					write_len=-errno; //防止多线程环境将errno更改
					
					if(-write_len==EINTR||-write_len==EAGAIN){
						continue;
					}
					else if(-write_len==EBADF){
						fd_=-1;
						continue;
					}
					else return write_len;
				}
				
				left-=write_len;
				p_tmp+=write_len;
			}
			
			if(left) return EXIT_DISK_OPER_INCOMPLETE;
			
			return TFS_SUCCESS;
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