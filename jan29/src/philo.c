/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philo.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: regillio <regillio@student.42singapore.sg> +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/29 07:55:35 by regillio          #+#    #+#             */
/*   Updated: 2026/01/29 10:17:15 by regillio         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/philos.h"

void	takes_fork(t_philo *philo)
{
	t_settings	*settings;
	t_timer		*timer;

	settings = philo->settings;
	timer = settings->timer;
	if (philo->id % 2 == 0)
	{
		msleep(1);
		pthread_mutex_lock(philo->right_fork);
		pthread_mutex_lock(philo->left_fork);
		m_forklog(timer, philo, "has taken a fork");
	}
	else
	{
		pthread_mutex_lock(philo->left_fork);
		m_print(timer, philo, "has taken a fork");
		pthread_mutex_lock(philo->right_fork);
		m_print(timer, philo, "has taken a fork");
	}
}

void	drop_forks(t_philo *philo)
{
	pthread_mutex_unlock(philo->right_fork);
	pthread_mutex_unlock(philo->left_fork);
}

void	eats(t_philo *philo)
{
	t_settings	*settings;
	t_timer		*timer;
	t_table		*table;
	int			tmp;

	settings = philo->settings;
	table = settings->table;
	timer = settings->timer;
	takes_fork(philo);
	m_print(timer, philo, "is eating");
	msleep(philo->time_to_eat);
	drop_forks(philo);
	tmp = now_ms(timer);
	set_resource(&philo->m_last_meal, &philo->last_meal, &tmp, sizeof(tmp));
	set_share_resource(&philo->m_has_eaten, &philo->has_eaten, true);
	pthread_mutex_lock(&settings->m_meals);
	settings->meals++;
	pthread_mutex_unlock(&settings->m_meals);
}

int	lonely_philo(t_settings *settings, t_philo *philo)
{
	if (settings->table->number_of_philos == 1)
	{
		m_print(settings->timer, philo, "has taken a fork");
		msleep(settings->time_to_die);
		return (1);
	}
	return (0);
}

void	*philo_routine(void *arg)
{
	t_settings	*settings;
	t_philo		*philo;
	int			i;

	philo = (t_philo *)arg;
	settings = philo->settings;
	i = 1;
	while (!check_end(settings))
	{
		set_share_resource(&philo->m_has_eaten, &philo->has_eaten, false);
		if (lonely_philo(settings, philo))
			break ;
		eats(philo);
		if (check_end(settings))
			break ;
		m_print(philo->settings->timer, philo, "is sleeping");
		msleep(philo->time_to_sleep);
		if (check_end(settings))
			break ;
		m_print(philo->settings->timer, philo, "is thinking");
		i++;
		msleep(1);
	}
	return (NULL);
}
