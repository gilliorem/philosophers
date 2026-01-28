#include "philos.h"

void	ft_memcpy(void *dest, void *src, size_t n)
{
	size_t	i;

	char *dest_tmp = (char*)dest;
	char *src_tmp = (char*)src;
	i = 0;
	while (i < n)
	{
		dest_tmp[i] = src_tmp[i];
		i++;
	}
}

int	now_ms(t_timer *timer)
{
	int	elapsed_ms;
	int	now_ms;
	struct	timeval now;
	
	gettimeofday(&now, NULL);
	now_ms = (now.tv_sec * 1000) + (now.tv_usec / 1000);
	elapsed_ms = now_ms - timer->start_ms;
	return (elapsed_ms);
}

void	msleep(int n)
{
	usleep(1000 * n);
}

void	set_share_resource(pthread_mutex_t *mutex, bool *resource_status, bool status)
{
	pthread_mutex_lock(mutex);
	*resource_status = status;
	pthread_mutex_unlock(mutex);
}

void	set_resource(pthread_mutex_t *mutex, void *resource, void *status, size_t res_size)
{
	pthread_mutex_lock(mutex);
	ft_memcpy(resource, status, sizeof(res_size));
	pthread_mutex_unlock(mutex);
}