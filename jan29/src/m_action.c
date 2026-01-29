/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   m_action.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: regillio <regillio@student.42singapore.sg> +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/29 07:48:44 by regillio          #+#    #+#             */
/*   Updated: 2026/01/29 10:42:05 by regillio         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/philos.h"

void	m_print(t_timer *timer, t_philo *philo, char *msg)
{
	if (check_end(philo->settings))
		return ;
	pthread_mutex_lock(&philo->m_log);
	printf("%d %d %s\n", now_ms(timer), philo->id, msg);
	pthread_mutex_unlock(&philo->m_log);
}

void	m_died(t_timer *timer, t_philo *philo, char *msg)
{
	m_print(timer, philo, msg);
	pthread_mutex_lock(&philo->settings->m_end);
	philo->settings->end = true;
	pthread_mutex_unlock(&philo->settings->m_end);
}

void	m_forklog(t_timer *timer, t_philo *philo, char *msg)
{
	long	now;

	if (check_end(philo->settings))
		return ;
	now = now_ms(timer);
	pthread_mutex_lock(&philo->m_log);
	printf("%ld %d %s\n%ld %d %s\n", now, philo->id, msg, now, philo->id, msg);
	pthread_mutex_unlock(&philo->m_log);
}
