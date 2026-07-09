#include "minishell.h"

int	main(void)
{
	t_token	*tokens;

	tokens = lexer("echo hello > out");
	print_tokens(tokens);
	free_tokens(tokens);
	return (0);
}
