#include "woody_woodpacker.h"

bool	read_elf_header(struct s_woody *woody) {
	if (woody->st_size < sizeof(Elf64_Ehdr)) {
		dprintf(STDERR_FILENO, "woody_woodpacker: not an ELF file (file too small to contain ELF header)\n");
		return (false);
	}
	woody->ehdr = *(Elf64_Ehdr *)woody->map_elf_file;

	if (*(uint32_t *)woody->ehdr.e_ident != *(uint32_t *)ELFMAG) {
		dprintf(STDERR_FILENO, "woody_woodpacker: not an ELF file (wrong magic number)\n");
		return (false);
	}

	if (woody->ehdr.e_ident[EI_CLASS] != ELFCLASS64) {
		dprintf(STDERR_FILENO, "woody_woodpacker: ELF Class not supported\n");
		return (false);
	}

	if (woody->ehdr.e_ident[EI_DATA] == ELFDATA2MSB) {
		woody->endian = E_BIG_ENDIAN;
	}
	else {
		if (woody->ehdr.e_ident[EI_DATA] != ELFDATA2LSB)
			dprintf(STDOUT_FILENO, "woody_woodpacker: warning: unknown ELF Data. Use little endian by default\n");
		woody->endian = E_LITTLE_ENDIAN;
	}
	return (true);
}
