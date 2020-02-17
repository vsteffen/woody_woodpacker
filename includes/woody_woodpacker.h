#ifndef WOODY_WOODPACKER_H
# define WOODY_WOODPACKER_H

# include "libft.h"
# include "tools.h"

# include <elf.h>

# include <sys/mman.h>
# include <fcntl.h>
# include <sys/stat.h>
# include <errno.h>

# define USAGE "Usage: woody_woodpacker elf_file [key]\n"

typedef enum {false, true} bool;

typedef struct	s_woody {
	int		fd;
	size_t		st_size;
	void		*map_elf_file;
	Elf64_Ehdr	ehdr;
	bool		reverse_endian;
}		t_woody;


void	exit_clean(struct s_woody *woody, int exit_status);

void	check_headers_offset(struct s_woody *woody);

const char	*get_phdr_type_str(uint32_t p_type);
const char	*get_phdr_flags_str(uint32_t flags, char buff[4]);
const char	*get_ehdr_type_str(uint16_t e_type);
const char	*get_shdr_type_str(uint32_t sh_type);
void		get_section_name_string_table(struct s_woody *woody, Elf64_Shdr *sh_strtab);

void	read_elf_header(struct s_woody *woody);
void	read_program_header(struct s_woody *woody, uint16_t index, Elf64_Phdr *phdr);
void	read_section_header(struct s_woody *woody, uint16_t index, Elf64_Shdr *shdr);

// DEBUG
void	debug_print_headers(struct s_woody *woody);
void	debug_print_program_header(struct s_woody *woody);
void	debug_print_section_header(struct s_woody *woody);


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

\-----> Elf Section Header

typedef struct {
	uint32_t   sh_name;
	uint32_t   sh_type;
	uint64_t   sh_flags;
	Elf64_Addr sh_addr;
	Elf64_Off  sh_offset;
	uint64_t   sh_size;
	uint32_t   sh_link;
	uint32_t   sh_info;
	uint64_t   sh_addralign;
	uint64_t   sh_entsize;
} Elf64_Shdr;

##########################
*/

#endif
