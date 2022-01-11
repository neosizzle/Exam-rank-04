#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#define STDIN 0
#define STDOUT 1
#define STDERR 2
#define PIPE_IN 0
#define PIPE_OUT 0
#define TYPE_END 0
#define TYPE_PIPE 1
#define TYPE_BREAK 2

typedef struct s_list
{
	char	**args;
	int		len;
	int		type;
	int		pipes[2];
	struct	s_list 	*next;
	struct	s_list	*prev;
} t_list;

int	ft_strlen(char *str)
{
	int	i;

	i = -1;
	while (str[++i]) ;
	return i;
}

char	*ft_strdup(char *str)
{
	char	*res;
	int		i;

	res = malloc(sizeof (char) * (ft_strlen(str) + 1));
	i = -1;
	while (str[++i])
		res[i] = str[i];
	res[i] = 0;
	return res;
}

int	print_err(char	*err)
{
	write(STDERR, err, ft_strlen(err));
	return (EXIT_FAILURE);
}

int	exit_fatal()
{
	print_err("error: fatal\n");
	exit(EXIT_FAILURE);
	return EXIT_FAILURE;
}

int	add_arg(t_list	*cmd, char *arg)
{
	char	**copy;
	int		i;
	
	copy = malloc(sizeof(char *) * (cmd->len + 2));
	if (!copy)
		exit_fatal();
	i = -1;
	while (++i < cmd->len)
		copy[i] = cmd->args[i];
	copy[i] = ft_strdup(arg);
	copy[++i] = 0;
	if (cmd->len > 0)
		free(cmd->args);
	cmd->args = copy;
	cmd->len++;
	return EXIT_SUCCESS;
}

int	list_push(t_list **list, char *arg)
{
	t_list	*new_list;
	t_list	*curr;

	new_list = malloc(sizeof (t_list));
	if (!new_list)
		exit_fatal();
	new_list->args = 0;
	new_list->len = 0;
	new_list->type = TYPE_END;
	new_list->next = 0;
	new_list->prev = 0;
	if (*list)
	{
		(*list)->next = new_list;
		new_list->prev = *list;
	}
	*list = new_list;
	return(add_arg(new_list, arg));
}

int	list_rewind(t_list **list)
{
	while ((*list) && (*list)->prev)
		*list = (*list)->prev;
	return EXIT_SUCCESS;
}

int	list_clear(t_list **list)
{
	t_list	*temp;
	int		i;

	list_rewind(list);
	while (*list)
	{
		temp = (*list)->next;
		i = -1;
		while (++i < (*list)->len)
			free((*list)->args[i]);
		free((*list)->args);
		free((*list));
		*list = temp;
	}
	list = 0;
	return EXIT_SUCCESS;
}

int	parse_arg(t_list **cmds, char *arg)
{
	int	is_break;

	is_break = !strcmp(arg, ";");
	if (is_break && !*cmds)
		return EXIT_SUCCESS;
	else if (!is_break && (!*cmds || (*cmds)->type > TYPE_END))
		return (list_push(cmds, arg));	
	else if (!strcmp(arg, "|"))
		(*cmds)->type = TYPE_PIPE;
	else if (is_break)
		(*cmds)->type = TYPE_BREAK;
	else
	{
		return (add_arg(*cmds, arg));
	}
}

int	main(int argc, char **argv)
{ 
	t_list	*cmds;
	int	i;

	cmds = 0;
	i = 0;
	while (++i < argc)
		parse_arg(&cmds, argv[i]);
	list_rewind(&cmds);
	t_list	*curr;
	int	j;
	curr = cmds;
	while (curr)
	{
		j = -1;
		while (++j < curr->len)
			printf("%s ", curr->args[j]);
		printf("\n");
		curr = curr->next;
	}
	list_clear(&cmds);
	return EXIT_SUCCESS;
}
 