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
//	pthread_t t_philos[];	
} t_table;

typedef struct s_philo
{
	t_settings *settings;
	int id;
	pthread_t t;
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

t_settings *init_settings(void)
{
	t_settings *settings;
	settings = calloc(1, sizeof(t_settings));
	settings->timer = init_time();	
	settings->table = init_table();
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

void *eat_routine(void *arg)
{
	t_philo *philo = (t_philo *) arg;
	t_settings *settings = philo->settings;
	t_timer *timer = settings->timer;
	int i = 0;
	while (i <= philo->rounds)
	{
		printf("%ld ms %d starts eating\n", now_ms(timer), philo->id);
		usleep(philo->time_to_eat);
		i++;
	}
	//printf("%ld ms %d has taken a fork\n", philo->timer->elapsed_time, philo->id);
	//get_timestamp(philo->timer);
//	printf("%ld ms %d has taken a fork\n", now_ms(timer), philo->id);	
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
	printf("number_of_philosophers: %d\n", settings->table->number_of_philos);
	printf("time to die: %d\n", settings->time_to_die);
	printf("time_to_eat: %d\n", settings->time_to_eat);
	printf("time_to_sleep: %d\n", settings->time_to_sleep);
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
	for (int i = 0; i < settings->table->number_of_philos; i++)
	{
		pthread_create(&philos[i].t, NULL, &eat_routine, &(philos[i]));
	}
	for (int i = 0; i < settings->table->number_of_philos; i++)
	{
		pthread_join(philos[i].t, NULL);
	}
	//t_philo *philos = init_philos(settings->table->number_of_philos);	
/*

//	eat_routine(&table);
	pthread_t philos[table.input_from_av[0]];
	while (1)
	{
		get_timestamp(table.philo->timer);
		for (int i = 0; i < table.input_from_av[0]; i++)
		{
			table.philo[i].id = i + 1;
			table.philo[i].time_to_die = table.input_from_av[1] * 1000;
			table.philo[i].time_to_eat = table.input_from_av[2] * 1000;
			table.philo[i].time_to_sleep = table.input_from_av[3] * 1000;
			pthread_create(philos + i, NULL, &eat_routine, &(table.philo[i]));
		}
		for (int i = 0; i < table.input_from_av[0]; i++)
			pthread_join(philos[i], NULL);
	}
	*/
	return 0;
}
