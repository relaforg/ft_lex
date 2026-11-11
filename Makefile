CC := cc
INCLUDES := -Iincludes
CFLAGS := -pedantic -std=c99 -D_POSIX_C_SOURCE=200809L -Werror -Wextra -Wall -MD $(INCLUDES)
DEBUG_CFLAGS := $(CFLAGS) -g3
NAME := ft_lex
LIBL_NAME := libl.a
BUILD_DIR := .build
DEBUG_DIR := .debug

VPATH := src
SRCS := main.c \
		vec.c \
		strbuf.c \
		lex_file.c \
		tree.c \
		tree_print.c \
		nfa.c \
		nfa_print.c \
		parser/pattern.c \
		parser/pcur.c \
		parser/parser.c \
		parser/cursor.c

OBJS = $(addprefix $(BUILD_DIR)/, $(SRCS:.c=.o))
DEPS = $(OBJS:.o=.d)
DEBUG_OBJS := $(SRCS:%.c=$(DEBUG_DIR)/%.o)

LIBL_SRCS := libl/main.c \
			 libl/yywrap.c
LIBL_OBJS = $(addprefix $(BUILD_DIR)/, $(LIBL_SRCS:.c=.o))
LIBL_DEPS = $(LIBL_OBJS:.o=.d)

all: $(NAME)

$(NAME): $(OBJS)
	$(CC) -o $(NAME) $(OBJS)

$(LIBL_NAME): $(LIBL_OBJS)
	ar rcs $(LIBL_NAME) $(LIBL_OBJS)

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

$(BUILD_DIR)/%.o: %.c | $(BUILD_DIR)
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) -o $@ -c $<

debug: $(NAME)_debug

$(NAME)_debug: $(DEBUG_OBJS)
	$(CC) -o $(NAME)_debug $(DEBUG_OBJS)

$(DEBUG_DIR):
	mkdir -p $(DEBUG_DIR)

$(DEBUG_DIR)/%.o: %.c | $(DEBUG_DIR)
	@mkdir -p $(@D)
	$(CC) $(DEBUG_CFLAGS) -o $@ -c $<

clean:
	rm -rf $(BUILD_DIR)
	rm -rf $(DEBUG_DIR)

fclean: clean
	rm -f $(NAME)
	rm -f $(NAME)_debug
	rm -f $(LIBL_NAME)

re: fclean all

.PHONY: all clean fclean re debug

-include $(DEPS) $(LIBL_DEPS)
