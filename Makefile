# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: dgeara <dgeara@student.42lausanne.ch>      +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2026/07/09 00:00:00 by cmauley           #+#    #+#              #
#    Updated: 2026/09/26 11:17:23 by dgeara           ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME		=	minishell

CC			=	cc
CFLAGS		=	-Wall -Wextra -Werror
CPPFLAGS	=	-Iinclude -Ilibft
RM			=	rm -rf

SRC_DIR		=	srcs
OBJ_DIR		=	objs
LIBFT_DIR	=	libft
LIBFT		=	$(LIBFT_DIR)/libft.a

SRCS		=	$(shell find $(SRC_DIR) -type f -name "*.c")
OBJS		=	$(SRCS:%.c=$(OBJ_DIR)/%.o)
DEPS		=	$(OBJS:.o=.d)

LDLIBS		=	-L$(LIBFT_DIR) -lft -lreadline -ltermcap


UNAME_S     :=  $(shell uname -s)

ifeq ($(UNAME_S), Darwin)
// need > brew install readline, first
READLINE_PREFIX :=  $(shell brew --prefix readline 2>/dev/null)
CPPFLAGS    +=  -I$(READLINE_PREFIX)/include
LDLIBS      =   -L$(LIBFT_DIR) -lft -L$(READLINE_PREFIX)/lib -lreadline
else
LDLIBS		=	-L$(LIBFT_DIR) -lft -lreadline -ltermcap
endif

all: $(NAME)

$(NAME): $(LIBFT) $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) $(LDLIBS) -o $(NAME)

$(LIBFT):
	$(MAKE) -C $(LIBFT_DIR)

$(OBJ_DIR)/%.o: %.c Makefile
	mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $(CPPFLAGS) -MMD -MP -c $< -o $@

clean:
	$(MAKE) clean -C $(LIBFT_DIR)
	$(RM) $(OBJ_DIR)

fclean: clean
	$(MAKE) fclean -C $(LIBFT_DIR)
	$(RM) $(NAME)

re: fclean all

-include $(DEPS)

.PHONY: all clean fclean re
