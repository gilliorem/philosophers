NAME = philo

CC = cc
CFLAGS = -Wall -Werror -Wextra -pthread #-fsanitize=thread
INCLUDES = -Iinclude

SRC_DIR = src
OBJ_DIR = obj

SRC = clean.c init.c m_action.c monitor.c parsing.c \
	  philo.c utils.c libft.c main.c

OBJ = $(addprefix $(OBJ_DIR)/,$(SRC:.c=.o))

all: $(NAME)

$(NAME): $(OBJ)
	$(CC) $(CFLAGS) $(INCLUDES) $(OBJ) -o $(NAME)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(OBJ_DIR)
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

clean:
	rm -rf $(OBJ_DIR)

fclean: clean
	rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re
