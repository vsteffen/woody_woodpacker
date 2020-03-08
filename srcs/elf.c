#include "woody_woodpacker.h"

void	check_headers_offset(struct s_woody *woody) {
	if (woody->ehdr.e_phoff + woody->ehdr.e_phentsize * woody->ehdr.e_phnum > (size_t)woody->bin_st.st_size \
		|| woody->ehdr.e_shoff + woody->ehdr.e_shentsize * woody->ehdr.e_shnum > (size_t)woody->bin_st.st_size)
	{
		dprintf(STDERR_FILENO, "%s: corrupted ELF file\n", woody->woody_name);
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
	Elf64_Shdr	tmp;
	void		*end_of_str;

	for (uint16_t i = 0; i < woody->ehdr.e_shnum; i++) {
		read_section_header(woody, i, &tmp);
		if (woody->shstrtab.sh_offset > (size_t)woody->bin_st.st_size) {
			dprintf(STDERR_FILENO, "%s: corrupted ELF file (wrong shstrtab.sh_offset)\n", woody->woody_name);
			exit_clean(woody, EXIT_FAILURE);
		}
		end_of_str = memchr(woody->bin_map + woody->shstrtab.sh_offset, 0, (size_t)woody->bin_st.st_size - woody->shstrtab.sh_offset);
		if (!end_of_str) {
			dprintf(STDERR_FILENO, "%s: corrupted ELF file (wrong sh_name offset)\n", woody->woody_name);
			exit_clean(woody, EXIT_FAILURE);
		}
		if (ft_strcmp(section_name, woody->bin_map + woody->shstrtab.sh_offset + tmp.sh_name) == 0)
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

void		fill_new_section(struct s_woody *woody, Elf64_Shdr *new_section, Elf64_Shdr *shdr_last) {
	new_section->sh_name = 0;
	new_section->sh_type = SHT_PROGBITS;
	new_section->sh_flags = SHF_EXECINSTR | SHF_ALLOC;
	new_section->sh_addr = shdr_last->sh_addr + shdr_last->sh_size; // Push bc shdr_last will be at least SHT_PROGBITS
	new_section->sh_offset = shdr_last->sh_offset + shdr_last->sh_size;
	new_section->sh_size = BYTECODE_SIZE + woody->key.length;
	new_section->sh_link = 0;
	new_section->sh_info = 0;
	new_section->sh_addralign = 1;
	new_section->sh_entsize = 0;
}

uint16_t	get_index_last_shdr_in_phdr_bss(struct s_woody *woody, uint16_t index_shdr_bss, Elf64_Phdr *phdr_bss) {
	uint64_t	phdr_bss_last_byte = phdr_bss->p_vaddr + phdr_bss->p_memsz;
	Elf64_Shdr	tmp;

	for (uint16_t i = index_shdr_bss; i < woody->ehdr.e_shnum; i++) {
		read_section_header(woody, i, &tmp);
		if (tmp.sh_addr + tmp.sh_size == phdr_bss_last_byte)
			return (i);
	}
	return (-1);
}

void		insert_section_after_bss(struct s_woody *woody) {
	Elf64_Shdr	shdr_last;
	Elf64_Shdr	shdr_bss;
	Elf64_Shdr	shdr_text;
	Elf64_Phdr	phdr_bss;
	uint16_t	index_shdr_bss;
	uint16_t	index_shdr_text;
	uint16_t	index_phdr_bss;
	uint16_t	index_shdr_last;

	index_shdr_bss = get_index_section_with_name(woody, ".bss");
	if (index_shdr_bss == (uint16_t)-1) {
		dprintf(STDERR_FILENO, "%s: .bss section not found\n", woody->woody_name);
		exit_clean(woody, EXIT_FAILURE);
	}
	read_section_header(woody, index_shdr_bss, &shdr_bss);

	index_phdr_bss = get_index_segment_containing_section(woody, &shdr_bss);
	if (index_phdr_bss == (uint16_t)-1) {
		dprintf(STDERR_FILENO, "%s: .bss section not mapped (?)\n", woody->woody_name);
		exit_clean(woody, EXIT_FAILURE);
	}
	read_program_header(woody, index_phdr_bss, &phdr_bss);

	index_shdr_last = get_index_last_shdr_in_phdr_bss(woody, index_shdr_bss, &phdr_bss);
	if (index_shdr_last == (uint16_t)-1) {
		dprintf(STDERR_FILENO, "%s: last section of phdr bss not found\n", woody->woody_name);
		exit_clean(woody, EXIT_FAILURE);
	}
	read_section_header(woody, index_shdr_last, &shdr_last);

	if (shdr_last.sh_type == SHT_NOBITS)
		woody->new_section_and_padding_size = BYTECODE_SIZE + woody->key.length + shdr_last.sh_size;
	else
		woody->new_section_and_padding_size = BYTECODE_SIZE + woody->key.length;

	index_shdr_text = get_index_section_with_name(woody, ".text");
	if (index_shdr_text == (uint16_t)-1) {
		dprintf(STDERR_FILENO, "%s: .text section not found\n", woody->woody_name);
		exit_clean(woody, EXIT_FAILURE);
	}
	read_section_header(woody, index_shdr_text, &shdr_text);
	// Check if old entrypoint + size .text are valid
	if (shdr_text.sh_offset + shdr_text.sh_size > (size_t)woody->bin_st.st_size) {
		dprintf(STDERR_FILENO, "%s: corrupted binary (wrong .text section size)\n", woody->woody_name);
		exit_clean(woody, EXIT_FAILURE);
	}

	modify_shdr_last(woody, &shdr_last, index_shdr_last); // Must be done before fill_new_section

	fill_new_section(woody, &woody->new_section, &shdr_last);
	woody->new_entry = shdr_last.sh_addr + shdr_last.sh_size;

	modify_ehdr(woody);
	modify_phdr_bss(woody, &phdr_bss, index_phdr_bss);
	modify_phdr_text(woody, &shdr_text);
	modify_shdr_pushed_by_new_section(woody, index_shdr_last);

	xor_cipher(woody->key.raw, woody->key.length, woody->bin_map + shdr_text.sh_offset, shdr_text.sh_size);

	save_new_elf_file(woody, &shdr_last, index_shdr_last);
}
