CC := cc
INCLUDES := -Iincludes
CFLAGS := -Werror -Wextra -Wall -MD $(INCLUDES)
DEBUG_CFLAGS := $(CFLAGS) -g3
NAME := ft_lex
BUILD_DIR := .build
DEBUG_DIR := .debug

VPATH := src
SRCS := main.c \
		vec.c \
		parser/parser.c \
		parser/cursor.c

OBJS = $(addprefix $(BUILD_DIR)/, $(SRCS:.c=.o))
DEPS = $(OBJS:.o=.d)
DEBUG_OBJS := $(SRCS:%.c=$(DEBUG_DIR)/%.o)

all: $(NAME)

$(NAME): $(OBJS)
	$(CC) -o $(NAME) $(OBJS)

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

re: fclean all

.PHONY: all clean fclean re debug

-include $(DEPS)
