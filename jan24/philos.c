#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <unistd.h>
#include <pthread.h>
#include <stdbool.h>

/* ./philos number_of_philos time_to_die time_to_eat time_to_sleep [number_times_each_philo_must_eat] */
/* prototypes of functions */

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
	int	rounds; 
	int	time_to_die;
	int	time_to_eat;
	int time_to_sleep;
	int meals;
	pthread_mutex_t m_log;
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
	int time_since_last_meal;
	int last_meal;
	pthread_mutex_t global_m;
	bool has_eaten;
	bool dead;
	int rounds;
} t_philo; 

t_settings *init_settings(void);
t_timer	*init_time(void);
t_table	*init_table(void);
t_philo *init_philos(int number_of_philos);

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
	if (argc == 6)
		settings->rounds = atoi(argv[5]);
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
	pthread_mutex_init(&settings->m_log, NULL);
	while (i < settings->table->number_of_philos)
	{
		pthread_mutex_init(&settings->table->forks[i], NULL);
		i++;
	}	
	return (settings);
}

/* Initialize a pointer to global object used throughout the program */
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

void	free_philos(t_table *table)
{
	int	i;

	i = 0;
	while (i < table->number_of_philos)
	{
		free (&table->philo[i]);
		i++;
	}
	free(table->philo);
}

long	now_ms(t_timer *timer)
{
	long	elapsed_ms;
	long	now_ms;
	struct	timeval now;
	
	gettimeofday(&now, NULL);
	now_ms = (now.tv_sec * 1000) + (now.tv_usec / 1000);
	elapsed_ms = now_ms - timer->start_ms;
	return (elapsed_ms);
}

void	m_print(t_timer *timer, t_philo *philo, char *msg) 
{
	pthread_mutex_lock(&philo->global_m);
	printf("%ld %d %s\n", now_ms(timer), philo->id, msg);
	pthread_mutex_unlock(&philo->global_m);
}

void	m_forklog(t_timer *timer, t_philo *philo, char *msg) 
{
	long now = now_ms(timer);
	pthread_mutex_lock(&philo->global_m);
	printf("%ld %d %s\n%ld %d %s\n", now, philo->id, msg, now, philo->id, msg);
	pthread_mutex_unlock(&philo->global_m);
}

void	msleep(int n)
{
	usleep(1000 * n);
}

void	*has_everyone_eaten(void *arg)
{
	int		round;
	t_settings	*settings;

	round = 0;
	settings = (t_settings*) arg;
	while (1)
	{
		pthread_mutex_lock(&settings->m_log);
		if (settings->meals >= settings->table->number_of_philos)
		{
			round++;
			settings->meals = 0;
		}
		if (round == settings->rounds)
		{
			printf("end of the simulation\n");
			pthread_mutex_unlock(&settings->m_log);
			// clean
			exit(EXIT_SUCCESS);
		}
		pthread_mutex_unlock(&settings->m_log);
		msleep(1);
	}
	return (NULL);
}

void	*is_time_for_you_to_die(void *arg)
{
	t_settings *settings;
	t_table	*table;
	t_timer	*timer;
	int	j;
       
	settings = (t_settings*) arg;
	settings = &(*settings);
	table = settings->table;
	timer = settings->timer;
	j = 0;
	while (true)
	{
		for (int i = 0; i < table->number_of_philos; i++)
		{
			pthread_mutex_lock(&settings->philo[i].global_m);
			settings->philo[i].time_since_last_meal = now_ms(timer) - settings->philo[i].last_meal;
			pthread_mutex_unlock(&settings->philo[i].global_m);
			if (settings->philo[i].time_since_last_meal > settings->philo[i].time_to_die)
			{
				/*function*/
				printf("Time since last meal:%d > time to die:%d\n", settings->philo[i].time_since_last_meal, settings->time_to_die);
				m_print(timer, &settings->philo[i], "died");
				settings->philo[i].dead = true;
				free(settings->philo);
				free(table);
				free(settings);
				exit(EXIT_FAILURE);
			}
		}
		msleep(1);
		j++;
	}
	return (NULL);
}

/* TODO: separate cut down is_time_to_die in a function
 * that monitor routine will call
 * monitor routine will call for each philo is_time_for_you to die
 * 
void monitor_routine(void *arg)
{
	t_settings *settings = (t_settings *)settings;
	t_table *table = settings->table;
	t_philo *philo = table->philo;
	is_time_for_you_to_die(philo);
}
*/

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
		pthread_mutex_init(&philos[i].global_m, NULL);
		philos[i].left_fork = &settings->table->forks[philos->id - 1];
		philos[i].right_fork = &settings->table->forks[philos->id % settings->table->number_of_philos];
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

void	eats(t_philo *philo)
{
	t_settings	*settings;
	t_timer	*timer;

	settings = philo->settings;
	timer = settings->timer;
	takes_fork(philo);
	m_print(timer, philo, "is eating");
	msleep(philo->time_to_eat);
	pthread_mutex_lock(&settings->m_log);
	philo->last_meal = now_ms(timer);
	philo->has_eaten = true;
	settings->meals++;
	pthread_mutex_unlock(&settings->m_log);
}

