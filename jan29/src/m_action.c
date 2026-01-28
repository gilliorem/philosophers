#include "philos.h"

void	m_print(t_timer *timer, t_philo *philo, char *msg) 
{
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
	long now = now_ms(timer);
	pthread_mutex_lock(&philo->m_log);
	printf("%ld %d %s\n%ld %d %s\n", now, philo->id, msg, now, philo->id, msg);
	pthread_mutex_unlock(&philo->m_log);
}

void	m_end(t_settings *settings)
{
	pthread_mutex_lock(&settings->m_log);
	settings->end = true;
	pthread_mutex_lock(&settings->m_log);
}
