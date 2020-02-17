#include "woody_woodpacker.h"

void	check_headers_offset(struct s_woody *woody) {
	if (woody->ehdr.e_phoff + woody->ehdr.e_phentsize * woody->ehdr.e_phnum > woody->st_size
		| woody->ehdr.e_shoff + woody->ehdr.e_shentsize * woody->ehdr.e_shnum > woody->st_size)
	{
		dprintf(STDERR_FILENO, "woody_woodpacker: corrupted ELF file\n");
		exit_clean(woody, EXIT_FAILURE);
	}
}

void		get_section_name_string_table(struct s_woody *woody, Elf64_Shdr *sh_strtab) {
	Elf64_Shdr first_shdr;
	read_section_header(woody, 0, &first_shdr);
	if (woody->ehdr.e_shstrndx == SHN_XINDEX)
		read_section_header(woody, first_shdr.sh_link, sh_strtab);
	else
		read_section_header(woody, woody->ehdr.e_shstrndx, sh_strtab);
}
