# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: vsteffen <marvin@42.fr>                    +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2016/02/06 20:52:05 by vsteffen          #+#    #+#              #
#    Updated: 2018/03/20 18:17:27 by vsteffen         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

PROJECT	= woody_woodpacker
NAME	= woody_woodpacker

NEW_BIN	= woody

CC 	=	/usr/bin/clang
RM 	=	/bin/rm
MAKE 	=	/usr/bin/make
MKDIR 	=	/bin/mkdir -p
AR 	=	/usr/bin/ar
RANLIB 	=	/usr/bin/ranlib
GIT	=	/usr/bin/git
NASM	=	/usr/bin/nasm
OBJCOPY	=	/usr/bin/objcopy
HEXDUMP	=	/usr/bin/hexdump

OBJ = $(patsubst %.c, $(OPATH)/%.o, $(SRC))

CFLAGS = -Wall -Wextra -Werror -g #-fsanitize=address

ifeq ($(with-asm),y)
	WITH-ASM:= with-asm
endif

LIB	=	$(ROOT)/lib
LIBSRCS	=	$(ROOT)/libsrcs
LIBFT 	=	$(LIBSRCS)/libft

ROOT  	=	$(shell /bin/pwd)
OPATH 	=	$(ROOT)/objs
CPATH 	=	$(ROOT)/srcs
LPATH	=	$(LIBFT)/libft.a
HPATH 	=	-I $(ROOT)/includes -I $(LIBFT)/includes

SRC =	woody_woodpacker.c \
	debug.c \
	elf.c \
	elf_read.c \
	elf_write.c \
	elf_modify.c \
	elf_save.c

ASMPATH		= $(ROOT)/asm
ASM_SRC		= $(ASMPATH)/unpacker.asm
ASM_OBJ		= $(ASMPATH)/unpacker.o
ASM_BYTECODE	= $(ASMPATH)/bytecode

PRE_CHECK_SUB_LIBFT	:= $(LIBFT)/Makefile
PRE_CHECK_SUB		:= $(PRE_CHECK_SUB_LIBFT)

PRE_CHECK_LIB_LIBFT 	:= $(LIBFT)/libft.a
PRE_CHECK_LIB		:= $(PRE_CHECK_LIB_LIBFT) $(PRE_CHECK_LIB_LULZ)

COMPILE	= no

OS	:= $(shell uname -s)

ifeq ($(OS),Darwin)
	NPROCS:=$(shell sysctl -n hw.ncpu)
else
	NPROCS:=$(shell grep -c ^processor /proc/cpuinfo)
endif


define PRINT_RED
	printf "\033[31m$(1)\033[0m"
endef

define PRINT_GREEN
	printf "\033[32m$(1)\033[0m"
endef

define PRINT_YELLOW
	printf "\033[33m$(1)\033[0m"
endef

define PRINT_STATUS
	printf '['
	$(if $(filter $(2),SUCCESS),$(call PRINT_GREEN,$(1)))
	$(if $(filter $(2),FAIL),$(call PRINT_RED,$(1)))
	$(if $(filter $(2),WARN),$(call PRINT_YELLOW,$(1)))
	$(if $(filter $(2),INFO),printf $(1))
	$(if $(filter $(3),-n),printf $(1),echo ']')
endef

.PHONY: all clean fclean re lib-clean lib-update
.SILENT: $(PRE_CHECK_SUB) $(PRE_CHECK_LIB) $(NAME) $(OPATH) $(ASM_OBJ) $(OPATH)/%.o clean fclean re lib-clean lib-update

all: $(NAME)

$(PRE_CHECK_SUB):
	@echo $(PROJECT)": Init submodule $(dir $@) ... "
	@$(GIT) submodule update --init --recursive > /dev/null  # can't directly redirect stdout on /dev/null cause of sync wait on Linux
	@printf $(PROJECT)": $(dir $@) "
	@$(call PRINT_STATUS,INITIALIZED,SUCCESS)

$(PRE_CHECK_LIB): $(PRE_CHECK_SUB)
	echo $(PROJECT)": Compile $@ ... " ; \
	if [ $@ = $(PRE_CHECK_LIB_LIBFT) ] ; then \
		if [ $(OS) = "Darwin" ] ; then \
			$(MAKE) -C $(LIBFT) $(WITH-ASM) -j$(NPROCS) > /dev/null ; \
		else \
			$(MAKE) -C $(LIBFT) -j$(NPROCS) ; \
		fi; \
	else \
		echo "Other libraries here" ; \
	fi; \
	printf $(PROJECT)": $@ " ; \
	$(call PRINT_STATUS,COMPILED,SUCCESS) ;


$(ASM_OBJ):
	$(NASM) -f elf64 $(ASM_SRC) -o $(ASM_OBJ)
	$(OBJCOPY) -O binary -j .text $(ASM_OBJ) $(ASM_BYTECODE)

$(NAME): $(PRE_CHECK_LIB) $(ASM_OBJ) $(OPATH) $(OBJ)
	$(if $(filter $(COMPILE),yes),echo ']')
	printf $(PROJECT)": Building $@ ... "
	$(CC) -o $@ $(CFLAGS) $(OBJ) $(LPATH) $(HPATH) $(ASM_OBJ)
	$(call PRINT_STATUS,DONE,SUCCESS)

$(OPATH)/%.o: $(CPATH)/%.c | $(PRE_CHECK_LIB)
	$(if $(filter $(COMPILE),no),@printf $(PROJECT)': Files compiling [')
	$(eval COMPILE := yes)
	@$(CC) $(CFLAGS) -c $< -o $@ $(HPATH) -DBYTECODE=\"`$(HEXDUMP) -v -e '"\\\\x" 1/1 "%02X"' $(ASM_BYTECODE)`\"
	@$(call PRINT_GREEN,.)

$(OPATH):
	echo $(PROJECT)": Creation of objects directory"
	$(MKDIR) $@

clean:
	$(RM) -Rf $(OPATH)
	$(RM) -f $(ASM_OBJ)
	echo $(PROJECT)": Objects cleaned "
	printf $(PROJECT)": $@ rule "
	$(call PRINT_STATUS,DONE,SUCCESS)

fclean: clean
	$(RM) -f $(NAME)
	$(RM) -f $(NEW_BIN)
	$(RM) -f $(ASM_BYTECODE)
	echo $(PROJECT)": executable clean"
	printf $(PROJECT)": $@ rule "
	$(call PRINT_STATUS,DONE,SUCCESS)

re: fclean
	$(MAKE) -C $(ROOT) -j$(NPROCS) -s

lib-clean:
	echo $(PROJECT)": cleaning libraries ..."
	-$(MAKE) -C $(LIBFT) fclean -j$(NPROCS) > /dev/null
	printf $(PROJECT)": $@ rule "
	$(call PRINT_STATUS,DONE,SUCCESS)

lib-update:
	echo $(PROJECT)": Update submodules ... "
	$(GIT) submodule update --init --recursive --remote > /dev/null
	printf $(PROJECT)": submodules "
	$(call PRINT_STATUS,UPDATED,SUCCESS)
