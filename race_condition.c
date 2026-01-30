#include <pthread.h>
#include <stdio.h>

int meals;

void	*increment_meals()
{
	for (int i = 0; i < 100000; i++)
		meals++;
	return NULL;
}

int main()
{
	pthread_t t1;
	pthread_t t2;

	pthread_create(&t1, NULL, &increment_meals, NULL);
	pthread_create(&t2, NULL, &increment_meals, NULL);

	pthread_join(t1, NULL);
	pthread_join(t2, NULL);

	printf("meals:%d\n", meals);
	return 0;
}
