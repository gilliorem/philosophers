/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   init.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: regillio <regillio@student.42singapore.sg> +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/29 07:45:25 by regillio          #+#    #+#             */
/*   Updated: 2026/01/30 06:04:06 by regillio         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/philos.h"

t_settings	*init_settings(void)
{
	t_settings	*settings;
	int			i;

	i = 0;
	settings = ft_calloc(1, sizeof(t_settings));
	settings->timer = init_time();
	settings->table = init_table();
	pthread_mutex_init(&settings->m_end, NULL);
	pthread_mutex_init(&settings->m_log, NULL);
	pthread_mutex_init(&settings->m_meals, NULL);
	settings->end = false;
	while (i < settings->table->number_of_philos)
	{
		pthread_mutex_init(&settings->table->forks[i], NULL);
		i++;
	}
	return (settings);
}

t_timer	*init_time(void)
{
	t_timer	*timer;

	timer = ft_calloc(1, sizeof(t_timer));
	gettimeofday(&timer->start, NULL);
	timer->start_ms = (timer->start.tv_sec * 1000)
		+ (timer->start.tv_usec / 1000);
	return (timer);
}

t_table	*init_table(void)
{
	t_table	*table;

	table = ft_calloc(1, sizeof(t_table));
	return (table);
}

void	init_mutexes(t_settings *settings, pthread_mutex_t *mutex)
{
	int	i;

	i = 0;
	while (i < settings->table->number_of_philos)
	{
		pthread_mutex_init(mutex, NULL);
		i++;
	}
}

void	init_philo(t_philo *philos, t_settings *settings)
{
	int	i;

	i = 0;
	while (i < settings->table->number_of_philos)
	{
		philos[i].settings = settings;
		philos[i].id = i + 1;
		philos[i].time_to_die = settings->time_to_die ;
		philos[i].time_to_eat = settings->time_to_eat ;
		philos[i].time_to_sleep = settings->time_to_sleep ;
		philos[i].rounds = settings->rounds;
		philos[i].left_fork = &settings->table->forks[philos[i].id - 1];
		philos[i].right_fork = &settings->table->forks
		[philos[i].id % settings->table->number_of_philos];
		pthread_mutex_init(&philos[i].m_last_meal, NULL);
		pthread_mutex_init(&philos[i].m_log, NULL);
		pthread_mutex_init(&philos[i].m_has_eaten, NULL);
		i++;
	}
}
