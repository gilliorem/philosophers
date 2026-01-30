#include <pthread.h> //pthread
#include <stdio.h> //printf
#include <unistd.h> //usleep
#include <stdlib.h> //calloc

/* this are just pre-register instructions because I use
 * philos struct in table struct before defining it, 
 * see it as prototypes so compiler knows I will define 
 * those data type and remove the warnings */

typedef struct s_table t_table;
typedef struct s_philo t_philo;
typedef struct s_timer t_timer;

typedef struct s_table
{
	int	number_of_philos;
	t_philo *philos;	
} t_table;

typedef struct s_philo
{
	int id;
	int time_to_eat;
	int time_to_sleep;
	pthread_t t;
} t_philo;

typedef struct s_timer
{
	int now;
	int elapsed_time;
} t_timer;

void *philo_routine(void *arg)
{
	t_philo *philo = (t_philo*) arg;
	
	for(int i = 0; i < 100000; i++)
	{
		printf("philo %d eats\n", philo->id);
		usleep(philo->time_to_eat * 1000);
		printf("philo %d sleeps\n", philo->id);
		usleep(philo->time_to_sleep * 1000);
	}
	return NULL;
}

int main()
{
	t_table *table = calloc(1, sizeof(t_table));
	table->number_of_philos = 2;
	table->philos = calloc(table->number_of_philos, sizeof(t_philo)); 
	for (int i = 0; i < 2; i++)
	{
		table->philos[i].id = i + 1;
		table->philos[i].time_to_eat = 200;
		table->philos[i].time_to_sleep = 500;
	}
	for (int i = 0; i < 2; i++)
		pthread_create(&table->philos[i].t, NULL, &philo_routine, &table->philos[i]);
	usleep(10000);
	for (int i = 0; i < 2; i++)
		pthread_join(table->philos[i].t, NULL);

	return 0;
}
