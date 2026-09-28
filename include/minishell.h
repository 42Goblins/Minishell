/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dgeara <dgeara@student.42lausanne.ch>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/06 16:12:40 by cmauley           #+#    #+#             */
<<<<<<< HEAD
/*   Updated: 2026/09/29 17:56:48 by dgeara           ###   ########.fr       */
=======
/*   Updated: 2026/09/29 00:53:08 by dgeara           ###   ########.fr       */
>>>>>>> 50f9fc8 (chore: euuuuh trying to fix les still reachables des les child)
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
# include "../libft/inc/libft.h"// libft functions
# include <stdbool.h>			// bool type

# define DEFAULT_PATH "/usr/local/bin:/usr/bin:/bin:/usr/sbin:/sbin"

/* Token categories produced by the lexer. */
typedef enum e_token_type
{
	T_WORD,
	T_PIPE,
	T_REDIR_IN,
	T_REDIR_OUT,
	T_APPEND,
	T_HEREDOC
}			t_token_type;

/* Doubly linked list node storing one parsed input token. */
typedef struct s_token
{
	t_token_type	type;
	char			*value;
	bool			had_quotes;
	struct s_token	*prev;
	struct s_token	*next;
}					t_token;

/* Linked list node storing one environment variable. */
typedef struct s_env
{
	char			*key;
	char			*value;
	struct s_env	*next;
}	t_env;

/* Command node produced by the parser and consumed by execution. */
typedef struct s_cmd
{
	char			**cmd_and_args;
	int				fd_in;
	int				fd_out;
	bool			is_builtin;
	bool			access_check;
	struct s_cmd	*next;
}	t_cmd;

/* Main shell state shared across parsing, execution, and cleanup. */
typedef struct s_shell
{
	t_env		*env;
	t_token		*token;
	t_cmd		*cmds;
	char		*current_line;
}				t_shell;

/* ========================================================================== */
/*                                  MAIN                                      */
/* ========================================================================== */

/* main.c */
void	reset_shell_state(t_shell *shell);
void	process_line(t_shell *shell, char *line);
int		launch_loop(t_shell *shell);
int		main(int ac, char **av, char **env);

/* ========================================================================== */
/*                                  SETUP                                     */
/* ========================================================================== */

/* setup.c */
int		update_shlvl(t_env *env);
char	*safe_getcwd(void);
int		ensure_pwd(t_shell *shell);
int		create_minimal_env(t_shell *shell);
int		setup(t_shell *shell, char **env);

/* ========================================================================== */
/*                                   ENV                                      */
/* ========================================================================== */

/* setup_env.c */
char	*cpy_key(char *env);
char	*cpy_value(char *env);
t_env	*new_env_node(char *env_line);
int		setup_env(t_shell *shell, char **env);

/* env_utils.c */
void	set_env_value(t_env *env, char *key, char *value);
char	*get_env_value(t_env *env, char *key);
int		update_env_vars(t_env **env, char *key, char *value);

/* ========================================================================== */
/*                                BUILTINS                                    */
/* ========================================================================== */

/* cd.c */
void	update_env_pwd(t_env **env);
int		go_to_oldpwd(t_env *env);
int		go_to_home_dir(t_env *env);
int		exec_cd(t_shell *shell, char **cmd);

/* echo.c */
int		has_n_flag(char *str);
int		exec_echo(char **cmd);

/* env.c */
int		exec_env(t_env *env, char **cmd);

/* exit.c */
void	clean_exit(t_shell *shell, int status);
int		is_num(char *str);
int		exec_exit(t_shell *shell, char **cmd);

/* export.c */
int		export_error(char *str);
int		safe_add_var(t_env **env, char *key, char *value);
int		add_new_var(t_env **env, char *key, char *value);
int		parse_export(char *str, char **key, char **value);
int		exec_export(t_env **env, char **cmd);

/* export_print.c */
t_env	**lst_cpy(t_env *env);
t_env	**sort_export(t_env *env);
int		print_export(t_env *env);

/* pwd.c */
int		exec_pwd(char **cmd);

/* unset.c */
void	del_env_variable(t_env **first, t_env *prev, t_env *current);
int		exec_unset(t_env **env, char **cmd);

/* ========================================================================== */
/*                                  LEXER                                     */
/* ========================================================================== */

/* lexer.c */
int		tokenizer(char *input, t_shell *shell);
int		add_operator_token(t_shell *shell, t_token_type type, char *str);

/* lexer_nodes.c */
t_token	*create_token_node(t_token_type type, char *value);
void	add_token_back(t_token **head, t_token *new_token);
void	free_tokens(t_token *head);

/* lexer_quotes.c */
int		remove_quotes_from_tokens(t_token *tokens);
char	*remove_quotes(char *value);

