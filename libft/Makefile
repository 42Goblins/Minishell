NAME = libft.a
CC = cc 
CFLAGS = -Wall -Wextra -Werror
AR	= ar rcs
RM = rm -f

OBJ_DIR = obj

INC = inc/

# libc functions
LIBC =	ft_atoi.c ft_bzero.c ft_calloc.c ft_isalnum.c ft_isalpha.c \
		ft_isascii.c ft_isdigit.c ft_isprint.c ft_memchr.c \
		ft_memcmp.c ft_memcpy.c ft_memmove.c ft_memset.c ft_strchr.c \
		ft_strdup.c ft_strlcat.c ft_strlcpy.c ft_strlen.c ft_strcmp.c \
		ft_strncmp.c ft_strnstr.c ft_strrchr.c ft_tolower.c ft_toupper.c 
# additional functions
ADDITIONALS =	ft_itoa.c ft_putchar_fd.c ft_putendl_fd.c ft_putnbr_fd.c ft_putstr_fd.c \
				ft_split.c ft_strjoin.c ft_strmapi.c ft_strtrim.c ft_substr.c ft_striteri.c
# bonus
BONUS =	ft_lstadd_back_bonus.c ft_lstadd_front_bonus.c ft_lstclear_bonus.c \
		ft_lstdelone_bonus.c ft_lstiter_bonus.c ft_lstlast_bonus.c \
		ft_lstmap_bonus.c ft_lstnew_bonus.c ft_lstsize_bonus.c

# ft_printf
FT_PRINTF = ft_printf.c put_c.c put_int.c put_s.c put_uint.c put_x.c put_p.c

#gnl
GNL = get_next_line_bonus.c get_next_line_utils_bonus.c

SRCS = ${LIBC} ${ADDITIONALS} ${FT_PRINTF} ${GNL} ${BONUS}
SRCS_B = ${SRCS} ${BONUS}

# transforme les chemins : obj/ft_*.o
vpath %.c src/libft src/ft_printf src/gnl src/bonus
OBJ = $(SRCS:%.c=$(OBJ_DIR)/%.o)
OBJ_B = $(SRCS_B:%.c=$(OBJ_DIR)/%.o)

# Colors
END		:= \033[0m
BOLD	:= \033[1m
GREEN	:= \033[32m
BLUE	:= \033[34m
PURPLE	:= \033[35m
YELLOW	:= \033[33m
CYAN	:= \033[36m

# Progress counter
TOTAL_FILES := $(words $(SRCS))
TOTAL_BONUS := $(words $(SRCS_B))
CURRENT_FILE = 0

all: ${NAME}

${NAME}: ${OBJ}
	@echo "${BOLD}${BLUE}📚 Creating library archive...${END}"
	@${AR} ${NAME} ${OBJ}
	@echo "${BOLD}${GREEN}✨ Library ${NAME} created successfully${END}"
	@echo "${BOLD}${CYAN}📊 Compiled ${TOTAL_FILES} files${END}"

bonus: ${OBJ_B}
	@$(eval TOTAL_FILES=$(shell echo $$(($(TOTAL_FILES)+$(TOTAL_BONUS)))))
	@echo "${BOLD}${BLUE}🎲 Building with bonus...${END}"
	@${AR} ${NAME} ${OBJ_B}
	@echo "${BOLD}${GREEN}✨ Bonus features added successfully${END}"
	@echo "${BOLD}${CYAN}📊 Total files compiled: ${TOTAL_FILES}${END}"

# règle magique pour obj/
$(OBJ_DIR)/%.o: %.c
	@mkdir -p $(dir $@)
	@$(eval CURRENT_FILE=$(shell echo $$(($(CURRENT_FILE)+1))))
	@printf "${BOLD}${PURPLE}⚡ [%2d/%2d] Compiling: %-20s ${END}" $(CURRENT_FILE) $(TOTAL_FILES) "$<"
	@${CC} ${CFLAGS} -I $(INC) -c $< -o $@
	@echo "${BOLD}${GREEN}✓${END}"

clean: 
	@echo "${BOLD}${YELLOW}🧹 Cleaning objects...${END}"
	@${RM} -r ${OBJ_DIR}
	@echo "${BOLD}${GREEN}✓ Clean complete${END}"

fclean: clean
	@echo "${BOLD}${YELLOW}🗑️  Removing library...${END}"
	@${RM} $(NAME)
	@echo "${BOLD}${GREEN}✨ All clean ✨${END}"

re: fclean all

.PHONY: all clean fclean re bonus