#ifndef WOODY_WOODPACKER_H
# define WOODY_WOODPACKER_H

# include "libft.h"

# include <sys/mman.h>
# include <fcntl.h>
# include <sys/stat.h>
# include <errno.h>

# define STR_IMPL_(x) #x
# define STR(x) STR_IMPL_(x)
# define ERROR_STR(x) __FILE__ ":" STR(__LINE__) ": " x
# define ERROR(x) perror(ERROR_STR(x))

# define USAGE "Usage: woody_woodpacker elf_file [key]\n"

typedef enum {false, true} bool;

typedef struct	s_woody {
	int	fd;
	size_t	st_size;
	void	*map_elf_file;
}		t_woody;

#endif
