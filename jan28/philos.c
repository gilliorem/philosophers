/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philos.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: regillio <regillio@student.42singapore.sg> +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/27 10:40:26 by regillio          #+#    #+#             */
/*   Updated: 2026/01/28 14:54:31 by regillio         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <unistd.h>
#include <pthread.h>
#include <stdbool.h>
#include <string.h>

typedef struct s_simulation t_simualation;
typedef struct s_settings t_settings;
typedef struct s_table t_table;
typedef struct s_timer t_timer;
typedef struct s_philo t_philo;

typedef struct s_simulation
{
	t_settings	*settings;
} 	t_simuation;

typedef struct s_settings
{
	bool	six_argc;
	int	rounds; 
	int	time_to_die;
	int	time_to_eat;
	int	time_to_sleep;
	int meals;
	bool	end;
	pthread_mutex_t m_end;
	pthread_mutex_t m_log;
	pthread_mutex_t	m_meals;
	pthread_t monitor;
	t_table *table;
	t_timer *timer;
	t_philo *philo;
} t_settings;

typedef struct s_timer
{
	struct timeval start;
	long start_ms;
} t_timer;

typedef struct s_table
{
	int number_of_philos;
	t_philo *philo;
	pthread_mutex_t *forks;
} t_table;

typedef struct s_philo
{
	t_settings *settings;
	int id;
	pthread_t t;
	pthread_mutex_t *left_fork;
	pthread_mutex_t *right_fork;
	int time_to_die;
	int time_to_eat;
	int time_to_sleep;
	int t_last_meal;
	int last_meal;
	pthread_mutex_t m_last_meal;
	pthread_mutex_t	m_log;
	bool	has_eaten;
	pthread_mutex_t	m_has_eaten;
	bool dead;
	int rounds;
} t_philo; 

t_settings *init_settings(void);
t_timer	*init_time(void);
t_table	*init_table(void);
t_philo *init_philos(int number_of_philos);
void	start_simulation(t_settings *settings, t_philo *philos);
void	end_simulation(void *arg);
void	*philo_routine(void *arg);
void	*monitor_routine(void *arg);

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

t_settings	*init_settings(void)
{
	t_settings	*settings;
	int		i;

	i = 0;
	settings = calloc(1, sizeof(t_settings));
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

t_timer*	init_time(void)
{
	t_timer	*timer;

	timer = calloc(1, sizeof(t_timer));
	gettimeofday(&timer->start, NULL);	
	timer->start_ms = (timer->start.tv_sec * 1000) + (timer->start.tv_usec / 1000);
	return (timer);
}

t_table	*init_table(void)
{
	t_table	*table;

	table = calloc(1, sizeof(t_table));
	return (table);
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

void	msleep(int n)
{
	usleep(1000 * n);
}

void	m_end(t_settings *settings)
{
	pthread_mutex_lock(&settings->m_log);
	settings->end = true;
	pthread_mutex_lock(&settings->m_log);
}

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
	t_philo *philo;

	philo = settings->philo;
	table = settings->table;
	free(settings->philo);
	free(table->forks);
	free(settings->timer);
	free(table);
	free(settings);
}

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
		*round+=1;
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
	t_settings *settings;
	int	round;
	int	i;
       
	settings = (t_settings*) arg;
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
		philos[i].right_fork = &settings->table->forks[philos[i].id % settings->table->number_of_philos];
		pthread_mutex_init(&philos[i].m_last_meal, NULL);
		pthread_mutex_init(&philos[i].m_log, NULL);
		pthread_mutex_init(&philos[i].m_has_eaten, NULL);
		i++;
	}
}

void	takes_fork(t_philo *philo)
{
	t_settings	*settings;
	t_timer	*timer;

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

void	set_share_resource(pthread_mutex_t *mutex, bool *resource_status, bool status)
{
	pthread_mutex_lock(mutex);
	*resource_status = status;
	pthread_mutex_unlock(mutex);
}

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

void	set_resource(pthread_mutex_t *mutex, void *resource, void *status, size_t res_size)
{
	pthread_mutex_lock(mutex);
	ft_memcpy(resource, status, sizeof(res_size));
	pthread_mutex_unlock(mutex);
}

void	drop_forks(t_philo *philo)
{
	pthread_mutex_unlock(philo->right_fork);	
	pthread_mutex_unlock(philo->left_fork);	
}

void	eats(t_philo *philo)
{
	t_settings	*settings;
	t_timer	*timer;
	t_table	*table;
	int	tmp;

	settings = philo->settings;
	table = settings->table;
	philo->left_fork = &table->forks[philo->id - 1];
	philo->right_fork = &table->forks[philo->id % table->number_of_philos];
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

void	*philo_routine(void *arg)
{
	t_philo	*philo;
	t_settings	*settings;
	int	i;

	philo = (t_philo *)arg;
	settings = philo->settings;
	i = 1;
	while (!check_end(settings))
	{
		set_share_resource(&philo->m_has_eaten, &philo->has_eaten, false);
		eats(philo);
		if (check_end(settings))
			break;
		m_print(philo->settings->timer, philo, "is sleeping");
		msleep(philo->time_to_sleep);
		if (check_end(settings))
			break;
		m_print(philo->settings->timer, philo, "is thinking");
		i++;
		msleep(1);
	}
	return (NULL);
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
//	for (int i = 0; i < settings->table->number_of_philos; i++)
//		pthread_mutex_init(&settings->table->forks[i], NULL);
	start_simulation(settings, settings->philo);
	/*
	for (int i = 0; i < settings->table->number_of_philos; i++)
	{
		pthread_mutex_destroy(&settings->table->forks[i]);
		pthread_mutex_destroy(&philos[i].m_log);
	}
//	pthread_mutex_destroy(&settings->m_log);
	*/
	destroy_mutexes(settings, philos);
	printf("end simulation\n");
	clean(settings);
	return (0);
}
