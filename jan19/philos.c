#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <unistd.h>
#include <pthread.h>

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
	pthread_t t_log;
	t_table *table;
	t_timer *timer;
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

void	*log_routine(void *arg)
{
	char* msg = (char*) arg;
	printf("%s\n", msg);

	return NULL;
}

t_settings *init_settings(void)
{
	t_settings *settings;
	settings = calloc(1, sizeof(t_settings));
	settings->timer = init_time();	
	settings->table = init_table();
	pthread_create(&settings->t_log, NULL, &log_routine, NULL);
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

typedef struct s_global
{
	int left_f;
} t_global;

t_global *init_global()
{
	t_global *global = calloc(1, sizeof(t_global));
	global->left_f = 0;
	return global;
}


void *eat_sleep_routine(void *arg)
{
	t_philo *philo = (t_philo *) arg;
	t_settings *settings = philo->settings;
	t_timer *timer = settings->timer;
	t_table *table = settings->table;
	pthread_mutex_t log;
	pthread_mutex_init(&log, NULL);
	philo->left_fork = &table->forks[philo->id - 1];
	philo->right_fork = &table->forks[philo->id % table->number_of_philos];
	int i = 1;
	while (i <= philo->rounds)
	{
		pthread_mutex_lock(&log);
		printf("ROUND %d\n", i);
		pthread_mutex_unlock(&log);
		if (philo->id % 2 == 0)
			usleep(200);
		pthread_mutex_lock(philo->left_fork);
//		pthread_mutex_lock(&log);
		printf("%ld ms %d has taken a *left* fork[%d]\n", now_ms(timer), philo->id, philo->id - 1);
//		pthread_mutex_unlock(&log);
		pthread_mutex_lock(philo->right_fork);
//		pthread_mutex_lock(&log);
		printf("%ld ms %d has taken a *right* fork[%d]\n", now_ms(timer), philo->id, ((philo->id) % table->number_of_philos));
		printf("%ld ms %d starts eating\n", now_ms(timer), philo->id);
//		pthread_mutex_unlock(&log);
		usleep(philo->time_to_eat);
		printf("%ld ms %d is done eating. put the forks back on the table.\n", now_ms(timer), philo->id);
		pthread_mutex_unlock(philo->right_fork);
		pthread_mutex_unlock(philo->left_fork);

		printf("%ld ms %d starts sleeping.\n", now_ms(timer), philo->id);
		usleep(philo->time_to_sleep);
		printf("%ld ms %d is thinking.\n", now_ms(timer), philo->id);
		i++;
		pthread_mutex_lock(&log);
		printf("Round %d ended.\n",i);
		printf("\n");
		pthread_mutex_unlock(&log);
	}
	return NULL;
}

void sleep_routine(t_table *table, t_timer *timer)
{
	printf("PHILO X is sleeping\n");
//	usleep(table->input_from_av[3] * 1000);
}

int main(int argc, char *argv[])
{
	if (argc < 5){printf("usage: <number_of_philos> <time_to_die> <time_to_eat> <time_to_sleep> <[rounds]>\n"); return 0;}
	t_settings *settings = init_settings();
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
	if (argc == 6)
		printf("rounds: %d\n", settings->rounds);

	t_philo *philos = calloc(settings->table->number_of_philos + 1, sizeof(t_philo));
	for (int i = 0; i <= settings->table->number_of_philos; i++)
	{
		philos[i].settings = settings;
		philos[i].id = i + 1;
		philos[i].time_to_die = settings->time_to_die * 1000; // convert ms ?
		philos[i].time_to_eat = settings->time_to_eat * 1000;
		philos[i].time_to_sleep = settings->time_to_sleep * 1000;
		philos[i].rounds = settings->rounds;
	}
	settings->table->forks = calloc(settings->table->number_of_philos + 1, sizeof(pthread_mutex_t));
	for (int i = 0; i <= settings->table->number_of_philos; i++)
		pthread_mutex_init(settings->table->forks + i, NULL);
	for (int i = 0; i < settings->table->number_of_philos; i++)
	{
		pthread_create(&philos[i].t, NULL, &eat_sleep_routine, &(philos[i]));
	}
	for (int i = 0; i < settings->table->number_of_philos; i++)
	{
		pthread_join(philos[i].t, NULL);
	}
	
	for (int i = 0; i <= settings->table->number_of_philos; i++)
		pthread_mutex_destroy(&settings->table->forks[i]);
	return 0;
}
