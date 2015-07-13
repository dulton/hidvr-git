
#include "generic.h"


ssize_t get_file_size(const char* file_nane)
{
	struct stat file_stat;

	if(stat(file_nane, &file_stat) < 0){
		return -1;
	}
	return (ssize_t)(file_stat.st_size);
}

