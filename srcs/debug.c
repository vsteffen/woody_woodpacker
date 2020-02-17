#include "woody_woodpacker.h"

const char	*get_phdr_type_str(uint32_t p_type) {
	switch (p_type) {
		case PT_NULL:		return "NULL";
		case PT_LOAD:		return "LOAD";
		case PT_DYNAMIC:	return "DYNAMIC";
		case PT_INTERP:		return "INTERP";
		case PT_NOTE:		return "NOTE";
		case PT_SHLIB:		return "SHLIB";
		case PT_PHDR:		return "PHDR";
		case PT_TLS:		return "TLS";
		case PT_GNU_EH_FRAME:	return "GNU_EH_FRAME";
		case PT_GNU_STACK:	return "GNU_STACK";
		case PT_GNU_RELRO:	return "GNU_RELRO";
		default:		return "UNKNOWN";
	}
}

const char	*get_phdr_flags_str(uint32_t flags, char buff[4]) {
	buff[0] = (flags & PF_R) ? 'R' : ' ';
	buff[1] = (flags & PF_W) ? 'W' : ' ';
	buff[2] = (flags & PF_X) ? 'A' : ' ';
	return buff;
}

void	debug_print_program_headers(struct s_woody *woody) {
	if (woody->ehdr.e_phoff + woody->ehdr.e_phentsize * woody->ehdr.e_phnum > woody->st_size) {
		dprintf(STDERR_FILENO, "woody_woodpacker: corrupted ELF file\n");
		exit_clean(woody, EXIT_FAILURE);
	}

	Elf64_Phdr *phdr;
	char  buff_flags_str[4];
	buff_flags_str[3] = 0;
	uint16_t i = 0;
	printf("Program Headers:\n");
	printf("  %-14s %-18s %-18s %-18s\n", "Type", "Offset", "VirtAddr", "PhysAddr");
	printf("  %14s %-18s %-18s %-18s\n", " ", "FileSiz", "MemSiz", " Flags  Align");
	while (i < woody->ehdr.e_phnum) {
		phdr = woody->map_elf_file + woody->ehdr.e_phoff + woody->ehdr.e_phentsize * i;
		printf("  %-14s 0x%016lx 0x%016lx 0x%016lx\n", get_phdr_type_str(phdr->p_type), phdr->p_offset, phdr->p_vaddr, phdr->p_paddr);
		printf("  %14s 0x%016lx 0x%016lx  %s    %#lx\n", " ", phdr->p_filesz, phdr->p_memsz, get_phdr_flags_str(phdr->p_flags, buff_flags_str), phdr->p_align);
		i++;
	}
}
