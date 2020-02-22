#include "woody_woodpacker.h"

void	exit_clean(struct s_woody *woody, int exit_status) {
	close(woody->bin_fd);
	if (munmap(woody->bin_map, woody->bin_st.st_size) == -1)
		ERROR("munmap");
	exit(exit_status);
}

bool	map_file(char *elf_filename, struct s_woody *woody) {
	if ((woody->bin_fd = open(elf_filename, O_RDONLY)) == -1) {
		ERROR("open");
		return (false);
	}

	if (fstat(woody->bin_fd, &woody->bin_st) == -1) {
		ERROR("fstat");
		close(woody->bin_fd);
		return (false);
	}

	if ((woody->bin_map = mmap(NULL, woody->bin_st.st_size, PROT_READ | PROT_WRITE, MAP_PRIVATE, woody->bin_fd, 0)) == MAP_FAILED) {
		ERROR("mmap");
		close(woody->bin_fd);
		return (false);
	}
	return (true);
}

void	print_usage() {
	printf(USAGE);
}

int	main(int ac, char **av)
{
	struct s_woody	woody;

	if (ac < 2 || ac > 3) {
		print_usage();
		return (EXIT_FAILURE);
	}

	if (!map_file(av[1], &woody))
		return (EXIT_FAILURE);
	read_elf_header(&woody);
	check_headers_offset(&woody);
	get_shstrtab(&woody);

	insert_section_after_bss(&woody);
	// debug_print_headers(&woody);

	exit_clean(&woody, EXIT_SUCCESS);
}
