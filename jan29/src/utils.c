/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: regillio <regillio@student.42singapore.sg> +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/29 07:59:33 by regillio          #+#    #+#             */
/*   Updated: 2026/01/29 10:15:20 by regillio         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/philos.h"

void	ft_memcpy(void *dest, void *src, size_t n)
{
	size_t	i;
	char	*dest_tmp;
	char	*src_tmp;

	dest_tmp = (char *)dest;
	src_tmp = (char *)src;
	i = 0;
	while (i < n)
	{
		dest_tmp[i] = src_tmp[i];
		i++;
	}
}

int	now_ms(t_timer *timer)
{
	struct timeval	now;
	int				now_ms;
	int				elapsed_ms;

	gettimeofday(&now, NULL);
	now_ms = (now.tv_sec * 1000) + (now.tv_usec / 1000);
	elapsed_ms = now_ms - timer->start_ms;
	return (elapsed_ms);
}

void	msleep(int n)
{
	usleep(1000 * n);
}

void	set_share_resource(pthread_mutex_t *m, bool *r_s, bool s)
{
	pthread_mutex_lock(m);
	*r_s = s;
	pthread_mutex_unlock(m);
}

void	set_resource(pthread_mutex_t *m, void *r, void *s, size_t n)
{
	pthread_mutex_lock(m);
	ft_memcpy(r, s, sizeof(n));
	pthread_mutex_unlock(m);
}
