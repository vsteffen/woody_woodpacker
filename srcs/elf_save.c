#include "woody_woodpacker.h"

static void *find_pattern32(void *addr, size_t size, uint32_t pattern, uint8_t opcode_size) {
	for (size_t i = 0; i < size - sizeof(pattern); i++) {
		if (*(uint32_t *)(addr + i) == pattern)
			return (addr + i + opcode_size);
	}
	return (NULL);
}

static __attribute__((unused)) void *find_pattern64(void *addr, size_t size, uint64_t pattern, uint8_t opcode_size) {
	for (size_t i = 0; i < size - sizeof(pattern); i++) {
		if (*(uint64_t *)(addr + i) == pattern)
			return (addr + i + opcode_size);
	}
	return (NULL);
}

void	save_new_section(struct s_woody *woody, int new_bin_fd, Elf64_Shdr *shdr_last) {
	static char bytecode[] = BYTECODE;

	// Padding for BSS section
	if (shdr_last->sh_type == SHT_NOBITS) {
		printf("debug: shdr_last_offset_adjustment = %#zx\n", woody->shdr_last_offset_adjustment);
		size_t size_to_write = shdr_last->sh_size + woody->shdr_last_offset_adjustment;
		char padding_zero[4096];
		memset(padding_zero, 0, sizeof(padding_zero));
		while (true) {
			if (size_to_write / sizeof(padding_zero) < 1) {
				if (write(new_bin_fd, padding_zero, size_to_write) == -1) {
					ERROR("write");
					exit_clean(woody, EXIT_FAILURE);
				}
				break ;
			}
			if (write(new_bin_fd, padding_zero, sizeof(padding_zero)) == -1) {
				ERROR("write");
				exit_clean(woody, EXIT_FAILURE);
			}
			size_to_write -= sizeof(padding_zero);
		}
	}

	// Write new section
	void *addr_pattern = find_pattern32((void *)bytecode, NEW_SECTION_SIZE, PATTERN_ENTRY_OLD, PATTERN_ENTRY_OLD_SIZE_OPCODE);
	if (!addr_pattern) {
		dprintf(STDERR_FILENO, "%s: old entry pattern not found\n", woody->woody_name);
		exit_clean(woody, EXIT_FAILURE);
	}
	*(int32_t *)addr_pattern = woody->ehdr.e_entry - (woody->new_entry  + (size_t)(addr_pattern - (void *)bytecode) + sizeof(PATTERN_ENTRY_OLD));
	if (write(new_bin_fd, bytecode, NEW_SECTION_SIZE) == -1) {
		ERROR("write");
		exit_clean(woody, EXIT_FAILURE);
	}
}

void	save_new_shdr(struct s_woody *woody, int new_bin_fd, Elf64_Shdr *new_section) {
	write_uint32(woody, &new_section->sh_type, new_section->sh_type);
	write_uint64(woody, &new_section->sh_flags, new_section->sh_flags);
	write_uint64(woody, &new_section->sh_addr, new_section->sh_addr);
	write_uint64(woody, &new_section->sh_offset, new_section->sh_offset);
	write_uint64(woody, &new_section->sh_size, new_section->sh_size);
	write_uint64(woody, &new_section->sh_addralign, new_section->sh_addralign);

	if (write(new_bin_fd, new_section, sizeof(Elf64_Shdr)) == -1) {
		ERROR("write");
		exit_clean(woody, EXIT_FAILURE);
	}
}

void	save_new_elf_file(struct s_woody *woody, Elf64_Shdr *shdr_last, uint16_t index_shdr_last) {
	int	new_bin_fd;
	size_t	written_map_bytes = 0;

	new_bin_fd = open(NEW_BIN_FILENAME, O_WRONLY | O_CREAT | O_TRUNC, woody->bin_st.st_mode);
	if (new_bin_fd == -1) {
		ERROR("open");
		exit_clean(woody, EXIT_FAILURE);
	}

	size_t size_to_write_before_decrypter = shdr_last->sh_offset - woody->shdr_last_offset_adjustment + (shdr_last->sh_type != SHT_NOBITS ? shdr_last->sh_size : 0);
	if (write(new_bin_fd, woody->bin_map, size_to_write_before_decrypter) == -1) { // FIRST PART
		ERROR("write");
		exit_clean(woody, EXIT_FAILURE);
	}
	written_map_bytes += size_to_write_before_decrypter;
	printf("debug: insert new section at %#zx\n", written_map_bytes);
	save_new_section(woody, new_bin_fd, shdr_last); // Write new section + .bss padding
	size_t size_to_write_before_new_shdr = (woody->ehdr.e_shoff + woody->ehdr.e_shentsize * (index_shdr_last + 1)) - written_map_bytes;
	if (write(new_bin_fd, woody->bin_map + written_map_bytes, size_to_write_before_new_shdr) == -1) { // SECOND PART
		ERROR("write");
		exit_clean(woody, EXIT_FAILURE);
	}
	written_map_bytes += size_to_write_before_new_shdr;
	save_new_shdr(woody, new_bin_fd, &woody->new_section); // Write new shdr
	if (write(new_bin_fd, woody->bin_map + written_map_bytes, woody->bin_st.st_size - written_map_bytes) == -1) { // THIRD PART
		ERROR("write");
		exit_clean(woody, EXIT_FAILURE);
	}
}


/*

+---------------+	<-------+
|      EHDR	|		|
+---------------+		|
|		|		|
|     PHDR_1 	|		|
|     PHDR_2 	|		|----	First part of bin_map
|     PHDR_N 	|		|
|		|		|
+---------------+		|
|		|		|
|    SECTION_1	|		|
|   SECTION_BSS	|	<-------+
|   NEW_SECTION	|	<-------+----	Write new section + .bss padding
|    SECTION_N	|		|
|		|		|
+---------------+		|----	Second part of bin_map
|		|		|
|     SHDR_1 	|		|
|    SHDR_BSS 	|		|
|    NEW_SHDR	|	<-------+----	Write new shdr
|     SHDR_N 	|		|
|		|		|----	Third part of bin_map
+---------------+	<-------+

*/
