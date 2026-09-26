# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: dgeara <dgeara@student.42lausanne.ch>      +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2026/07/09 00:00:00 by cmauley           #+#    #+#              #
#    Updated: 2026/09/26 14:03:25 by dgeara           ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

# ══════════════════════════════════════════════════════════════════════════════
#   minishell — Makefile
# ══════════════════════════════════════════════════════════════════════════════

NAME        =   minishell

CC          =   cc
CFLAGS      =   -Wall -Wextra -Werror
CPPFLAGS    =   -Iinclude -Ilibft
RM          =   rm -rf

SRC_DIR     =   srcs
OBJ_DIR     =   objs
LIBFT_DIR   =   libft
LIBFT       =   $(LIBFT_DIR)/libft.a

SRCS        =   $(shell find $(SRC_DIR) -type f -name "*.c")
OBJS        =   $(SRCS:%.c=$(OBJ_DIR)/%.o)
DEPS        =   $(OBJS:.o=.d)

TOTAL_FILES :=  $(words $(SRCS))
CURRENT_FILE = 0

# ── Colors ────────────────────────────────────────────────────────────────────
END         :=  \033[0m
BOLD        :=  \033[1m
GREEN       :=  \033[32m
BLUE        :=  \033[34m
LIGHTBLUE   :=  \033[1;96m
ORANGE      :=  \033[38;5;208m
YELLOW      :=  \033[33m

# ── OS detection (readline differs on macOS vs Linux) ────────────────────────
UNAME_S     :=  $(shell uname -s)

ifeq ($(UNAME_S), Darwin)
# needs: brew install readline
READLINE_PREFIX :=  $(shell brew --prefix readline 2>/dev/null)
CPPFLAGS    +=  -I$(READLINE_PREFIX)/include
LDLIBS      =   -L$(LIBFT_DIR) -lft -L$(READLINE_PREFIX)/lib -lreadline
else
LDLIBS      =   -L$(LIBFT_DIR) -lft -lreadline -ltermcap
endif

# ── Targets ───────────────────────────────────────────────────────────────────

all: $(NAME) ascii

$(NAME): $(LIBFT) $(OBJS)
	@echo "$(BOLD)$(BLUE)🔗 Linking executable...$(END)"
	@$(CC) $(CFLAGS) $(OBJS) $(LDLIBS) -o $(NAME)
	@echo "$(BOLD)$(GREEN)✔  $(NAME) built successfully$(END)"

# ── Libraries ─────────────────────────────────────────────────────────────────

$(LIBFT):
	@echo "$(BOLD)$(YELLOW)→ Building libft...$(END)"
	@$(MAKE) -C $(LIBFT_DIR) --silent

# ── Compilation ───────────────────────────────────────────────────────────────

$(OBJ_DIR)/%.o: %.c Makefile
	@mkdir -p $(dir $@)
	@$(eval CURRENT_FILE=$(shell echo $$(($(CURRENT_FILE)+1))))
	@printf "$(BOLD)$(LIGHTBLUE)⚡ [%2d/%2d] Compiling %-40s$(END)" \
		$(CURRENT_FILE) $(TOTAL_FILES) "$<"
	@$(CC) $(CFLAGS) $(CPPFLAGS) -MMD -MP -c $< -o $@
	@echo " $(GREEN)✓$(END)"

# ── Clean ─────────────────────────────────────────────────────────────────────

clean:
	@echo "$(BOLD)$(YELLOW)🧹 Cleaning objects...$(END)"
	@$(RM) $(OBJ_DIR)

fclean: clean
	@echo "$(BOLD)$(YELLOW)🗑  Removing executable...$(END)"
	@$(RM) $(NAME)

superclean: fclean
	@echo "$(BOLD)$(YELLOW)💣 Deep cleaning libft too...$(END)"
	@$(MAKE) -C $(LIBFT_DIR) fclean --silent

re: fclean all

# ── ASCII art ─────────────────────────────────────────────────────────────────

ascii:
	@echo "$(BOLD)$(ORANGE)"
	@if [ -f ascii_art.txt ]; then cat ascii_art.txt; fi
	@echo "$(END)"
	@echo "$(BOLD)$(GREEN)✨  minishell is ready ✨$(END)"
	@echo "$(YELLOW)➜   use ./$(NAME)$(END)"

-include $(DEPS)

.PHONY: all clean fclean superclean re ascii
