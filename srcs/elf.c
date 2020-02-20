#include "woody_woodpacker.h"

void	check_headers_offset(struct s_woody *woody) {
	if (woody->ehdr.e_phoff + woody->ehdr.e_phentsize * woody->ehdr.e_phnum > woody->st_size
		| woody->ehdr.e_shoff + woody->ehdr.e_shentsize * woody->ehdr.e_shnum > woody->st_size)
	{
		dprintf(STDERR_FILENO, "woody_woodpacker: corrupted ELF file\n");
		exit_clean(woody, EXIT_FAILURE);
	}
}

void		get_shstrtab(struct s_woody *woody) {
	Elf64_Shdr first_shdr;
	read_section_header(woody, 0, &first_shdr);
	if (woody->ehdr.e_shstrndx == SHN_XINDEX)
		read_section_header(woody, first_shdr.sh_link, &woody->shstrtab);
	else
		read_section_header(woody, woody->ehdr.e_shstrndx, &woody->shstrtab);
}

uint16_t	get_index_section_with_name(struct s_woody *woody, char *section_name) {
	Elf64_Shdr tmp;
	for (uint16_t i = 0; i < woody->ehdr.e_shnum; i++) {
		read_section_header(woody, i, &tmp);
		if (strcmp(section_name, woody->map_elf_file + woody->shstrtab.sh_offset + tmp.sh_name) == 0)
			return (i);
	}
	return (-1);
}

uint16_t	get_index_segment_containing_section(struct s_woody *woody, Elf64_Shdr *section) {
	Elf64_Phdr tmp;
	for (uint16_t i = 0; i < woody->ehdr.e_phnum; i++) {
		read_program_header(woody, i, &tmp);
		printf("tmp.p_vaddr = %zu | tmp.p_vaddr + tmp.p_memsz = %zu | section->sh_addr = %zu\n", tmp.p_vaddr, tmp.p_vaddr + tmp.p_memsz, section->sh_addr);
		if (tmp.p_vaddr <= section->sh_addr && section->sh_addr <= tmp.p_vaddr + tmp.p_memsz)
			return (i);
	}
	return (-1);
}

void		insert_section_after_bss(struct s_woody *woody) {
	uint16_t i;
	Elf64_Shdr bss;
	Elf64_Phdr bss_load_segment;

	i = get_index_section_with_name(woody, ".bss");
	if (i == (uint16_t)-1) {
		dprintf(STDERR_FILENO, "woody_woodpacker: .bss section not found\n");
		exit_clean(woody, EXIT_FAILURE);
	}
	read_section_header(woody, i, &bss);
	i = get_index_segment_containing_section(woody, &bss);
	if (i == (uint16_t)-1) {
		dprintf(STDERR_FILENO, "woody_woodpacker: .bss section not mapped (?)");
		exit_clean(woody, EXIT_FAILURE);
	}
	read_program_header(woody, i, &bss_load_segment);
	printf("I = %hu\n", i);
}
