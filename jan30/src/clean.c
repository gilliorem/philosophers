/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   clean.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: regillio <regillio@student.42singapore.sg> +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/29 07:44:54 by regillio          #+#    #+#             */
/*   Updated: 2026/01/29 07:45:16 by regillio         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/philos.h"

void	destroy_mutexes(t_settings *settings, t_philo *philos)
{
	int	i;

	i = 0;
	while (i < settings->table->number_of_philos)
	{
		pthread_mutex_destroy(&settings->table->forks[i]);
		pthread_mutex_destroy(&philos[i].m_last_meal);
		pthread_mutex_destroy(&philos[i].m_log);
		pthread_mutex_destroy(&philos[i].m_has_eaten);
		i++;
	}
	pthread_mutex_destroy(&settings->m_log);
	pthread_mutex_destroy(&settings->m_end);
	pthread_mutex_destroy(&settings->m_meals);
}

void	clean(t_settings *settings)
{
	t_table	*table;
	t_philo	*philo;

	philo = settings->philo;
	table = settings->table;
	free(settings->philo);
	free(table->forks);
	free(settings->timer);
	free(table);
	free(settings);
}
