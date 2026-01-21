#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <unistd.h>
#include <pthread.h>
#include <stdbool.h>

/*
missing:
- monitor
- last meal
- has eaten
- stop (flag)
- forks
*/

/* ./philos number_of_philos time_to_die time_to_eat time_to_sleep [number_times_each_philo_must_eat] */


/* prototypes of functions */

typedef struct s_simulation t_simualation;
typedef struct s_settings t_settings;
typedef struct s_table t_table;
typedef struct s_timer t_timer;
typedef struct s_philo t_philo;

typedef struct s_simulation
{
	t_settings *settings;
} t_simuation;

typedef struct s_settings
{
	int rounds; 
	int time_to_die;
	int time_to_eat;
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
//	struct timeval now;
	long start_ms;
//	long elapsed_time;
} t_timer;

typedef struct s_table
{
	int number_of_philos;
	t_philo *philo;
	pthread_mutex_t *forks;
//	pthread_t t_philos[];	
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
t_philo *init_philo();
t_philo *init_philos(int number_of_philos);

int check_number(char *av)
{
	int i = 0;
	while (av[i])
	{
		if (av[i] < '0' || av[i] > '9')
		{
			printf("NAN\n");
			return 0;
		}
		i++;
	}
	return 1;
}

t_settings *init_settings(void)
{
	t_settings *settings;
	settings = calloc(1, sizeof(t_settings));
	settings->timer = init_time();	
	settings->table = init_table();
	pthread_mutex_init(&settings->m_log, NULL);
	return settings;
}

/* Initialize a pointer to global object used throughout the program */
t_timer* init_time(void)
{
	t_timer *timer;
	timer = calloc(1, sizeof(t_timer));
	gettimeofday(&timer->start, NULL);	
	timer->start_ms = (timer->start.tv_sec * 1000) + (timer->start.tv_usec / 1000);
	return timer;
}

t_table *init_table(void)
{
	t_table *table;
	table = calloc(1, sizeof(t_table));
	return table;
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
	long elapsed_ms;
	long now_ms;
	struct timeval now;
	
	gettimeofday(&now, NULL);
	now_ms = (now.tv_sec * 1000) + (now.tv_usec / 1000);
	elapsed_ms = now_ms - timer->start_ms;
	return elapsed_ms;
}

// m for mutex
// timestamp_in_ms X [msg]
void	m_print(t_timer *timer, t_philo *philo, char *msg) 
{
	pthread_mutex_lock(&philo->global_m);
	printf("%ld %d %s\n", now_ms(timer), philo->id, msg);
	pthread_mutex_unlock(&philo->global_m);
}

void	msleep(int n)
{
	usleep(1000 * n);
}

void	*has_everyone_eaten(void *arg)
{
	t_settings *settings = (t_settings*) arg;
	int round = 0;
	while (1)
	{
		pthread_mutex_lock(&settings->m_log);
//		printf("meals:%d\n",settings->meals);
		if (settings->meals >= settings->table->number_of_philos)
		{
			//printf("***END OF ROUND %d***\n---ROUNDS left:%d---\n", round + 1, settings->rounds - (round + 1));
			round++;
			settings->meals = 0;
		}
		if (round == settings->rounds)
		{
			printf("end of the simulation\n");
			pthread_mutex_unlock(&settings->m_log);
			exit(EXIT_SUCCESS);
		}
		pthread_mutex_unlock(&settings->m_log);
		msleep(5);
	}
	return NULL;
}

void	*is_time_for_you_to_die(void *arg)
{
	t_settings *settings = (t_settings*) arg;
	settings = &(*settings);
	t_table *table = settings->table;
	t_timer *timer = settings->timer;
	// **condition de variable au lieu d'1 while true**
	int j = 0;
	while (true)
	{
		for (int i = 0; i < table->number_of_philos; i++)
		{
			//printf("now: %ld\n",now_ms(timer));
			pthread_mutex_lock(&settings->philo[i].global_m);
			settings->philo[i].time_since_last_meal = now_ms(timer) - settings->philo[i].last_meal;
			//printf("time %d ate: %d\n", settings->philo[i].id, settings->philo[i].last_meal);
			//printf("time since last meal:%d\n", settings->philo[i].time_since_last_meal);
			pthread_mutex_unlock(&settings->philo[i].global_m);
			//printf("time to die:%d\n", settings->philo[i].time_to_die);
			if (settings->philo[i].time_since_last_meal > settings->philo[i].time_to_die + 9)
			{
				printf("Time since last meal:%d > time to die:%d\n", settings->philo[i].time_since_last_meal, settings->time_to_die);
				printf("philo %d DIED\n", settings->philo[i].id);
				settings->philo[i].dead = true;
				//free_philos(table);
				//free(timer);
				free(settings->philo);
				free(table);
				free(settings);
				exit(EXIT_FAILURE);
			}
		}
		msleep(5);
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

void *eat_sleep_routine(void *arg)
{
	t_philo *philo = (t_philo *) arg;
	t_settings *settings = philo->settings;
	t_timer *timer = settings->timer;
	t_table *table = settings->table;
	philo->left_fork = &table->forks[philo->id - 1];
	philo->right_fork = &table->forks[philo->id % table->number_of_philos];
	int i = 1;
	while (i <= philo->rounds)
	{
		/*
		pthread_mutex_lock(&settings->m_log);
		printf("meals %d\n", i); 
		pthread_mutex_unlock(&settings->m_log);
		*/
		philo->has_eaten = false;
		if (philo->id % 2 == 0)
		{
			//printf("number of philos is even, philo taking right fork first.\n");
			msleep(1);
			pthread_mutex_lock(philo->right_fork);
			m_print(timer, philo, "has taken a fork");
//			printf("%ld ms %d has taken a *right* fork[%d]\n", now_ms(timer), philo->id, philo->id % table->number_of_philos);
			pthread_mutex_lock(philo->left_fork);
			m_print(timer, philo, "has taken a fork");
			//printf("%ld ms %d has taken a *left* fork[%d]\n", now_ms(timer), philo->id, philo->id -1);
		}
		else
		{
			msleep(2);
			pthread_mutex_lock(philo->left_fork);
			m_print(timer, philo, "has taken a fork");
			/*
			pthread_mutex_lock(&settings->m_log);
			printf("%ld ms %d has taken a *left* fork[%d]\n", now_ms(timer), philo->id, philo->id - 1);
			pthread_mutex_unlock(&settings->m_log);
			*/
			pthread_mutex_lock(philo->right_fork);
			m_print(timer, philo, "has taken a fork");
			/*
			pthread_mutex_lock(&settings->m_log);
			printf("%ld ms %d has taken a *right* fork[%d]\n", now_ms(timer), philo->id, ((philo->id) % table->number_of_philos));
			pthread_mutex_unlock(&settings->m_log);
			*/
		}
		/*
		pthread_mutex_lock(&settings->m_log);
		printf("%ld ms %d starts eating\n", now_ms(timer), philo->id);
		pthread_mutex_unlock(&settings->m_log);
		*/
		m_print(timer, philo, "is eating");
		msleep(philo->time_to_eat);
		philo->last_meal = now_ms(timer);
		philo->has_eaten = true;
		settings->meals++;
		//usleep(philo->time_to_eat * 1000);
		//printf("%ld ms %d is done eating. put the forks back on the table.\n", now_ms(timer), philo->id);
		pthread_mutex_unlock(philo->right_fork);
		pthread_mutex_unlock(philo->left_fork);

		/*
		printf("%ld ms %d starts sleeping.\n", now_ms(timer), philo->id);
//		usleep(philo->time_to_sleep * 1000);
		*/
		m_print(timer, philo, "is sleeping");
		msleep(philo->time_to_sleep);
		m_print(timer, philo, "is thinking");
		//printf("%ld ms %d is thinking.\n", now_ms(timer), philo->id);
		i++;
	}
	return NULL;
}

int main(int argc, char *argv[])
{
	if (argc < 5)
	{
		printf("usage: <number_of_philos> <time_to_die> <time_to_eat> <time_to_sleep> <[number_of_times_each_philo_must_eat]>\n");
	       	return 0;
	}
	t_settings *settings;
       
	settings = init_settings();
	if (argc >= 5)
	{
		for (int i = 1; i <= argc - 1; i++)
		{
			if (!check_number(argv[i]))
				return 0;
		}
		settings->table->number_of_philos = atoi(argv[1]);
		settings->time_to_die = atoi(argv[2]);
		settings->time_to_eat = atoi(argv[3]);
		settings->time_to_sleep = atoi(argv[4]);
	}
	if (argc == 6)
		settings->rounds = atoi(argv[5]);
	/*
	printf("number_of_philosophers: %d\n", settings->table->number_of_philos);
	printf("time to die: %d\n", settings->time_to_die);
	printf("time_to_eat: %d\n", settings->time_to_eat);
	printf("time_to_sleep: %d\n", settings->time_to_sleep);
	*/
	//if (argc == 6)
	//	printf("rounds: %d\n", settings->rounds);

	t_philo *philos = calloc(settings->table->number_of_philos + 1, sizeof(t_philo));
	for (int i = 0; i <= settings->table->number_of_philos; i++)
	{
		philos[i].settings = settings;
		philos[i].id = i + 1;
		philos[i].time_to_die = settings->time_to_die ; // convert ms ?
		philos[i].time_to_eat = settings->time_to_eat ;
		philos[i].time_to_sleep = settings->time_to_sleep ;
		philos[i].rounds = settings->rounds;
		pthread_mutex_init(&philos[i].global_m, NULL);
	}
	settings->table->forks = calloc(settings->table->number_of_philos + 1, sizeof(pthread_mutex_t));
	for (int i = 0; i <= settings->table->number_of_philos; i++)
		pthread_mutex_init(settings->table->forks + i, NULL);
	for (int i = 0; i < settings->table->number_of_philos; i++)
	{
		pthread_create(&philos[i].t, NULL, &eat_sleep_routine, &(philos[i]));
	}
	settings->philo = philos;
	pthread_create(&settings->monitor, NULL, &is_time_for_you_to_die, settings);
	pthread_t has_eaten_t;
	pthread_create(&has_eaten_t, NULL, &has_everyone_eaten, settings);
	for (int i = 0; i < settings->table->number_of_philos; i++)
	{
		pthread_join(philos[i].t, NULL);
	}
	pthread_join(settings->monitor, NULL);
	pthread_join(has_eaten_t, NULL);
	for (int i = 0; i <= settings->table->number_of_philos; i++)
	{
		pthread_mutex_destroy(&settings->table->forks[i]);
		pthread_mutex_destroy(&philos[i].global_m);
	}
	pthread_mutex_destroy(&settings->m_log);
	return 0;
}
