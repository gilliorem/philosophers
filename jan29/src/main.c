/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philos.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: regillio <regillio@student.42singapore.sg> +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/27 10:40:26 by regillio          #+#    #+#             */
/*   Updated: 2026/01/29 06:55:25 by regillio         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philos.h"

void	start_simulation(t_settings *settings, t_philo *philo)
{
	int	i;

	i = 0;
	while (i < settings->table->number_of_philos)
	{
		pthread_create(&philo[i].t, NULL, &philo_routine, &philo[i]);	
		i++;
	}
	pthread_create(&settings->monitor, NULL, &monitor_routine, settings);
	pthread_join(settings->monitor, NULL);
	i = 0;
	while (i < settings->table->number_of_philos)
	{
		pthread_join(philo[i].t, NULL);
		i++;
	}
}

int	main(int argc, char *argv[])
{
	t_settings *settings;
	t_table *table;
	t_philo	*philos;

	if (!check_argc(argc))
		return (0);
	settings = init_settings();
	philos = settings->philo;
	table = settings->table;
	if (!parse_argv(argc, argv, settings))
		return (0);
	philos = calloc(settings->table->number_of_philos, sizeof(t_philo));
	settings->philo = philos;
	table->forks = calloc(table->number_of_philos, sizeof(pthread_mutex_t));
	init_philo(philos, settings);	
	start_simulation(settings, settings->philo);
	destroy_mutexes(settings, philos);
	clean(settings);
	return (0);
}
