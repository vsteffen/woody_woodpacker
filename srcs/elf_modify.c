#include "woody_woodpacker.h"

void	modify_shdr_last(struct s_woody *woody, Elf64_Shdr *shdr_last, uint16_t index_shdr_last) {
	Elf64_Shdr shdr_prev_last;
	Elf64_Shdr *fptr_shdr_last;
	size_t shdr_last_offset;

	if (shdr_last->sh_type == SHT_NOBITS) {
		fptr_shdr_last = (Elf64_Shdr *)(woody->bin_map + woody->ehdr.e_shoff + woody->ehdr.e_shentsize * index_shdr_last);
		write_uint32(woody, &fptr_shdr_last->sh_type, SHT_PROGBITS);
		read_section_header(woody, index_shdr_last - 1, &shdr_prev_last);
		shdr_last_offset = shdr_prev_last.sh_offset + (shdr_last->sh_addr - shdr_prev_last.sh_addr);
		if (shdr_last_offset > shdr_last->sh_offset) {
			printf("debug: change shdr_last offset\n");
			write_uint64(woody, &fptr_shdr_last->sh_offset, shdr_last_offset);
			woody->shdr_last_offset_adjustment = shdr_last_offset - shdr_last->sh_offset;
			shdr_last->sh_offset = shdr_last_offset;
		}
		else
			woody->shdr_last_offset_adjustment = 0;
	}
}

void	modify_ehdr(struct s_woody *woody) {
	Elf64_Ehdr	*fptr_ehdr;

	fptr_ehdr = (Elf64_Ehdr *)woody->bin_map;
	write_uint16(woody, &fptr_ehdr->e_shnum, woody->ehdr.e_shnum + 1);
	write_uint64(woody, &fptr_ehdr->e_shoff, woody->ehdr.e_shoff + woody->new_section_and_padding_size + woody->shdr_last_offset_adjustment);
	write_uint16(woody, &fptr_ehdr->e_shstrndx, woody->ehdr.e_shstrndx + 1);
	printf("debug: new entry point at %#zx\n", woody->new_entry);
	write_uint64(woody, &fptr_ehdr->e_entry, woody->new_entry);
}

void	modify_phdr_bss(struct s_woody *woody, Elf64_Phdr *phdr_bss, uint16_t index_phdr_bss) {
	Elf64_Phdr	*fptr_phdr_bss;

	fptr_phdr_bss = (Elf64_Phdr *)(woody->bin_map + woody->ehdr.e_phoff + woody->ehdr.e_phentsize * index_phdr_bss);
	write_uint64(woody, &fptr_phdr_bss->p_filesz, phdr_bss->p_filesz + woody->new_section_and_padding_size + woody->shdr_last_offset_adjustment);
	write_uint64(woody, &fptr_phdr_bss->p_memsz, phdr_bss->p_memsz + BYTECODE_SIZE + woody->key.length);
	write_uint32(woody, &fptr_phdr_bss->p_flags, PF_R | PF_W | PF_X);
}

void	modify_phdr_text(struct s_woody *woody, Elf64_Shdr *shdr_text) {
	Elf64_Phdr	*fptr_phdr_text;
	uint16_t	index_phdr_text;

	index_phdr_text = get_index_segment_containing_section(woody, shdr_text);
	if (index_phdr_text == (uint16_t)-1) {
		dprintf(STDERR_FILENO, "%s: .bss section not mapped (?)\n", woody->woody_name);
		exit_clean(woody, EXIT_FAILURE);
	}

	fptr_phdr_text = (Elf64_Phdr *)(woody->bin_map + woody->ehdr.e_phoff + woody->ehdr.e_phentsize * index_phdr_text);
	write_uint32(woody, &fptr_phdr_text->p_flags, PF_R | PF_W | PF_X);
}

void	modify_shdr_pushed_by_new_section(struct s_woody *woody, uint16_t index_shdr_last) {
	Elf64_Shdr tmp;
	Elf64_Shdr *fptr_shdr_tmp;

	for (uint16_t index = index_shdr_last + 1; index < woody->ehdr.e_shnum; index++) {
		read_section_header(woody, index, &tmp);
		if (tmp.sh_type == SHT_NOBITS)
			continue ;
		fptr_shdr_tmp = (Elf64_Shdr *)(woody->bin_map + woody->ehdr.e_shoff + woody->ehdr.e_shentsize * index);
		if (tmp.sh_type == SHT_SYMTAB) {
			write_uint32(woody, &fptr_shdr_tmp->sh_link, tmp.sh_link + 1);
		}
		write_uint64(woody, &fptr_shdr_tmp->sh_offset, tmp.sh_offset + woody->new_section_and_padding_size + woody->shdr_last_offset_adjustment);
	}
}
