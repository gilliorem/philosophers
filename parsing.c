#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <unistd.h>
#include <pthread.h>

/* ./philos number_of_philos time_to_die time_to_eat time_to_sleep [number_times_each_philo_must_eat] */


/* prototypes of functions */

long now_ms();
long elapsed_ms();

typedef struct s_table t_table;
typedef struct s_timer t_timer;
typedef struct s_philo t_philo;

typedef struct s_settings
{
	int rounds; 
	t_table *table;
	t_timer *timer;
} t_settings;

typedef struct s_timer
{
	struct timeval simulation_start;
	long starting_time;
	struct timeval end;
	long elapsed_time;
} t_timer;

typedef struct s_table
{
	int number_of_philos;
//	int *input_from_av;
	int time_to_die;
	int time_to_eat;
	int time_to_sleep;
	t_philo *philo;
	pthread_t t_philos[];	
} t_table;

typedef struct s_philo
{
	t_settings *settings;
	int id;
	int time_to_die;
	int time_to_eat;
	int time_to_sleep;
} t_philo; 

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

void	start_stopwatch(t_timer *timer)
{
	gettimeofday(&timer->simulation_start, NULL);
	timer->elapsed_time = 0;
	timer->starting_time = (timer->simulation_start.tv_sec * 1000) + (timer->simulation_start.tv_usec / 1000);
}
// usleep n'est pas la dedans: usleep est dans la routine.
void	get_timestamp(t_timer *timer)
{
	//usleep(time_to_sleep * 1000); // suspends main thread during time_to_sleep ms (0.5 sec); => usleep goes in the routine itseld
	gettimeofday(&timer->end, NULL);
	timer->elapsed_time = (timer->end.tv_sec - timer->simulation_start.tv_sec) * 1000; 
	timer->elapsed_time += (timer->end.tv_usec - timer->simulation_start.tv_usec) / 1000;
	printf("%ld ms ", timer->elapsed_time);
}

void *eat_routine(void *arg)
{
	t_philo *philo = (t_philo *) arg;
	//printf("%ld ms %d has taken a fork\n", philo->timer->elapsed_time, philo->id);
	//get_timestamp(philo->timer);
	printf("%d has taken a fork\n", philo->id);	
	printf("%d starts eating\n", philo->id);
	usleep(philo->time_to_eat);
	return NULL;
}

void sleep_routine(t_table *table, t_timer *timer)
{
	printf("PHILO X is sleeping\n");
	usleep(table->input_from_av[3] * 1000);
}

int main(int argc, char *argv[])
{
	//start_stopwatch(timer);
	if (argc < 5){printf("usage: <number_of_philos> <time_to_die> <time_to_eat> <time_to_sleep> <[rounds]>\n"); return 0;}
	//t_table table;
	//table.input_from_av = calloc(6, sizeof(int));
	//table.philo = calloc(atoi(argv[1]), sizeof(t_philo));
	//table.philo->timer = malloc(1000);
	//start_stopwatch(table.philo->timer);
	//get_timestamp(table.philo->timer);
	//eat_routine(table.philo);
	if (argc >= 5)
	{
		for (int i = 1; i <= 4; i++)
		{
			if (!check_number(argv[i]))
				return 0;
			table.input_from_av[i-1] = atoi(argv[i]);
		}
	}
	if (argc == 6)
		table.input_from_av[4] = atoi(argv[5]);
	/*
	printf("number_of_philosophers: %d\n", table.input_from_av[0]);
	printf("time_to_die: %d\n", table.input_from_av[1]);
	printf("time_to_eat: %d\n", table.input_from_av[2]);
	printf("time_to_sleep: %d\n", table.input_from_av[3]);
	*/
	if (argc == 6)
		printf("rounds: %d\n", table.input_from_av[4]);
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
}
