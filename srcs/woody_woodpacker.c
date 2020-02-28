#include "woody_woodpacker.h"

void	exit_clean(struct s_woody *woody, int exit_status) {
	close(woody->bin_fd);
	if (woody->key.raw != NULL)
		free(woody->key.raw);
	if (munmap(woody->bin_map, woody->bin_st.st_size) == -1)
		ERROR("munmap");
	exit(exit_status);
}

bool	map_file(char *elf_filename, struct s_woody *woody) {
	if ((woody->bin_fd = open(elf_filename, O_RDONLY)) == -1) {
		ERROR("open");
		return (false);
	}

	if (syscall(__NR_fstat, woody->bin_fd, &woody->bin_st) == -1) {
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

void	gen_random_key(struct s_woody *woody, struct s_key *key) {
	long key_length;

	if ((key_length = syscall(__NR_getrandom, key->raw, KEY_DEFAULT_SIZE, 0)) <= -1) {
		ERROR("getrandom");
		exit_clean(woody, EXIT_FAILURE);
	}
	key->length = (size_t)key_length;
	printf("Key value: ");
	for (ssize_t i = 0; i < key_length; i++) {
		printf("%02hhX", key->raw[i]);
	}
	printf("\n");
}

int	main(int ac, char **av)
{
	struct s_woody	woody;

	if (ac < 2 || ac > 3) {
		printf(USAGE, av[0]);
		return (EXIT_FAILURE);
	}

	woody.key.raw = NULL;
	woody.woody_name = av[0];

	if (!map_file(av[1], &woody))
		return (EXIT_FAILURE);
	read_elf_header(&woody);
	check_headers_offset(&woody);
	get_shstrtab(&woody);

	if (ac == 2) {
		woody.key.raw = malloc(KEY_DEFAULT_SIZE);
		gen_random_key(&woody, &woody.key);
	}
	else {
		woody.key.raw = ft_strdup(av[2]);
		woody.key.length = ft_strlen(av[2]);
	}

	insert_section_after_bss(&woody);
	// debug_print_headers(&woody);

	exit_clean(&woody, EXIT_SUCCESS);
}
