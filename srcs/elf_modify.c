#include "woody_woodpacker.h"

void	modify_phdr_bss(struct s_woody *woody, Elf64_Shdr *shdr_bss) {
	Elf64_Phdr	*fptr_phdr_bss;
	Elf64_Phdr	phdr_bss;
	uint16_t	index_phdr_bss;

	index_phdr_bss = get_index_segment_containing_section(woody, shdr_bss);
	if (index_phdr_bss == (uint16_t)-1) {
		dprintf(STDERR_FILENO, "woody_woodpacker: .bss section not mapped (?)");
		exit_clean(woody, EXIT_FAILURE);
	}
	read_program_header(woody, index_phdr_bss, &phdr_bss);

	fptr_phdr_bss = (Elf64_Phdr *)(woody->bin_map + woody->ehdr.e_phoff + woody->ehdr.e_phentsize * index_phdr_bss);
	write_uint64(woody, &fptr_phdr_bss->p_filesz, phdr_bss.p_filesz + shdr_bss->sh_size + NEW_SECTION_SIZE); // Maybe remove padding
	write_uint64(woody, &fptr_phdr_bss->p_memsz, phdr_bss.p_memsz + NEW_SECTION_SIZE);
	write_uint32(woody, &fptr_phdr_bss->p_flags, PF_R | PF_W | PF_X);
}

void	modify_ehdr(struct s_woody *woody, Elf64_Shdr *shdr_bss) {
	Elf64_Ehdr	*fptr_ehdr;

	fptr_ehdr = (Elf64_Ehdr *)woody->bin_map;
	write_uint16(woody, &fptr_ehdr->e_shnum, woody->ehdr.e_shnum + 1);
	write_uint64(woody, &fptr_ehdr->e_shoff, woody->ehdr.e_shoff + NEW_SECTION_SIZE + shdr_bss->sh_size);
	write_uint16(woody, &fptr_ehdr->e_shstrndx, woody->ehdr.e_shstrndx + 1);
}

void	modify_shdr_pushed_by_new_section(struct s_woody *woody, Elf64_Shdr *shdr_bss, uint16_t index_shdr_bss) {
	Elf64_Shdr tmp;
	Elf64_Shdr *fptr_shdr_tmp;

	for (uint16_t index = index_shdr_bss + 1; index < woody->ehdr.e_shnum; index++) {
		read_section_header(woody, index, &tmp);
		if (tmp.sh_type == SHT_NOBITS)
			continue ;
		fptr_shdr_tmp = (Elf64_Shdr *)(woody->bin_map + woody->ehdr.e_shoff + woody->ehdr.e_shentsize * index);
		if (tmp.sh_type == SHT_SYMTAB) {
			write_uint32(woody, &fptr_shdr_tmp->sh_link, tmp.sh_link + 1);
		}
		write_uint64(woody, &fptr_shdr_tmp->sh_offset, tmp.sh_offset + NEW_SECTION_SIZE + shdr_bss->sh_size);
	}
}
