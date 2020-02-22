#include "woody_woodpacker.h"

void	check_headers_offset(struct s_woody *woody) {
	if (woody->ehdr.e_phoff + woody->ehdr.e_phentsize * woody->ehdr.e_phnum > (size_t)woody->bin_st.st_size
		| woody->ehdr.e_shoff + woody->ehdr.e_shentsize * woody->ehdr.e_shnum > (size_t)woody->bin_st.st_size)
	{
		dprintf(STDERR_FILENO, "woody_woodpacker: corrupted ELF file\n");
		exit_clean(woody, EXIT_FAILURE);
	}
}

void		get_shstrtab(struct s_woody *woody) {
	if (woody->ehdr.e_shstrndx == SHN_XINDEX) {
		Elf64_Shdr first_shdr;
		read_section_header(woody, 0, &first_shdr);
		read_section_header(woody, first_shdr.sh_link, &woody->shstrtab);
	}
	else
		read_section_header(woody, woody->ehdr.e_shstrndx, &woody->shstrtab);
}

uint16_t	get_index_section_with_name(struct s_woody *woody, char *section_name) {
	Elf64_Shdr tmp;
	for (uint16_t i = 0; i < woody->ehdr.e_shnum; i++) {
		read_section_header(woody, i, &tmp);
		if (strcmp(section_name, woody->bin_map + woody->shstrtab.sh_offset + tmp.sh_name) == 0)
			return (i);
	}
	return (-1);
}

uint16_t	get_index_segment_containing_section(struct s_woody *woody, Elf64_Shdr *section) {
	Elf64_Phdr tmp;
	for (uint16_t i = 0; i < woody->ehdr.e_phnum; i++) {
		read_program_header(woody, i, &tmp);
		if (tmp.p_vaddr <= section->sh_addr && section->sh_addr <= tmp.p_vaddr + tmp.p_memsz)
			return (i);
	}
	return (-1);
}

void		fill_new_section(struct s_woody *woody, Elf64_Shdr *new_section, Elf64_Shdr *shdr_bss) {
	Elf64_Shdr	shdr_text;
	uint16_t	index_shdr_text;

	index_shdr_text = get_index_section_with_name(woody, ".text");
	if (index_shdr_text == (uint16_t)-1) {
		dprintf(STDERR_FILENO, "woody_woodpacker: .text section not found\n");
		exit_clean(woody, EXIT_FAILURE);
	}
	read_section_header(woody, index_shdr_text, &shdr_text);

	new_section->sh_name = 0;
	new_section->sh_type = SHT_PROGBITS;
	new_section->sh_flags = SHF_EXECINSTR | SHF_ALLOC;
	new_section->sh_addr = shdr_bss->sh_addr + shdr_bss->sh_size;
	new_section->sh_offset = shdr_bss->sh_offset;
	new_section->sh_size = NEW_SECTION_SIZE;
	new_section->sh_link = 0;
	new_section->sh_info = 0;
	new_section->sh_addralign = shdr_text.sh_addralign;
	new_section->sh_entsize = 0;
}

void		insert_section_after_bss(struct s_woody *woody) {
	Elf64_Shdr	shdr_bss;
	uint16_t	index_shdr_bss;

	index_shdr_bss = get_index_section_with_name(woody, ".bss");
	if (index_shdr_bss == (uint16_t)-1) {
		dprintf(STDERR_FILENO, "woody_woodpacker: .bss section not found\n");
		exit_clean(woody, EXIT_FAILURE);
	}
	read_section_header(woody, index_shdr_bss, &shdr_bss);

	modify_ehdr(woody, &shdr_bss);
	modify_phdr_bss(woody, &shdr_bss);
	modify_shdr_pushed_by_new_section(woody, &shdr_bss, index_shdr_bss);

	fill_new_section(woody, &woody->new_section, &shdr_bss);

	save_new_elf_file(woody, &shdr_bss, index_shdr_bss);
}
