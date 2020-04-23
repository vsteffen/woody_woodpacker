#include "woody_woodpacker.h"

void	read_elf_header(struct s_woody *woody) {
	if ((size_t)woody->bin_st.st_size < sizeof(Elf64_Ehdr)) {
		dprintf(STDERR_FILENO, "%s: not an ELF file (file too small to contain ELF header)\n", woody->woody_name);
		exit_clean(woody, EXIT_FAILURE);
	}
	woody->ehdr = *(Elf64_Ehdr *)woody->bin_map;

	if (*(uint32_t *)woody->ehdr.e_ident != *(uint32_t *)ELFMAG) {
		dprintf(STDERR_FILENO, "%s: not an ELF file (wrong magic number)\n", woody->woody_name);
		exit_clean(woody, EXIT_FAILURE);
	}

	if (woody->ehdr.e_ident[EI_CLASS] != ELFCLASS64) {
		dprintf(STDERR_FILENO, "%s: ELF Class not supported\n", woody->woody_name);
		exit_clean(woody, EXIT_FAILURE);
	}

	if (woody->ehdr.e_ident[EI_DATA] == ELFDATA2MSB) {
		#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
			woody->reverse_endian = true;
		#else
			woody->reverse_endian = false;
		#endif
	}
	else if (woody->ehdr.e_ident[EI_DATA] == ELFDATA2LSB) {
		#if __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
			woody->reverse_endian = true;
		#else
			woody->reverse_endian = false;
		#endif
	}
	else {
		woody->reverse_endian = false;
	}

	if (woody->ehdr.e_type != ET_EXEC && woody->ehdr.e_type != ET_DYN) {
		dprintf(STDERR_FILENO, "%s: wrong ELF type\n", woody->woody_name);
		exit_clean(woody, EXIT_FAILURE);
	}

	if (woody->reverse_endian) {
		woody->ehdr.e_type 	=  BSWAP16(woody->ehdr.e_type);
		woody->ehdr.e_machine 	=  BSWAP16(woody->ehdr.e_machine);
		woody->ehdr.e_version 	=  BSWAP32(woody->ehdr.e_version);
		woody->ehdr.e_entry 	=  BSWAP64(woody->ehdr.e_entry);
		woody->ehdr.e_phoff 	=  BSWAP64(woody->ehdr.e_phoff);
		woody->ehdr.e_shoff 	=  BSWAP64(woody->ehdr.e_shoff);
		woody->ehdr.e_flags 	=  BSWAP32(woody->ehdr.e_flags);
		woody->ehdr.e_ehsize 	=  BSWAP16(woody->ehdr.e_ehsize);
		woody->ehdr.e_phentsize =  BSWAP16(woody->ehdr.e_phentsize);
		woody->ehdr.e_phnum 	=  BSWAP16(woody->ehdr.e_phnum);
		woody->ehdr.e_shentsize =  BSWAP16(woody->ehdr.e_shentsize);
		woody->ehdr.e_shnum 	=  BSWAP16(woody->ehdr.e_shnum);
		woody->ehdr.e_shstrndx 	=  BSWAP16(woody->ehdr.e_shstrndx);
	}
}

void	read_program_header(struct s_woody *woody, uint16_t index, Elf64_Phdr *phdr) {
	if (index >= woody->ehdr.e_phentsize) {
		dprintf(STDERR_FILENO, "%s: wrong index of phdr\n", woody->woody_name);
		exit_clean(woody, EXIT_FAILURE);
	}
	*phdr = *(Elf64_Phdr *)(woody->bin_map + woody->ehdr.e_phoff + woody->ehdr.e_phentsize * index);
	if (woody->reverse_endian) {
		phdr->p_type	= BSWAP32(phdr->p_type);
		phdr->p_flags	= BSWAP32(phdr->p_flags);
		phdr->p_offset	= BSWAP64(phdr->p_offset);
		phdr->p_vaddr	= BSWAP64(phdr->p_vaddr);
		phdr->p_paddr	= BSWAP64(phdr->p_paddr);
		phdr->p_filesz	= BSWAP64(phdr->p_filesz);
		phdr->p_memsz	= BSWAP64(phdr->p_memsz);
		phdr->p_align	= BSWAP64(phdr->p_align);
	}
}

void	read_section_header(struct s_woody *woody, uint16_t index, Elf64_Shdr *shdr) {
	if (index >= woody->ehdr.e_shentsize) {
		dprintf(STDERR_FILENO, "%s: wrong index of shdr\n", woody->woody_name);
		exit_clean(woody, EXIT_FAILURE);
	}
	*shdr = *(Elf64_Shdr *)(woody->bin_map + woody->ehdr.e_shoff + woody->ehdr.e_shentsize * index);
	if (woody->reverse_endian) {
		shdr->sh_name		= BSWAP32(shdr->sh_name);
		shdr->sh_type		= BSWAP32(shdr->sh_type);
		shdr->sh_flags		= BSWAP64(shdr->sh_flags);
		shdr->sh_addr		= BSWAP64(shdr->sh_addr);
		shdr->sh_offset		= BSWAP64(shdr->sh_offset);
		shdr->sh_size		= BSWAP64(shdr->sh_size);
		shdr->sh_link		= BSWAP32(shdr->sh_link);
		shdr->sh_info		= BSWAP32(shdr->sh_info);
		shdr->sh_addralign	= BSWAP64(shdr->sh_addralign);
		shdr->sh_entsize	= BSWAP64(shdr->sh_entsize);
	}
}
