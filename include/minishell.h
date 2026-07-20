/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dgeara <dgeara@student.42lausanne.ch>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/06 16:12:40 by cmauley           #+#    #+#             */
/*   Updated: 2026/07/19 18:14:28 by dgeara           ###   ########.fr       */
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
# include "../libft/libft.h"	// libft functions
# include <stdbool.h>			// bool type

typedef enum e_token_type
{
	T_WORD,
	T_PIPE,
	T_REDIR_IN,
	T_REDIR_OUT,
	T_APPEND,
	T_HEREDOC
}			t_token_type;

typedef struct s_token
{
	t_token_type	type;
	char			*value;
	struct s_token	*next;
}					t_token;

/* struct temp pour commencer exec (dounia) */
typedef struct	s_cmd
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
	t_sig		sig;
	t_env		*env;
	t_token		*token;
	char		**env_for_exec;
	t_parser	*parser;
	char		*path;
}	t_shell;

#endif
