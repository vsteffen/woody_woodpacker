#ifndef WOODY_WOODPACKER_H
# define WOODY_WOODPACKER_H

# include "libft.h"

# include <elf.h>

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

typedef enum {E_BIG_ENDIAN, E_LITTLE_ENDIAN} e_endian;

typedef struct	s_woody {
	int		fd;
	size_t		st_size;
	void		*map_elf_file;
	Elf64_Ehdr	ehdr;
	e_endian	endian;
}		t_woody;


void	exit_clean(struct s_woody *woody, int exit_status);

bool	read_elf_header(struct s_woody *woody);

// DEBUG
void	debug_print_program_headers(struct s_woody *woody);


/*
##########################

\-----> Elf Header

#define EI_NIDENT 16

typedef struct {
	unsigned char e_ident[EI_NIDENT];
	uint16_t      e_type;
	uint16_t      e_machine;
	uint32_t      e_version;
	ElfN_Addr     e_entry;
	ElfN_Off      e_phoff;
	ElfN_Off      e_shoff;
	uint32_t      e_flags;
	uint16_t      e_ehsize;
	uint16_t      e_phentsize;
	uint16_t      e_phnum;
	uint16_t      e_shentsize;
	uint16_t      e_shnum;
	uint16_t      e_shstrndx;
} ElfN_Ehdr;

##########################

\-----> Elf Program Header

typedef struct {
	uint32_t   p_type;
	uint32_t   p_flags;
	Elf64_Off  p_offset;
	Elf64_Addr p_vaddr;
	Elf64_Addr p_paddr;
	uint64_t   p_filesz;
	uint64_t   p_memsz;
	uint64_t   p_align;
} Elf64_Phdr;

##########################
*/

#endif
