/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   monitor.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: regillio <regillio@student.42singapore.sg> +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/29 07:51:30 by regillio          #+#    #+#             */
/*   Updated: 2026/01/29 07:52:48 by regillio         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/philos.h"

int	check_last_meal(t_settings *settings, t_philo *philo)
{
	pthread_mutex_lock(&philo->m_last_meal);
	philo->t_last_meal = now_ms(settings->timer) - philo->last_meal;
	if (philo->t_last_meal > philo->time_to_die)
	{
		pthread_mutex_unlock(&philo->m_last_meal);
		return (1);
	}
	pthread_mutex_unlock(&philo->m_last_meal);
	return (0);
}

int	everyone_ate(t_settings *settings, int *round)
{
	if (!settings->six_argc)
		return (0);
	pthread_mutex_lock(&settings->m_meals);
	if (settings->meals >= settings->table->number_of_philos)
	{
		*round += 1;
		settings->meals = 0;
	}
	pthread_mutex_unlock(&settings->m_meals);
	if (*round == settings->rounds)
	{
		pthread_mutex_lock(&settings->m_end);
		settings->end = true;
		pthread_mutex_unlock(&settings->m_end);
		return (1);
	}
	return (0);
}

int	check_end(t_settings *settings)
{
	pthread_mutex_lock(&settings->m_end);
	if (settings->end == true)
	{
		pthread_mutex_unlock(&settings->m_end);
		return (1);
	}
	pthread_mutex_unlock(&settings->m_end);
	return (0);
}

void	*monitor_routine(void *arg)
{
	t_settings	*settings;
	int			round;
	int			i;

	settings = (t_settings *) arg;
	round = 0;
	while (!check_end(settings))
	{
		i = 0;
		while (i < settings->table->number_of_philos)
		{
			if (check_last_meal(settings, &settings->philo[i]))
			{
				m_died(settings->timer, &settings->philo[i], "died");
				return (NULL);
			}
			if (everyone_ate(settings, &round))
				return (NULL);
			i++;
		}
		msleep(1);
	}
	return (NULL);
}
