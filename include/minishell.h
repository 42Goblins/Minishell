/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cmauley <cmauley@student.42lausanne.ch>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/06 16:12:40 by cmauley           #+#    #+#             */
/*   Updated: 2026/08/17 22:01:48 by cmauley          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef MINISHELL_H
# define MINISHELL_H

# include <stdio.h>				// printf
# include <stdlib.h>			// malloc, free, exit
# include <string.h>			// strerror
# include <unistd.h>			// write, access, read, close, fork, execve
# include <fcntl.h>				// open, O_RDONLY, O_WRONLY, O_CREAT
# include <sys/stat.h>			// stat, lstat, fstat
# include <sys/types.h>			// system types
# include <sys/wait.h>			// wait, waitpid, wait3, wait4
# include <dirent.h>			// opendir, readdir, closedir
# include <readline/readline.h>	// readline, rl_*
# include <readline/history.h>	// add_history, rl_clear_history
# include <signal.h>			// signal, sigaction, kill
# include <sys/ioctl.h>			// ioctl, isatty, ttyname, ttyslot
# include <termios.h>			// tcsetattr, tcgetattr
# include <termcap.h>			// tgetent, tgetflag, tgetnum, tgetstr, tputs
# include "../libft/inc/libft.h"	// libft functions
# include <stdbool.h>			// bool type

/* enum pour les types de tokens (chloé) */
typedef enum e_token_type
{
	T_WORD,
	T_PIPE,
	T_REDIR_IN,
	T_REDIR_OUT,
	T_APPEND,
	T_HEREDOC
}			t_token_type;

/* struct temp pour les tokens (chloé) */
typedef struct s_token
{
	t_token_type	type;
	char			*value;
	bool			had_quotes;
	struct s_token	*prev;
	struct s_token	*next;
}					t_token;

/* struct temp pour commencer exec (dounia) */
typedef struct s_env
{
	char			*key;
	char			*value;
	struct s_env	*next;
}	t_env;

typedef struct s_cmd
{
	char	**cmd_and_args;
	char	*path;
	int		fd_in;
	int		fd_out;
	bool	is_builtin;
	bool	access_check;
	struct s_cmd	*next;
}	t_cmd;

typedef struct	s_shell
{
	// t_sig		sig;
	t_env		*env;
	t_token		*token;
	// char		**env_for_exec;
	t_cmd		*cmds;
	char		*path;
}	t_shell;


/* ========================================================================== */
/*                                  MAIN                                      */
/* ========================================================================== */
/* main.c */
int		main(int ac, char **av, char **env);


/* ========================================================================== */
/*                                  ENV                                       */
/* ========================================================================== */
/* setup_env.c */
char	*cpy_key(char *env);
char	*cpy_value(char *env);
void	setup_env(t_shell *shell, char **env);
t_env	*new_env_node(char *env_line);

/* ========================================================================== */
/*                                BUILTINS                                    */
/* ========================================================================== */
/*cd.c */
int		exec_cd(t_shell *shell, char **cmd);
void	update_env(t_env *env);
char	*get_env_value(t_env *env, const char *key);
void	set_env_value(t_env *env, const char *key, const char *value);
void	go_to_oldpwd(t_env *env);
void	go_to_home_dir(t_env *env);

/* echo.c */
int		has_n_flag(char *str);
int		exec_echo(char **cmd);

/* env.c */
int		exec_env(t_env *env, char **cmd);

/* pwd.c */
int	exec_pwd();

/* unset.c */
void	free_t_env(t_env *env);
void	del_env_variable(t_env **first, t_env *prev, t_env *current);
int		exec_unset(t_env **env, char **cmd);

/* exit.c */
int	is_num(char *str);
int	exec_exit(char **cmd);

/* export_print.c */
t_env **lst_cpy(t_env *env);
t_env **sort_export(t_env *env);
int print_export(t_env *env);

/* export.c */
int		export_error(char *str);
void	add_new_var(t_env **env, char *key, char *value);
int		parse_export(char *str, char **key, char **value);
void	update_env_vars(t_env **env, char *key, char *value);
int		exec_export(t_env **env, char **cmd);

/* ========================================================================== */
/*                                  LEXER                                     */
/* ========================================================================== */

t_token		*create_token_node(t_token_type type, char *value);
void		add_token_back(t_token **head, t_token *new_token);
void		free_tokens(t_token *head);
int			add_operator_token(t_shell *shell, t_token_type type, char *str);
int			add_redir_in_or_heredoc(char *input, int i, t_shell *shell);
int			add_redir_out_or_append(char *input, int i, t_shell *shell);
char		*remove_quotes(char *value);
int			is_blank(char character);
int			word_len(char *input, int i);
int			tokenizer(char *input, t_shell *shell);
int			remove_quotes_from_tokens(t_token *tokens);

/* ========================================================================== */
/*                                  EXPANSION                                 */
/* ========================================================================== */

int			var_name_len(char *var);
char		*get_var_value(char *var, t_env *env);
bool		is_dollar_expand(char *word, int i, bool in_single);
char		*expand_word(char *word, t_env *env);
char		*append_expansion_part(char *built, char *part);
void		free_three_strings(char *first, char *second, char *third);
int			expand_tokens(t_token *tokens, t_env *env);

/* ========================================================================== */
/*                                  PARSER                                    */
/* ========================================================================== */

int			count_cmd_args(t_token *tokens);
char		**create_cmd_args(t_token *tokens);
t_cmd		*create_cmd_node(t_token *tokens);
t_cmd		*parse_tokens(t_token *tokens);

/* ========================================================================== */
/*                                  UTILS                                     */
/* ========================================================================== */

int			*get_status(void);
void		free_tab(char **tab);

#endif
