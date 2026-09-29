*This project has been created as part of the 42 curriculum by cmauley, dgeara.*

# Minishell

<p align="center">
  <img src="https://img.shields.io/badge/Language-C-00599C?style=for-the-badge&logo=c" alt="C language">
  <img src="https://img.shields.io/badge/School-42-000000?style=for-the-badge&logo=42" alt="42 School">
  <img src="https://img.shields.io/badge/Unix-POSIX-4B8BBE?style=for-the-badge" alt="POSIX APIs">
  <img src="https://img.shields.io/badge/Scope-Mandatory-00A676?style=for-the-badge" alt="Mandatory scope">
</p>

<p align="center">
  <b>A small interactive Unix shell written in C.</b><br>
  Prompt, history, parsing, expansion, redirections, pipes, builtins, signals,
  and process execution.
</p>

<p align="center">
  Authors: <code>cmauley</code> - GitHub profile: <a href="https://github.com/gpalemo">gpalemo</a> |
  <code>dgeara</code> - GitHub profile: <a href="https://github.com/deltafraktal">deltafraktal</a>
</p>

| Project area | Status |
|---|---|
| Mandatory features | Implemented |
| Language | C |
| Libraries and APIs | GNU Readline, Libft, POSIX/Unix system calls |
| Build system | Makefile |
| Main focus | Unix processes, file descriptors, parsing, signals |

## Table of Contents

