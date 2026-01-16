#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <stdbool.h>
#include "libft/libft.h"

// write a prog that express concurrency.

void	*hello(void *arg)
{
	printf("%s\n", (char *) arg);
	return NULL;
}

int main()
{
	pthread_t t1, t2;
	
	printf("start\n");
	pthread_create(&t1, NULL, hello, "A");
	pthread_create(&t2, NULL, hello, "B");

	pthread_join(t1, NULL);
	pthread_join(t2, NULL);
	printf("end\n");
	return 0;
}
