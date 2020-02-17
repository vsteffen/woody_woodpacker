#ifndef TOOLS_H
# define TOOLS_H


# define STR_IMPL_(x) #x
# define STR(x) STR_IMPL_(x)

# define ERROR_STR(x) __FILE__ ":" STR(__LINE__) ": " x
# define ERROR(x) perror(ERROR_STR(x))

# define COUNT_OF(ptr) (sizeof(ptr) / sizeof((ptr)[0]))

# define BSWAP16(x) \
	((__uint16_t) ((((x) >> 8) & 0xff) | (((x) & 0xff) << 8)))

# define BSWAP32(x) \
	((((x) & 0xff000000u) >> 24) | (((x) & 0x00ff0000u) >> 8) \
	| (((x) & 0x0000ff00u) << 8) | (((x) & 0x000000ffu) << 24))

# define BSWAP64(x)				\
	((((x) & 0xff00000000000000ull) >> 56)	\
	| (((x) & 0x00ff000000000000ull) >> 40)	\
	| (((x) & 0x0000ff0000000000ull) >> 24)	\
	| (((x) & 0x000000ff00000000ull) >> 8)	\
	| (((x) & 0x00000000ff000000ull) << 8)	\
	| (((x) & 0x0000000000ff0000ull) << 24)	\
	| (((x) & 0x000000000000ff00ull) << 40)	\
	| (((x) & 0x00000000000000ffull) << 56))


#endif
