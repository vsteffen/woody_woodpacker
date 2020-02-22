#include "woody_woodpacker.h"

void	write_uint16(struct s_woody *woody, uint16_t *addr, uint16_t value) {
	if (woody->reverse_endian)
		*addr = BSWAP16(value);
	else
		*addr = value;
}

void	write_uint32(struct s_woody *woody, uint32_t *addr, uint32_t value) {
	if (woody->reverse_endian)
		*addr = BSWAP32(value);
	else
		*addr = value;
}

void	write_uint64(struct s_woody *woody, uint64_t *addr, uint64_t value) {
	if (woody->reverse_endian)
		*addr = BSWAP64(value);
	else
		*addr = value;
}