void	drop_forks(t_philo *philo)
{
	pthread_mutex_unlock(philo->right_fork);	
	pthread_mutex_unlock(philo->left_fork);	
}

void	*philo_routine(void *arg)
{
	t_philo	*philo;
	t_settings	*settings;
	t_table	*table;
	int	i;

	philo = (t_philo *)arg;
	settings = philo->settings;
	table = settings->table;
	philo->left_fork = &table->forks[philo->id - 1];
	philo->right_fork = &table->forks[philo->id % table->number_of_philos];
	i = 1;
	while (i <= philo->rounds || true)
	{
		philo->has_eaten = false;
		eats(philo);
		drop_forks(philo);
		m_print(philo->settings->timer, philo, "is sleeping");
		msleep(philo->time_to_sleep);
		m_print(philo->settings->timer, philo, "is thinking");
		i++;
	}
	return (NULL);
}

/*
void *eat_sleep_routine(void *arg)
{
	t_philo *philo;
	t_settings	*settings;
	t_timer	*timer;
	t_table	*table;
	int	i;
       
	philo = (t_philo *) arg;
	settings = philo->settings;
	timer = settings->timer;
	table = settings->table;
	philo->left_fork = &table->forks[philo->id - 1];
	philo->right_fork = &table->forks[philo->id % table->number_of_philos];
	i = 1;
	while (i <= philo->rounds || true)
	{
		philo->has_eaten = false;
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
		takes_fork(philo);
		m_print(timer, philo, "is eating");
		msleep(philo->time_to_eat);
		pthread_mutex_lock(&settings->m_log);
		philo->last_meal = now_ms(timer);
		philo->has_eaten = true;
		settings->meals++;
		pthread_mutex_unlock(&settings->m_log);
		eats(philo);
//		pthread_mutex_unlock(philo->right_fork);
//		pthread_mutex_unlock(philo->left_fork);
		drop_forks(philo);
		m_print(timer, philo, "is sleeping");
		msleep(philo->time_to_sleep);
		m_print(timer, philo, "is thinking");
		i++;
	}
	return NULL;
}
*/

void free_thread(pthread_t **thread)
{
	free(thread);
}


void	start_simulation(int argc, t_settings *settings, t_philo *philos)
{
	pthread_t has_eaten_t;
	int	i;
	
	i = 0;
	while (i < settings->table->number_of_philos)
	{
		pthread_create(&philos[i].t, NULL, &philo_routine, &(philos[i]));
		i++;
	}
	pthread_create(&settings->monitor, NULL, &is_time_for_you_to_die, settings);
	if (argc == 6)
	{
		pthread_create(&has_eaten_t, NULL, &has_everyone_eaten, settings);
	}
	i = 0;
	while (i < settings->table->number_of_philos)
	{
		pthread_join(philos[i].t, NULL);
		i++;
	}
	pthread_join(settings->monitor, NULL);
	if (argc == 6)
		pthread_join(has_eaten_t, NULL);
}

void	destroy_mutexes(t_settings *settings, t_philo *philos)
{
	int	i;

	i = 0;
	while (i < settings->table->number_of_philos)
	{
		pthread_mutex_destroy(&settings->table->forks[i]);
		pthread_mutex_destroy(&philos[i].global_m);
		i++;
	}
	pthread_mutex_destroy(&settings->m_log);
}

int	main(int argc, char *argv[])
{
	t_settings *settings;
	t_table *table;
	t_philo	*philos;
//	pthread_t has_eaten_t;
	if (!check_argc(argc))
		return (0);
	settings = init_settings();
	//philos = settings->philo;
	table = settings->table;
	if (!parse_argv(argc, argv, settings))
		return (0);
	philos = calloc(settings->table->number_of_philos, sizeof(t_philo));
	settings->philo = philos;
	init_philo(philos, settings);	
	table->forks = calloc(table->number_of_philos, sizeof(pthread_mutex_t));
	/*
	//for (int i = 0; i < settings->table->number_of_philos; i++)
	//	pthread_mutex_init(&settings->table->forks[i], NULL);
	//for (int i = 0; i < settings->table->number_of_philos; i++)
	//	pthread_create(&philos[i].t, NULL, &eat_sleep_routine, &(philos[i]));
//	pthread_create(&settings->monitor, NULL, &is_time_for_you_to_die, settings);
//	if (argc == 6)
//		pthread_create(&has_eaten_t, NULL, &has_everyone_eaten, settings);

	for (int i = 0; i < table->number_of_philos; i++)
		pthread_join(philos[i].t, NULL);
	pthread_join(settings->monitor, NULL);

	if (argc == 6)
		pthread_join(has_eaten_t, NULL);
	*/
	start_simulation(argc, settings, philos);
	/*
	for (int i = 0; i < settings->table->number_of_philos; i++)
	{
		pthread_mutex_destroy(&settings->table->forks[i]);
		pthread_mutex_destroy(&philos[i].global_m);
	}
	pthread_mutex_destroy(&settings->m_log);
	*/
	destroy_mutexes(settings, philos);
	return 0;
}