/* lexer_redir.c */
int		add_redir_in_or_heredoc(char *input, int i, t_shell *shell);
int		add_redir_out_or_append(char *input, int i, t_shell *shell);

/* lexer_utils.c */
int		word_len(char *input, int i);
int		is_blank(char character);

/* ========================================================================== */
/*                                EXPANSION                                   */
/* ========================================================================== */

/* expand_tokens.c */
int		expand_tokens(t_token *tokens, t_env *env);

/* expansion.c */
char	*expand_word(char *word, t_env *env);

/* expansion_utils.c */
void	free_three_strings(char *first, char *second, char *third);
char	*append_expansion_part(char *built, char *part);
char	*remove_char_at(char *str, int index);

/* expansion_vars.c */
bool	is_dollar_expand(char *word, int i, bool in_single);
char	*get_var_value(char *var, t_env *env);
int		var_name_len(char *var);

/* ========================================================================== */
/*                                  PARSER                                    */
/* ========================================================================== */

/* parser.c */
t_cmd	*parse_tokens(t_shell *shell, t_token *tokens);
int		count_cmd_args(t_token *tokens);
char	**create_cmd_and_args(t_token *tokens);
t_cmd	*create_cmd_node(t_shell *shell, t_token *tokens);

/* parser_redir.c */
int		open_redirections(t_shell *shell, t_cmd *cmd, t_token *tokens);

/* parser_utils.c */
int		is_redirection_token(t_token_type type);
int		is_empty_unquoted_word(t_token *token);

/* syntax.c */
int		validate_syntax(t_token *tokens);

/* ========================================================================== */
/*                                  HEREDOC                                   */
/* ========================================================================== */

/* heredoc.c */
int		open_heredoc_redirection(t_shell *shell, t_cmd *cmd,
			t_token *delimiter);

/* heredoc_utils.c */
int		write_heredoc_content(int write_fd, char *line, bool should_expand,
			t_env *env);
int		write_heredoc_line(int write_fd, char *line);

/* ========================================================================== */
/*                                    EXEC                                    */
/* ========================================================================== */

/* exec.c */
int		count_cmds(t_cmd *cmds);
void	launch_exec(t_shell *shell, t_cmd *cmds);

/* exec_builtins.c */
bool	check_is_builtins(char *cmd);
void	exec_builtins(t_shell *shell, t_cmd *cmd);
void	exec_single_builtins(t_shell *shell, t_cmd *cmd);

/* exec_external.c */
int		env_len(t_env *env);
char	**t_env_to_tab(t_env *env);
<<<<<<< HEAD
void	command_error(t_cmd *cmd, int not_exec);
=======
void	command_error(t_shell *shell, t_cmd *cmd, int not_exec);
>>>>>>> 50f9fc8 (chore: euuuuh trying to fix les still reachables des les child)
void	exec_external(t_shell *shell, t_cmd *cmd, t_env *env);
void	exec_single_external(t_shell *shell, t_cmd *cmd, t_env *env);

/* exec_external_path.c */
int		handle_direct_path_error(t_shell *shell, char *cmd);
char	*try_path(char *dir, char *cmd, int *not_exec);
char	*get_path(t_env *env);
char	*find_path(char *cmd, t_env *env, int *not_exec);

/* exec_pipeline.c */
void	wait_all_pids(pid_t last_pid);
void	set_fds(t_cmd *cmds, int prev_fd, int pipefd[2]);
void	exec_cmd(t_shell *shell, t_cmd *cmds);
pid_t	spawn_cmd(t_shell *shell, t_cmd *cmds, int *prev_fd, int pipefd[2]);
void	exec_pipeline(t_shell *shell, t_cmd *cmds);

/* ========================================================================== */
/*                                  SIGNALS                                   */
/* ========================================================================== */

extern int	g_signal;

/* signals.c */
int		ignore_exec_signals(void);
int		setup_signals(void);
int		setup_heredoc_signals(void);

/* signals_utils.c */
void	print_signal_message(int signal);
void	track_child_signal(int status, int *sigint, int *sigquit);
void	print_pipeline_signal(int sigint, int sigquit);

/* ========================================================================== */
/*                                  UTILS                                     */
/* ========================================================================== */

/* close_fds.c */
int		safe_close_fd(int *fd);
void	safe_close_all_fd(int *fd, int *pipefd);

/* free_cmds.c */
void	free_cmds(t_cmd *cmds);

/* free_stuff.c */
void	free_t_env(t_env *env);
void	free_lst_env(t_env *env);
void	free_tab(char **tab);

/* get_status.c */
int		*get_status(void);

/* read_input.c */
char	*read_input(char *prompt);

#endif