1. [Description](#1-description)
2. [Features](#2-features)
3. [Project Flow](#3-project-flow)
4. [Understanding A Shell](#4-understanding-a-shell)
   - [4.1 What Is A Shell?](#41-what-is-a-shell)
   - [4.2 Parent And Child Processes](#42-parent-and-child-processes)
5. [Parsing And Expansion](#5-parsing-and-expansion)
   - [5.1 Quotes](#51-quotes)
   - [5.2 Environment Expansion](#52-environment-expansion)
6. [Processes And Execution](#6-processes-and-execution)
7. [Redirections, Pipes And Heredoc](#7-redirections-pipes-and-heredoc)
   - [7.1 Redirections](#71-redirections)
   - [7.2 Pipes](#72-pipes)
   - [7.3 Heredoc](#73-heredoc)
8. [Builtins](#8-builtins)
9. [Signals](#9-signals)
10. [Instructions](#10-instructions)
    - [10.1 Dependencies](#101-dependencies)
    - [10.2 Compilation](#102-compilation)
    - [10.3 Execution](#103-execution)
11. [Usage Examples](#11-usage-examples)
12. [Testing](#12-testing)
13. [Scope And Bash Differences](#13-scope-and-bash-differences)
14. [Resources](#14-resources)
15. [Use of AI](#15-use-of-ai)

## 1. Description

Minishell is a 42 School project whose goal is to recreate a minimal shell
inspired by Bash. The program displays a prompt, reads user input, interprets
commands, and launches the requested programs while handling the main shell
mechanisms required by the subject.

The project is not about writing a full Bash clone. It focuses on understanding
what happens between typing a command and seeing its result: lexical analysis,
quote handling, environment expansion, file descriptor manipulation, process
creation, command execution, exit statuses, and interactive signal behavior.

A typical shell cycle can be summarized as:

```text
read input
   |
   v
tokenize words and operators
   |
   v
expand variables and remove quotes
   |
   v
validate syntax and build commands
   |
   v
prepare redirections and heredocs
   |
   v
execute builtins or external programs
   |
   v
store the resulting exit status
```

## 2. Features

This implementation supports the mandatory behavior required by the Minishell
subject:

- interactive prompt using `readline`;
- command history;
- executable lookup through `PATH`;
- execution through relative and absolute paths;
- single quotes and double quotes;
- environment variable expansion with `$VAR`;
- last exit status expansion with `$?`;
- redirections: `<`, `>`, `>>`;
- heredoc redirection: `<<`;
- pipes with `|`;
- builtins required by the subject;
- signal handling for prompt, command execution, and heredoc input.

## 3. Project Flow

The code is organized around a few major responsibilities:

| Step | Role |
|---|---|
| Input | read one line from the user or from standard input |
| Lexer | split the line into words and operators |
| Expansion | replace variables and handle `$?` |
| Quote removal | remove syntactic quotes after expansion decisions |
| Syntax validation | reject invalid operator sequences |
| Parser | create command nodes with arguments and redirections |
| Execution | run builtins, fork external commands, connect pipes |
| Cleanup | free command state before the next prompt |

The shell keeps a small state structure containing the environment list, current
tokens, parsed commands, and the last status used by `$?`.

Main code entry points for this flow:

| Area | Entry point |
|---|---|
| Program setup | [`setup`](srcs/setup/setup.c#L95) |
| Main loop | [`launch_loop`](srcs/main.c#L43) |
| Line processing | [`process_line`](srcs/main.c#L29) |
| Input handling | [`read_input`](srcs/utils/read_input.c#L76) |
| Tokenization | [`tokenizer`](srcs/lexer/lexer.c#L27) |
| Expansion | [`expand_tokens`](srcs/expansion/expand_tokens.c#L21) |
| Syntax validation | [`validate_syntax`](srcs/parser/syntax.c#L27) |
| Parsing | [`parse_tokens`](srcs/parser/parser.c#L27) |
| Execution dispatch | [`launch_exec`](srcs/exec/exec.c#L32) |
| Status storage | [`get_status`](srcs/utils/get_status.c#L20) |

## 4. Understanding A Shell

### 4.1 What Is A Shell?

A shell is a command interpreter. It reads text written by the user and turns it
into actions performed by the operating system.

For example:

```bash
cat infile | grep hello > outfile
```

This line is not executed directly as one program. The shell must understand
that:

- `cat` and `grep` are two commands;
- `|` connects the output of `cat` to the input of `grep`;
- `>` redirects the final output to `outfile`;
- both commands may need to be launched in child processes;
- the final exit status must be saved.

### 4.2 Parent And Child Processes

A running program is a **process**. Minishell itself is one process. When it
needs to run an external program such as `ls`, `cat`, or `grep`, it should not
replace itself directly. If it did, the shell would disappear as soon as the
command starts.

Instead, the shell creates a child process with `fork`:

```text
before fork:

  minishell process

      |
      | fork()
      v

after fork:

  parent process                 child process
  still minishell                copy of minishell
  waits and keeps prompt         prepares and runs command
```

`fork` returns twice: once in the parent and once in the child. The two processes
continue from the same place in the code, but they do not have the same role.

| Process | Role |
|---|---|
| Parent | keeps the shell alive, waits for children, stores the final status |
| Child | prepares redirections or pipes, then executes the requested command |

The usual external-command flow is:

```text
parent shell
    |
    | fork()
    +---------------- child process
    |                    |
    |                    | prepare file descriptors
    |                    v
    |                execve("/bin/ls", ...)
    |
    v
 waitpid(child)
    |
    v
store exit status
```

`execve` is the point where the child stops being a copy of minishell and becomes
the requested executable. If `execve` succeeds, the original child code does not
continue. If it fails, the child must print an error and exit with the correct
status.

`waitpid` lets the parent wait for the child and inspect how it ended:

- normal exit, for example status `0`, `1`, or `127`;
- signal interruption, for example `Ctrl-C` leading to status `130`;
- quit signal, for example `Ctrl-\` leading to status `131`.

In this implementation, external commands are handled through
[`exec_external`](srcs/exec/exec_external.c#L85), while path lookup is handled by
[`find_path`](srcs/exec/exec_external_path.c#L90). Pipeline children are created
from [`spawn_cmd`](srcs/exec/exec_pipeline.c#L93).

## 5. Parsing And Expansion

Parsing is one of the hardest parts of a shell because spaces, quotes, and
operators do not all have the same meaning depending on the context.

The main lexer logic starts in [`tokenizer`](srcs/lexer/lexer.c#L27). Word length
and quote errors are handled by [`word_len`](srcs/lexer/lexer_utils.c#L20), while
quote removal is handled by
[`remove_quotes_from_tokens`](srcs/lexer/lexer_quotes.c#L20).

### 5.1 Quotes

Single quotes preserve their content literally:

```bash
echo '$HOME'
```

Output:

```text
$HOME
```

Double quotes still allow environment expansion:

```bash
echo "$HOME"
```

Output:

```text
/home/user
```

After the shell has used quotes to decide how a word behaves, syntactic quotes
are removed before execution.

### 5.2 Environment Expansion

Variables are expanded from the shell environment:

```bash
echo $USER
```

The special variable `$?` expands to the status of the last executed command:

```bash
false
echo $?
```

Output:

```text
1
```

Expansion must happen before command execution, but after the shell knows which
characters are protected by quotes. The expansion pass starts in
[`expand_tokens`](srcs/expansion/expand_tokens.c#L21), and individual words are
expanded by [`expand_word`](srcs/expansion/expansion.c#L32). Variable lookup is
implemented in [`get_var_value`](srcs/expansion/expansion_vars.c#L39).

## 6. Processes And Execution

Minishell has to distinguish between two kinds of commands:

| Command type | Example | Execution strategy |
|---|---|---|
| Builtin | `cd`, `export`, `exit` | may need to run in the parent shell |
| External command | `ls`, `/bin/cat` | usually runs in a child process with `execve` |

Some builtins must modify the shell itself. For example, `cd` changes the
current working directory of the shell process. If `cd` always ran only inside a
child process, the parent shell would remain in the same directory after the
child exits.

Pipelines are different: each command in a pipeline runs in a child process so
file descriptors can be connected independently.

This parent/child distinction is especially important for builtins:

| Situation | Where it runs | Why |
|---|---|---|
| `cd /tmp` | parent | the shell itself must change directory |
| `export A=1` | parent | the shell environment must be updated |
| `exit` | parent | the shell process must terminate |
| `echo hi | cat` | child processes | each side of the pipe needs isolated file descriptors |

If `cd` ran only in a child, the child would change directory and then exit, but
the parent shell would remain in the old directory. This is why single builtins
are handled separately from builtins inside pipelines.

Execution is dispatched by [`launch_exec`](srcs/exec/exec.c#L32). Builtins are
routed through [`exec_builtins`](srcs/exec/exec_builtins.c#L41), single builtins
through [`exec_single_builtins`](srcs/exec/exec_builtins.c#L63), and pipelines
through [`exec_pipeline`](srcs/exec/exec_pipeline.c#L128).

## 7. Redirections, Pipes And Heredoc

### 7.1 Redirections

Redirections change where a command reads from or writes to:

| Operator | Meaning |
|---|---|
| `< file` | read standard input from `file` |
| `> file` | write standard output to `file`, truncating it |
| `>> file` | append standard output to `file` |
| `<< delimiter` | read input until `delimiter` is reached |

Internally, redirections are implemented by opening the requested file and
using `dup2` to replace `STDIN_FILENO` or `STDOUT_FILENO` before execution.

A file descriptor is a small integer used by the operating system to represent
an open file or stream:

| Descriptor | Meaning |
|---|---|
| `0` | standard input |
| `1` | standard output |
| `2` | standard error |

For example, `echo hello > outfile` opens `outfile`, then redirects descriptor
`1` to that file before running `echo`. The command still writes to standard
output from its point of view, but standard output now points to `outfile`.

Lexer redirection tokens are created in
[`add_redir_in_or_heredoc`](srcs/lexer/lexer_redir.c#L18) and
[`add_redir_out_or_append`](srcs/lexer/lexer_redir.c#L34). Redirections are then
opened from [`open_redirections`](srcs/parser/parser_redir.c#L32).

### 7.2 Pipes

A pipe connects the output of one command to the input of the next one:

```bash
ls -la | grep minishell | wc -l
```

Conceptually:

```text
ls -la stdout -> pipe -> grep stdin
grep stdout   -> pipe -> wc stdin
```

Every pipe has two ends: a read end and a write end. For `cmd1 | cmd2`, the
first child writes into the pipe and the second child reads from it.

```text
cmd1 stdout -> pipe write end | pipe read end -> cmd2 stdin
```

Every unused pipe end must be closed in the correct process. Otherwise,
commands may block while waiting for an EOF that never arrives. File descriptor
routing for pipeline commands is centralized in
[`set_fds`](srcs/exec/exec_pipeline.c#L48), and cleanup helpers are grouped in
[`close_fds.c`](srcs/utils/close_fds.c).

### 7.3 Heredoc

A heredoc provides inline input to a command:

```bash
cat << EOF
hello
EOF
```

If the delimiter is quoted, variable expansion inside the heredoc body is
disabled:

```bash
cat << 'EOF'
$USER
EOF
```

Output:

```text
$USER
```

Heredoc setup starts in
[`open_heredoc_redirection`](srcs/heredoc/heredoc.c#L32). Writing and optional
expansion of heredoc content are handled by
[`write_heredoc_content`](srcs/heredoc/heredoc_utils.c#L21).

## 8. Builtins

The subject requires the following builtins:

| Builtin | Required behavior |
|---|---|
| `echo` | supports option `-n` |
| `cd` | changes directory with a relative or absolute path |
| `pwd` | prints the current working directory, no options required |
| `export` | manages environment variables, no options required |
| `unset` | removes environment variables, no options required |
| `env` | prints the environment, no options or arguments required |
| `exit` | exits the shell, no options required |

Each builtin has its own implementation:

| Builtin | Function |
|---|---|
| `echo` | [`exec_echo`](srcs/builtins/echo.c#L38) |
| `cd` | [`exec_cd`](srcs/builtins/cd.c#L83) |
| `pwd` | [`exec_pwd`](srcs/builtins/pwd.c#L33) |
| `export` | [`exec_export`](srcs/builtins/export.c#L92) and [`print_export`](srcs/builtins/export_print.c#L80) |
| `unset` | [`exec_unset`](srcs/builtins/unset.c#L30) |
| `env` | [`exec_env`](srcs/builtins/env.c#L18) |
| `exit` | [`exec_exit`](srcs/builtins/exit.c#L53) |

Relative paths depend on the current directory:

```bash
cd srcs
cd ..
```

Absolute paths start from the root directory:

```bash
cd /
cd /tmp
```

## 9. Signals

Interactive shells must react to keyboard shortcuts differently depending on
what they are doing.

| Input | At prompt | During execution | During heredoc |
|---|---|---|---|
| `Ctrl-C` | clears the line and displays a new prompt | interrupts the running command | cancels heredoc input |
| `Ctrl-\` | ignored | may quit the running command | ignored |
| `Ctrl-D` | exits the shell on an empty prompt | sends EOF to the running program | ends heredoc input with a warning if delimiter was not reached |

The project uses a single global signal variable, as required by the subject,
to record received signals without accessing complex shell structures from a
signal handler.

Prompt and heredoc signal modes are configured in
[`setup_signals`](srcs/signals/signals.c#L39) and
[`setup_heredoc_signals`](srcs/signals/signals.c#L54). Signal messages and child
status tracking are grouped in [`signals_utils.c`](srcs/signals/signals_utils.c).

## 10. Instructions

### 10.1 Dependencies

This project requires:

- a C compiler;
- `make`;
- the GNU Readline library;
- the project `libft`, included in the repository.

On Debian/Ubuntu-based systems, Readline development files can usually be
installed with:

```bash
sudo apt install libreadline-dev
```

### 10.2 Compilation

```bash
make
```

The Makefile also provides:

```bash
make clean
make fclean
make re
```

### 10.3 Execution

```bash
./minishell
```

Exit the shell with:

```bash
exit
```

or with `Ctrl-D` on an empty prompt.

## 11. Usage Examples

Basic commands:

```bash
echo hello
pwd
cd /tmp
```

Environment expansion:

```bash
echo "Hello $USER"
echo $?
```

Redirections:

```bash
echo hello > outfile
cat < outfile
cat < infile > outfile
```

Pipes:

```bash
ls -la | grep minishell
cat file | wc -l
```

Heredoc:

```bash
cat << EOF
hello from heredoc
EOF
```

Builtins:

```bash
export PROJECT=minishell
echo $PROJECT
unset PROJECT
env
```

## 12. Testing

Useful manual checks:

```bash
make re
./minishell
```

Then test:

```bash
echo hello
cat < missing_file
echo $?
echo hello | wc -c
cat << EOF
hello
EOF
```

Check memory and file descriptors with Valgrind:

```bash
valgrind --leak-check=full --show-leak-kinds=all --track-fds=yes ./minishell
```

A few external testers were also used during development to compare behavior
against Bash and to stress mandatory features. Their results were reviewed
manually because some testers include Bash features outside the mandatory
subject scope.

## 13. Scope And Bash Differences

Minishell follows the mandatory subject scope. It is intentionally smaller than
Bash and does not aim to implement every Bash feature.

Examples of behavior outside the mandatory scope include:

- command separators such as `;`;
- globbing with `*`;
- subshell syntax with parentheses;
- advanced `env` usage with arguments or options;
- full Bash-specific option handling for every builtin;
- tilde expansion such as `~` or `~/path`.

The goal is to implement the required shell mechanisms correctly, not to
reproduce the entire Bash language.

## 14. Resources

Classic references used to understand and verify the project include the Bash
manual, the Readline documentation, Linux manual pages, and the official 42
subject.

### General Documentation

- [GNU Bash Reference Manual](https://www.gnu.org/software/bash/manual/bash.html)
- [GNU Readline Library](https://tiswww.case.edu/php/chet/readline/rltop.html)


### Useful Videos

- [Sending and Handling Signals in C (kill, signal, sigaction) - Jacob Sorber](https://www.youtube.com/watch?v=83M5-NPDeWs&list=PL7_TuD9ZDMhg5uLHLyd8em13XBKfjzCzR)
- [Redirecting standard output in C - CodeVault](https://www.youtube.com/watch?v=5fnVr-zH-SE&list=PL7_TuD9ZDMhg5uLHLyd8em13XBKfjzCzR&index=2)
- [Communicating between processes (using pipes) in C - CodeVault](https://www.youtube.com/watch?v=Mqb2dVRe0uo&list=PL7_TuD9ZDMhg5uLHLyd8em13XBKfjzCzR&index=3)
- [Linux processes, init, fork/exec, ps, kill, fg, bg, jobs - Engineer Man](https://www.youtube.com/watch?v=TJzltwv7jJs&list=PL7_TuD9ZDMhg5uLHLyd8em13XBKfjzCzR&index=6)
- [Heredocs in Bash! Understanding how they work and a few gotchas. You Suck at Programming #069 - You Suck at Programming](https://www.youtube.com/watch?v=-a1VAole01s)

### Manual Pages

- [readline(3)](https://man7.org/linux/man-pages/man3/readline.3.html)
- [fork(2)](https://man7.org/linux/man-pages/man2/fork.2.html)
- [execve(2)](https://man7.org/linux/man-pages/man2/execve.2.html)
- [waitpid(2)](https://man7.org/linux/man-pages/man2/waitpid.2.html)
- [pipe(2)](https://man7.org/linux/man-pages/man2/pipe.2.html)
- [dup2(2)](https://man7.org/linux/man-pages/man2/dup.2.html)
- [open(2)](https://man7.org/linux/man-pages/man2/open.2.html)
- [signal(7)](https://man7.org/linux/man-pages/man7/signal.7.html)
- [sigaction(2)](https://man7.org/linux/man-pages/man2/sigaction.2.html)

### 42 Subject

- Official Minishell subject provided by 42 School.

## 15. Use of AI

AI was used as a pedagogical support tool throughout the project. It helped
with:

- explaining shell concepts such as `fork`, `execve`, pipes, file descriptors,
  heredoc behavior, and signal handling;
- planning manual test scenarios and interpreting differences between Bash and
  Minishell;
- drafting and improving documentation, including this README.

The project authors implemented the shell and carried out extensive manual
debugging: tracing execution, running commands, comparing results with Bash,
and checking memory and file descriptors with Valgrind. AI explanations and
suggestions were checked against the 42 subject, manual pages, and observed
behavior.

Other 42 students also tested the shell, tried edge cases, and shared feedback.
These hands-on sessions helped identify issues and verify the shell's behavior
beyond our own test scenarios.

