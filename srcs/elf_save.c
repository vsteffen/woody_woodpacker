#include "woody_woodpacker.h"

static void *find_pattern(void *addr, size_t size, uint64_t pattern) {
	for (size_t i = 0; i < size; i++) {
		if (*(uint64_t *)(addr + i) == pattern)
			return (addr + i);
	}
	return (NULL);
}

void	save_new_section(struct s_woody *woody, int new_bin_fd, Elf64_Shdr *shdr_bss) {
	static char bytecode[] = BYTECODE;

	// Padding for BSS section
	size_t size_to_write = shdr_bss->sh_size;
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

	// Write new section
	void *addr_pattern = find_pattern((void *)bytecode, sizeof(bytecode), PATTERN_ENTRY);
	if (!addr_pattern) {
		dprintf(STDERR_FILENO, "woody_woodpacker: entry pattern not found\n");
		exit_clean(woody, EXIT_FAILURE);
	}
	*(uint64_t *)addr_pattern = woody->ehdr.e_entry;
	if (write(new_bin_fd, bytecode, sizeof(bytecode)) == -1) {
		ERROR("write");
		exit_clean(woody, EXIT_FAILURE);
	}
}

void	save_new_shdr(struct s_woody *woody, int new_bin_fd, Elf64_Shdr *new_section) {
	// write_uint32(woody, &new_section->sh_name, new_section->sh_name);
	write_uint32(woody, &new_section->sh_type, new_section->sh_type);
	write_uint64(woody, &new_section->sh_flags, new_section->sh_flags);
	write_uint64(woody, &new_section->sh_addr, new_section->sh_addr);
	write_uint64(woody, &new_section->sh_offset, new_section->sh_offset);
	write_uint64(woody, &new_section->sh_size, new_section->sh_size);
	// write_uint32(woody, &new_section->sh_link, new_section->sh_link);
	// write_uint32(woody, &new_section->sh_info, new_section->sh_info);
	write_uint64(woody, &new_section->sh_addralign, new_section->sh_addralign);
	// write_uint64(woody, &new_section->sh_entsize, new_section->sh_entsize);

	if (write(new_bin_fd, new_section, sizeof(Elf64_Shdr)) == -1) {
		ERROR("write");
		exit_clean(woody, EXIT_FAILURE);
	}
}

void	save_new_elf_file(struct s_woody *woody, Elf64_Shdr *shdr_bss, uint16_t index_shdr_bss) {
	int	new_bin_fd;
	size_t	written_map_bytes = 0;

	new_bin_fd = open(NEW_BIN_FILENAME, O_WRONLY | O_CREAT | O_TRUNC, woody->bin_st.st_mode);
	if (new_bin_fd == -1) {
		ERROR("open");
		exit_clean(woody, EXIT_FAILURE);
	}

	size_t size_to_write_before_decrypter = shdr_bss->sh_offset;
	if (write(new_bin_fd, woody->bin_map, size_to_write_before_decrypter) == -1) { // FIRST PART
		ERROR("write");
		exit_clean(woody, EXIT_FAILURE);
	}
	written_map_bytes += size_to_write_before_decrypter;
	printf("debug: insert new section at %#zx\n", written_map_bytes);
	save_new_section(woody, new_bin_fd, shdr_bss);
	size_t size_to_write_before_new_section = (woody->ehdr.e_shoff + woody->ehdr.e_shentsize * (index_shdr_bss + 1)) - written_map_bytes;
	if (write(new_bin_fd, woody->bin_map + written_map_bytes, size_to_write_before_new_section) == -1) { // SECOND PART
		ERROR("write");
		exit_clean(woody, EXIT_FAILURE);
	}
	written_map_bytes += size_to_write_before_new_section;
	save_new_shdr(woody, new_bin_fd, &woody->new_section);
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
