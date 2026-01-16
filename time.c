#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <stdbool.h>
#include <sys/time.h>

// usleep, gettimeofday

// 3 400 100 100 [4]

// we want to track the time that elapsed
// to make it simpler, lets just use seconds.
// =>
// difference between 2 timestamps give us elapased time
// now - start = elapsed.



//volatile struct timeval last_activity;
struct timeval tv;
pthread_mutex_t tv_mtx = PTHREAD_MUTEX_INITIALIZER;
void update_activity(void)
{
	struct timeval now;
	gettimeofday(&now, NULL);
	pthread_mutex_lock(&tv_mtx);
	tv = now;
	pthread_mutex_unlock(&tv_mtx);
}

double elapsed_sec(const struct timeval *start, const struct timeval *end)
{
	return (end->tv_sec - start->tv_sec) + (end->tv_usec - start->tv_usec) / 1e6;
}

void	*watchdog(void *arg)
{
	const double timeout = 10.0;
	for (;;)
	{
		struct timeval now, last;
		gettimeofday(&now, NULL);
		pthread_mutex_lock(&tv_mtx);
		last = tv;
		pthread_mutex_unlock(&tv_mtx);
		if (elapsed_sec(&tv, &now) > timeout)
		{
			fprintf(stderr,"timeout\n");
			_exit(2);
		}
		usleep(20000);
	}
	return NULL;
}

void	do_action()
{
	update_activity();
	while (1)
	{
		int c = getchar();
		if (c == 'x')
			break;
		update_activity();
		usleep(100000);
	}
}

void	time_elapsed()
{
	struct timeval tv;
	gettimeofday(&tv, NULL);
	long start = tv.tv_sec * 1000 + (tv.tv_usec / 1000);
	char c;
	while (1)
	{
		c = getchar();
		if (c == 'x')
			break;
	}
	gettimeofday(&tv, NULL);
	long end = tv.tv_sec * 1000 + (tv.tv_usec / 1000);
	long elapsed_time = end - start;
	printf("millisec elapsed: %ld\n", elapsed_time);
}



int main()
{
	time_elapsed();
	return 0;
	pthread_t wd;
	gettimeofday(&tv, NULL);
	pthread_create(&wd, NULL, watchdog, NULL);
	for (int i = 0; i < 10; i++)
	{
		do_action();
	}

	return 0;
	struct timeval tv;
	int now = 0 ;
	now = gettimeofday(&tv, NULL);
	printf("seconds elapsed since 1970: %ld\n", tv.tv_sec);
	printf("minutes elapsed since 1970: %ld\n", tv.tv_sec / 60);
	printf("hours elapsed since 1970: %ld\n", tv.tv_sec / 60 / 60);
	printf("days elapsed since 1970: %ld\n", tv.tv_sec / 60 / 60 / 24);
	printf("years elapsed since 1970: %ld\n", tv.tv_sec / 60 / 60 / 24 / 365);

	return 0;
}
