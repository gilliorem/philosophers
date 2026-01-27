#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <unistd.h>
#include <pthread.h>
#include <stdbool.h>

typedef struct philo
{
	bool has_eaten;
	bool is_dead;
}t_philo;

void	set_shared_bool(bool *bool_address, bool value)
{
	*bool_address = value;
}

int	main()
{
	t_philo *philo = calloc(1, sizeof(t_philo));
	//philo->has_eaten = false;
	//philo->is_dead = false;
	set_shared_bool(&philo->has_eaten, true);
	set_shared_bool(&philo->is_dead, true);
	if (philo->has_eaten == true)
		printf("has eaten\n");
	else
		printf("hasn't eat\n");
	if (philo->is_dead == true)
		printf("died\n");
	else
		printf("is alive\n");
	return 0;
	
}
