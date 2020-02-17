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

const char	*get_ehdr_type_str(uint16_t e_type) {
	switch (e_type) {
		case ET_NONE:	return "NONE";
		case ET_REL:	return "REL";
		case ET_EXEC:	return "EXEC";
		case ET_DYN:	return "DYN";
		case ET_CORE:	return "CORE";
		default:	return "UNKNOWN";
	}
}

const char	*get_shdr_type_str(uint32_t sh_type) {
	switch (sh_type) {
		case SHT_NULL:		return "NULL";
		case SHT_PROGBITS:	return "PROGBITS";
		case SHT_SYMTAB:	return "SYMTAB";
		case SHT_STRTAB:	return "STRTAB";
		case SHT_RELA:		return "RELA";
		case SHT_HASH:		return "HASH";
		case SHT_DYNAMIC:	return "DYNAMIC";
		case SHT_NOTE:		return "NOTE";
		case SHT_NOBITS:	return "NOBITS";
		case SHT_REL:		return "REL";
		case SHT_SHLIB:		return "SHLIB";
		case SHT_DYNSYM:	return "DYNSYM";
		default:		return "UNKNOWN";
	}
}

void	debug_print_program_header(struct s_woody *woody) {
	Elf64_Phdr phdr;
	char  buff_flags_str[4];
	buff_flags_str[3] = 0;

	printf("There are %hu program headers, starting at offset %#lx:\n\n", woody->ehdr.e_phnum, woody->ehdr.e_phoff);
	printf("Program Headers:\n");
	printf("  Type           Offset             VirtAddr           PhysAddr\n");
	printf("                 FileSiz            MemSiz              Flags  Align\n");
	for (uint16_t i = 0; i < woody->ehdr.e_phnum; i++) {
		read_program_header(woody, i, &phdr);
		printf("  %-14s 0x%016lx 0x%016lx 0x%016lx\n", get_phdr_type_str(phdr.p_type), phdr.p_offset, phdr.p_vaddr, phdr.p_paddr);
		printf("                 0x%016lx 0x%016lx  %s    %#lx\n", phdr.p_filesz, phdr.p_memsz, get_phdr_flags_str(phdr.p_flags, buff_flags_str), phdr.p_align);
	}
}

void	debug_print_section_header(struct s_woody *woody) {
	Elf64_Shdr shdr;
	Elf64_Shdr sh_strtab;
	get_section_name_string_table(woody, &sh_strtab);

	printf("There are %hu section headers, starting at offset %#lx:\n\n", woody->ehdr.e_shnum, woody->ehdr.e_shoff);
	printf("Section Headers:\n");
	printf("  [Nr] Name              Type             Address           Offset\n");
	printf("       Size              EntSize          Flags  Link  Info  Align\n");
	for (uint16_t i = 0; i < woody->ehdr.e_shnum; i++) {
		read_section_header(woody, i, &shdr);
		printf("  [%2hu] %-16s  %-16s %016lx  %08lx\n", i, woody->map_elf_file + sh_strtab.sh_offset + shdr.sh_name, get_shdr_type_str(shdr.sh_type), shdr.sh_addr, shdr.sh_offset);
		printf("       %016lx  %016lx   ?       %-5u %-5u %-lu\n", shdr.sh_size, shdr.sh_entsize, shdr.sh_link, shdr.sh_info, shdr.sh_addralign);
	}
}

void	debug_print_headers(struct s_woody *woody) {
	printf("Elf file type is %s\n", get_ehdr_type_str(woody->ehdr.e_type));
	printf("Entry point %#lx\n\n", woody->ehdr.e_entry);
	debug_print_program_header(woody);
	printf("\n\n\n");
	debug_print_section_header(woody);
}

