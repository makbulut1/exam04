/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   microshell.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: makbulut <makbulut@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/09/19 20:03:39 by makbulut          #+#    #+#             */
/*   Updated: 2022/09/22 13:03:17 by makbulut         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

int	ft_strlen(char *s)
{
	int	i;

	i = 0;
	while (s[i])
		i++;
	return (i);
}

void	ft_write(char *s1, char *s2)
{
	write(STDERR_FILENO, s1, ft_strlen(s1));
	write(STDERR_FILENO, s2, ft_strlen(s2));
	write(STDERR_FILENO, "\n", 1);
}

void	ft_close(int *fd)
{
	if (*fd != -1)
		close(*fd);
	*fd = -1;
}

int	main(int ac, char **av, char **env)
{
	int i = 0, cmd = 1, pid = 0, fd_in[2] = {-1, -1}, fd_out[2] = {-1, -1};

	if (ac < 2)
		return (0);
	while (av[i])
	{
		i++;
		while (av[i] && av[i][0] == ';')
			i++;
		if (!av[i])
			break ;
		if (strcmp(av[i], "cd") == 0)
		{
			cmd = i;
			while (av[i] && av[i][0] != ';')
				i++;
			if (i - cmd != 2)
				ft_write("error: cd: bad arguments", "");
			else if (chdir(av[cmd + 1]) == -1)
				ft_write("error: cd: cannot change \
				directory to ", av[cmd + 1]);
		}
		else
		{
			cmd = i;
			i++;
			while (av[i])
			{
				if (av[i][0] == '|')
				{
					if (pipe(fd_out) == -1)
						ft_write("error: fatal", "");
					break ;
				}
				else if (av[i][0] == ';')
					break ;
				i++;
			}
			pid = fork();
			if (pid == 0)
			{
				if (fd_out[1] != -1)
				{
					close(fd_out[0]);
					dup2(fd_out[1], STDOUT_FILENO);
					close(fd_out[1]);
				}
				if (fd_in[0] != -1)
				{
					close(fd_in[1]);
					dup2(fd_in[0], STDIN_FILENO);
					close(fd_in[0]);
				}
				av[i] = NULL;
				if (execve(av[cmd], &av[cmd], env) == -1)
					ft_write("error: cannot execute ", av[cmd]);
				exit(0);
			}
			else if (pid == -1)
				ft_write("error: fatal", "");
			ft_close(&fd_in[0]);
			ft_close(&fd_in[1]);
			fd_in[0] = fd_out[0];
			fd_in[1] = fd_out[1];
			fd_out[0] = -1;
			fd_out[1] = -1;
			if (pid > 0)
				waitpid(pid, NULL, WUNTRACED);
		}
	}
	return (0);
}
