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
	bool	six_argc;
	int	rounds; 
	int	time_to_die;
	int	time_to_eat;
	int time_to_sleep;
	int meals;
	bool	end;
	pthread_mutex_t m_log;
	pthread_mutex_t m_dead;
	pthread_t monitor;
	pthread_t has_eaten_t;
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
void	start_simulation(int argc, t_settings *settings, t_philo *philos);
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
		//
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
	pthread_mutex_init(&settings->m_log, NULL);
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

void	m_died(t_timer *timer, t_philo *philo, char *msg)
{
	m_print(timer, philo, msg);
	pthread_mutex_lock(&philo->settings->m_dead);
	philo->settings->end = true;
	pthread_mutex_unlock(&philo->settings->m_dead);
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

void	m_end(t_settings *settings)
{
	pthread_mutex_lock(&settings->m_log);
	settings->end = true;
	pthread_mutex_lock(&settings->m_log);
}

/*
void	*has_everyone_eaten(void *arg)
{
	int		round;
	t_settings	*settings;

	round = 0;
	settings = (t_settings*) arg;
//	while (1)
	while (settings->end == false)
	{
		//pthread_mutex_lock(&settings->m_log);
//		printf("rounds:%d\nsettings rounds:%d\n",
//			round, settings->rounds);
		if (settings->meals >= settings->table->number_of_philos)
		{
			round++;
			settings->meals = 0;
		}
		if (round == settings->rounds)
		{
			printf("IN\n");
			//pthread_mutex_lock(&settings->m_log);
			//printf("SIMULATION ENDS\n");
			//settings->end = true;
			m_end(settings);
			return (NULL);
			//pthread_mutex_unlock(&settings->m_log);
//			end_simulation(settings, settings->philo);
			//end_simulation(settings);
			//printf("END SIM() RAN\n");
			//end_simulation(6, settings, settings->philo);
			//exit(EXIT_FAILURE);
		}
		//pthread_mutex_unlock(&settings->m_log);
		msleep(1);
	}
	return (NULL);
}
*/

bool	has_everyone_eaten(t_settings *settings)
{
	int	i;

	i = 0;
	while (i < settings->table->number_of_philos)
	{
		pthread_mutex_lock(&settings->philo[i].global_m);
		if (settings->philo[i].rounds < settings->rounds)
		{
			pthread_mutex_unlock(&settings->philo[i].global_m);
			return (false);
		}
		pthread_mutex_unlock(&settings->philo[i].global_m);
		i++;
	}
	return (true);
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

void	start_simulation(int argc, t_settings *settings, t_philo *philos)
{
	t_table	*table;
	int	i;
	
	i = 0;
	table = settings->table;
	table->forks = calloc(table->number_of_philos, sizeof(pthread_mutex_t));
	while (i < table->number_of_philos)
	{
		pthread_mutex_init(&settings->table->forks[i], NULL);
		i++;
	}
	printf("MUTEXES INIT\n");
	i = 0;
	while (i < settings->table->number_of_philos)
	{
		pthread_create(&philos[i].t, NULL, &philo_routine, &(philos[i]));
		i++;
	}
	printf("PHILO THREADS CREATED\n");
	pthread_create(&settings->monitor, NULL, &monitor_routine, settings);
	printf("MONITOR THREAD CREATED\n");
	//if (argc == 6)
	//	pthread_create(&settings->has_eaten_t, NULL, &has_everyone_eaten, settings);
	printf("HAS EATEN THREAD CREATED\n");
	i = 0;
	while (i < settings->table->number_of_philos)
	{
		pthread_join(philos[i].t, NULL);
		i++;
	}
	printf("PHILO THREADS JOINED\n");
	if (argc == 6)
	pthread_join(settings->has_eaten_t, NULL);
	pthread_join(settings->monitor, NULL);
	destroy_mutexes(settings, settings->philo);
	clean(settings);
	printf("end of simulation\n");
	exit(EXIT_SUCCESS);
}

void	end_simulation(void *arg)
{
	t_settings	*settings;
	t_philo	*philos;
	int	i;

	i = 0;
	settings = (t_settings *)arg;
	philos = settings->philo;
	while (i < settings->table->number_of_philos)
	{
		pthread_join(philos[i].t, NULL);
		i++;
	}
	printf("PHILO THREADS JOINED\n");
	//if (argc == 6)
	//	pthread_join(settings->has_eaten_t, NULL);
	// pthread_join(settings->monitor, NULL);
	// destroy_mutexes(settings, settings->philo);
	// clean(settings);
	// printf("end of simulation\n");
	// exit(EXIT_SUCCESS);
}

void	m_dead(t_settings *settings)
{
	pthread_mutex_lock(&settings->m_log);
	settings->end = true;
	pthread_mutex_unlock(&settings->m_log);
}

bool	is_time_to_die(void *arg, t_philo *philo)
{
	t_settings	*settings;
	int	i;

	settings = (t_settings*) arg;
	settings = &(*settings);	
	i = 0;
	while (i < settings->table->number_of_philos)
	{
		pthread_mutex_lock(&settings->philo[i].global_m);
		settings->philo[i].time_since_last_meal = now_ms(settings->timer) - settings->philo[i].last_meal;
		pthread_mutex_unlock(&settings->philo[i].global_m);
		if (settings->philo[i].time_since_last_meal > settings->philo[i].time_to_die)
		{
			m_print(settings->timer, &settings->philo[i], "died");
			settings->philo[i].dead = true;
			return (true);
		}
		i++;
	}
	return (false);
}

void	end_philo_threads(t_settings *settings, t_philo *philos)
{
	//printf("ending threads\n");
	for (int i = 0; i < settings->table->number_of_philos; i++)
	{
		// printf("ending thread at address:%p\n", &philos[i].t);
		pthread_join(philos[i].t, NULL);
	}
	//printf("all threads ended\n");
}

void	*monitor_routine(void *arg)
{
	t_settings *settings;
	t_table	*table;
	t_timer	*timer;
       
	settings = (t_settings*) arg;
//	settings = &(*settings);
	table = settings->table;
	timer = settings->timer;
	
	//	while (true)
	//while (stop == false)
	while (settings->end == false)
	{
		/*
		if (is_time_to_die(settings, settings->philo))
		{
			pthread_mutex_lock(&settings->m_log);
			settings->end = true;
			pthread_mutex_lock(&settings->m_log);
		}
		*/
		for (int i = 0; i < table->number_of_philos; i++)
		{
			pthread_mutex_lock(&settings->philo[i].global_m);
			settings->philo[i].time_since_last_meal = now_ms(timer) - settings->philo[i].last_meal;
			pthread_mutex_unlock(&settings->philo[i].global_m);
			if (settings->philo[i].time_since_last_meal > settings->philo[i].time_to_die)
			{
				settings->end = true;
				m_died(timer, &settings->philo[i], "died");
				return (NULL);
				//end_threads(settings, settings->philo);
				//return NULL;
				//printf("Time since last meal:%d > time to die:%d\n", settings->philo[i].time_since_last_meal, settings->time_to_die);
				//m_print(timer, &settings->philo[i], "died");
				//settings->philo[i].dead = true;
				/*
				free(settings->philo);
				free(table);
				free(settings);
				*/
				//exit(EXIT_FAILURE);
			}
		}
		/*
		if (is_time_to_die(settings, settings->philo))
			end_simulation(settings, settings->philo);
		if (is_time_to_die(settings, settings->philo))
			m_dead(settings);
		*/
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
		pthread_mutex_init(&philos[i].global_m, NULL);
		philos[i].left_fork = &settings->table->forks[philos[i].id - 1];
		philos[i].right_fork = &settings->table->forks[philos[i].id % settings->table->number_of_philos];
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
//	printf("in routine, philo thread address:%p\n", &philo->t);
//	while (i <= philo->rounds || true)
	while (settings->end == false)
	{
		philo->has_eaten = false;
		eats(philo);
		if (settings->end == true)
		{
			//m_died(settings->timer, philo, "died");
			break;
		}
		drop_forks(philo);
		m_print(philo->settings->timer, philo, "is sleeping");
		msleep(philo->time_to_sleep);
		if (settings->end == true)
		{
			//m_died(settings->timer, philo, "died");
			break;
		}
		m_print(philo->settings->timer, philo, "is thinking");
		i++;
	}
	//m_died(settings->timer, philo, "died");
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


int	main(int argc, char *argv[])
{
	t_settings *settings;
	t_table *table;
	t_philo	*philos;
//	pthread_t has_eaten_t;
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

	for (int i = 0; i < settings->table->number_of_philos; i++)
		pthread_mutex_init(&settings->table->forks[i], NULL);
	for (int i = 0; i < settings->table->number_of_philos; i++)
	{
		pthread_create(&philos[i].t, NULL, &philo_routine, &(philos[i]));
		//printf("philo thread address: %p\n", &philos[i].t);
	}
	pthread_create(&settings->monitor, NULL, &monitor_routine, settings);
	//printf("thread monitor address:%p\n", &settings->monitor);
	pthread_join(settings->monitor, NULL);
	//printf("ended monitor thread:%p\n",  &settings->monitor);
	end_philo_threads(settings, settings->philo);
	//printf("ended philo threads\n");
	
	
//	if (argc == 6)
//		pthread_create(&has_eaten_t, NULL, &has_everyone_eaten, settings);
	//start_simulation(argc, settings, philos);
	//end_simulation(settings);
	//for (int i = 0; i < table->number_of_philos; i++)
	//	pthread_join(philos[i].t, NULL);
	//printf("philo thread ended\n");
	
//	if (argc == 6)
//		pthread_join(has_eaten_t, NULL);
	//	pthread_join(settings->has_eaten_t, NULL);
	//printf("has eaten thread ended\n");
	//start_simulation(argc, settings, philos);
	//if (settings->end == true)
	//	end_simulation(argc, settings, philos);
	for (int i = 0; i < settings->table->number_of_philos; i++)
	{
		pthread_mutex_destroy(&settings->table->forks[i]);
		pthread_mutex_destroy(&philos[i].global_m);
	}
	pthread_mutex_destroy(&settings->m_log);
	destroy_mutexes(settings, philos);
	printf("end simulation\n");
	clean(settings);
	return 0;
}
