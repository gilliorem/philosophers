#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <unistd.h>
#include <pthread.h>
#include <stdbool.h>
#include <string.h>

typedef struct s_settings t_settings;
typedef struct s_table t_table;
typedef struct s_timer t_timer;
typedef struct s_philo t_philo;

typedef struct s_settings
{
	bool	six_argc;
	int	rounds; 
	int	time_to_die;
	int	time_to_eat;
	int	time_to_sleep;
	int meals;
	bool	end;
	pthread_mutex_t m_end;
	pthread_mutex_t m_log;
	pthread_mutex_t	m_meals;
	pthread_t monitor;
	t_table *table;
	t_timer *timer;
	t_philo *philo;
} t_settings;

typedef struct s_timer
{
	struct timeval start;
	long start_ms;
} t_timer;

typedef struct s_table
{
	int number_of_philos;
	t_philo *philo;
	pthread_mutex_t *forks;
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
	int t_last_meal;
	int last_meal;
	pthread_mutex_t m_last_meal;
	pthread_mutex_t	m_log;
	bool	has_eaten;
	pthread_mutex_t	m_has_eaten;
	bool dead;
	int rounds;
} t_philo; 


int	    check_argc(int argc);
int    check_number(char *av);
int    parse_argv(int argc, char *argv[], t_settings *settings);

t_settings *init_settings(void);
t_timer	*init_time(void);
t_table	*init_table(void);
void	init_mutexes(t_settings *settings, pthread_mutex_t *mutex);
void	init_philo(t_philo *philos, t_settings *settings);

void    ft_memcpy(void *dest, void *src, size_t n);
int now_ms(t_timer *timer);
void    msleep(int n);
void	set_share_resource(pthread_mutex_t *mutex, bool *resource_status, bool status);
void	set_resource(pthread_mutex_t *mutex, void *resource, void *status, size_t res_size);

void	m_print(t_timer *timer, t_philo *philo, char *msg);
void	m_died(t_timer *timer, t_philo *philo, char *msg);
void	m_forklog(t_timer *timer, t_philo *philo, char *msg);
void	m_end(t_settings *settings);

void	destroy_mutexes(t_settings *settings, t_philo *philos);
void	clean(t_settings *settings);

int	check_last_meal(t_settings *settings, t_philo *philo);
int	everyone_ate(t_settings *settings, int *round);
int	check_end(t_settings *settings);
void	*monitor_routine(void *arg);

void	takes_fork(t_philo *philo);
void	drop_forks(t_philo *philo);
void	eats(t_philo *philo);
int	lonely_philo(t_settings *settings, t_philo *philo);
void	*philo_routine(void *arg);

void	start_simulation(t_settings *settings, t_philo *philos);
void	*philo_routine(void *arg);
void	*monitor_routine(void *arg);