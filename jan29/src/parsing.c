#include "philos.h"

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
			printf("NAN\n");
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
	settings->table->number_of_philos = atoi(argv[1]);
	settings->time_to_die = atoi(argv[2]);
	settings->time_to_eat = atoi(argv[3]);
	settings->time_to_sleep = atoi(argv[4]);
	settings->six_argc = false;
	if (argc == 6)
	{
		settings->six_argc = true;
		settings->rounds = atoi(argv[5]);
	}
	return (1);
}