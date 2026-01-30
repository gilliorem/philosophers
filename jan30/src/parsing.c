/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parsing.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: regillio <regillio@student.42singapore.sg> +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/29 07:52:58 by regillio          #+#    #+#             */
/*   Updated: 2026/01/30 06:11:00 by regillio         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/philos.h"

int	check_argc(int argc)
{
	if (argc < 5 || argc > 6)
	{
		printf("usage: <number_of_philos> <time_to_die> <time_to_eat> <time_to_sleep>\
 <[number_of_times_each_philo_must_eat]>\ntime is in ms\n");
		return (0);
	}
	return (1);
}

int	check_number(char *av)
{
	int	i;

	i = 0;
	while (av[i])
	{
		if (av[i] < '0' || av[i] > '9')
		{
			printf("NOT A POSITIVE NUMBER\n");
			return (0);
		}
		i++;
	}
	return (1);
}

int	parse_argv(int argc, char *argv[], t_settings *settings)
{
	int	i;

	i = 1;
	while (i <= argc - 1)
	{
		if (!check_number(argv[i]))
			return (0);
		i++;
	}
	settings->table->number_of_philos = ft_atoi(argv[1]);
	settings->time_to_die = ft_atoi(argv[2]);
	settings->time_to_eat = ft_atoi(argv[3]);
	settings->time_to_sleep = ft_atoi(argv[4]);
	settings->six_argc = false;
	if (argc == 6)
	{
		settings->six_argc = true;
		settings->rounds = ft_atoi(argv[5]);
	}
	return (1);
}
