#include "woody_woodpacker.h"

void	exit_clean(struct s_woody *woody, int exit_status) {
	close(woody->fd);
	if (munmap(woody->map_elf_file, woody->st_size) == -1)
		ERROR("munmap");
	exit(exit_status);
}

bool	map_file(char *elf_filename, struct s_woody *woody) {
	struct stat	st;

	if ((woody->fd = open(elf_filename, O_RDONLY)) == -1) {
		ERROR("open");
		return (false);
	}

	if (fstat(woody->fd, &st) == -1) {
		ERROR("fstat");
		close(woody->fd);
		return (false);
	}
	woody->st_size = st.st_size;

	if ((woody->map_elf_file = mmap(NULL, st.st_size, PROT_READ | PROT_WRITE, MAP_PRIVATE, woody->fd, 0)) == MAP_FAILED) {
		ERROR("mmap");
		close(woody->fd);
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
	if (!read_elf_header(&woody))
		return (EXIT_FAILURE);

	debug_print_program_headers(&woody);

	exit_clean(&woody, EXIT_SUCCESS);
}
