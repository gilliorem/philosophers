#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

// 2 [t_to_die] [t_to_eat] [t_to_sleep] ([number_of_rounds])

// lets say that we dont have time
// that we dont think
// philo either sleep or eat.

void	*philo_routine(void *arg)
{
	printf("philo is eating\n");
	printf("philo is sleeping\n");

	return NULL;
}

int main(int argc, char *argv[])
{
	if (argc != 2)
		{printf("need 2 args\n");return 0;}
	int number_of_philo = atoi(argv[1]);

	pthread_t philo_t[number_of_philo];
	while (1)
	{
		for (int i = 0; i < 2; i++)
		{
			pthread_create(philo_t + i, NULL, &philo_routine, NULL);
			usleep(100000);
		}
	}

	return 0;
	
}
