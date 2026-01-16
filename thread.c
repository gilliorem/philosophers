#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <stdbool.h>

// the routine is eat, sleep, think.
// is eat a routine itself ?
//    sleep
//    think 
// it is multithreaded: more than one philo.

// questions:
// 	the printf statement:
// 	a printf is an independant thread ? with its associated mutexes ?
//	the init of the print thread needs to be in the main () or in a specific init function that will init
//	all the mutexes ? 	
// 	the shared ressources: (forks)
// 	once philo needs to take 2 forks: does that mean that 2 mutex need to be lock and then unlock (while eating) ? YES.
// 	by the way, as we have an array of pthread that represent the philosophers, we have an array of mutexes that represent the forks.
// 	print_routine: I want to print the correct action based on what the philos needs to do.
// 	I need a certain kind of tracker that check: have you already eaten in this round ? yes: then sleep; no then eat.
// 	I also need a tracker that check for philo that dies (every 10 ms)

// I need some kind of struct for the forks.
//
/* 4) Ton idée “tracker has_eaten / rounds”

Tu mélanges deux concepts :

A) Le cycle logique
Un philo fait : think → take forks → eat → release → sleep → repeat
Tu n’as pas besoin d’un bool has_eaten “par round” si ton loop structure le garantit déjà => du fait que ce soit un cycle, on peut juste compter le nombre de repas.

B) L’état observé / debug
Tu peux stocker un state (THINKING/EATING/SLEEPING) juste pour afficher et vérifier.
Mais attention : si plusieurs threads lisent/écrivent cet état, ça devient shared state ⇒ mutex ou approche “owner-only” (seul le thread du philo modifie son propre état, les autres ne font que lire avec protection): implique un autre thread pour check l'etat.
Donc oui, un “tracker” peut exister, mais il ne doit pas créer un nouveau bordel de races.
Tu as un tableau de t_philo philos[n]
Tu as un tableau de pthread_mutex_t forks[n]
Chaque philo[i] pointe sur forks[i] et forks[(i+1)%n]

7) Ton init_philo() : c’est une fête du slip niveau alloc
calloc(0, sizeof(&philo));
Ça alloue… 0 octet. Et sizeof(&philo) c’est la taille d’un pointeur, pas de la struct. Donc même l’intention est à côté.
philo->ids[philo->n];
Ça ne déclare rien. Ça ne réserve rien. Ça fait rien.

for (int i = 1; i <= philo->n; i++)
Tu démarres à 1, tu vas jusqu’à n inclus : tu sors des limites d’un tableau C classique (0..n-1). Même problème dans multithread d’ailleurs.

Bref : ici il te manque la base : allocation + indexation.

8) rounds : data race direct
Tu fais rounds++ dans chaque thread sans mutex.
C’est une course. Même si tu locks autour de certains prints, rounds++ lui-même est pas protégé.

Et en plus : “rounds” global, ça veut dire quoi ? Le nombre de rounds total tous philos confondus ? ou le numéro de round global ? Ce n’est pas clair.
Souvent :
chaque philo a son compteur de meals / rounds
et un monitor global check la condition d’arrêt
I love the idea of a *grim reaper* which his litteraly the dead persona, that check everyone state and kill if need to kill.
 *
 * we have a thread as the whole program
 * we other thread for each philo
 * we have the forks that can be represented as whatever, int for example AND
 * we have forks represented as mutexes. an array of mutexes. (that part I dont understand)
*/

// I do need a table structure that represent the whole with the philosophers and other global objects like monitoring
// timing and more essential like total number of philos, total number of forks.

// I need to set and get different values like number fo meals total meals
// I need to get the elapsed time relative to the current time
// I need a precise usleep
// eating, sleeping, thinking are actual routine ?....
//
// I can do the parsing. and the translation into long (correct unit of time: miliseconds.)

typedef struct s_input
{
	int n_philos;
	long time_to_eat;
	long time_to_sleep;
	long time_to_think;
	int rounds;
} t_input;

typedef struct s_table
{
	t_input *input;
	int n_philos;
	int n_forks;
	pthread_mutex_t forks[];
} t_table;

typedef struct s_philo
{
	int	n;
	int	id;
	pthread_t p_thread;
	pthread_mutex_t *left_fork; // should I do an array of two mutexes instead ?
	pthread_mutex_t *right_fork;
	bool has_eaten; // useless, replace with a state var ?...
	int	ids[];
} t_philo;

int rounds = 0; 
pthread_mutex_t lock;
pthread_mutex_t lock_print;

void	init_mutex(pthread_mutex_t *mutex, t_philo *philo)
{
	pthread_mutex_t forks[philo->n];
	for (int i = 0; i < philo->n; i++)
	{
		pthread_mutex_init(forks + i, NULL);
	}
}

// might have to use malloc to initialize the n_philos array instead ?
void	init_philo(t_philo *philo)
{
	calloc(0, sizeof(t_philo));
	int n_philos = 2;
	philo->n = n_philos;
//	philo->ids[philo->n];
	for (int i = 1; i <= philo->n; i++)
	{
		philo->ids[i] = i;
	}
	philo->has_eaten = false;
	pthread_mutex_init(philo->left_fork, NULL);
	pthread_mutex_init(philo->right_fork, NULL);
}

// has to take forks in a specific order
// the forks has to be identifiable
// if odd number of philos:
// 	philo[n] tries to take fork[1] and fork[N]

void	*routine(void *arg)
{
	int id = *(int *)arg;
	rounds++;
	for (int i = 0; i < 2; i++)
	{
		pthread_mutex_lock(&lock); // I want to lock the printing messages for the rounds. do I need a specific pthread_mutex_t var ? like pthread_mutex_t lock_print 
		printf("Round %d\n", rounds);
		pthread_mutex_unlock(&lock);
		pthread_mutex_lock(&lock);
		printf("philo[%d] is eating\n", id);
		usleep(30000);
		pthread_mutex_unlock(&lock);
		printf("philo[%d] is sleeping\n", id);
		usleep(10000);
		printf("philo[%d] is thinking\n", id);
		usleep(10000);
	}
	return NULL;
}

void	*print_routine(void *arg)
{
//	char *message = (char *)arg;
	int id = *(int *)arg;
//	bool has_eaten = false;
	//t_philo *philo = (t_philo *)arg;
	pthread_mutex_lock(&lock_print);
//	printf("philo[%d] is %s.\n", id, action);
	printf("philo[%d] is eating.\n", id);	
	pthread_mutex_unlock(&lock_print);
	return NULL;
}

void	multithread(int* ids)
{
	pthread_t philo[2];
	for (int i = 0; i < 2; i++)
	{
		pthread_create(philo + i, NULL, &routine, &ids[i]);
	}
	for (int i = 0; i < 2; i++)
	{
		pthread_join(philo[i], NULL);
	}
	
}

// parsing

// 3 300 100 100 [3]

// to represent the parameter of the simulation we can have a struct: initial_data
// parsing:
// 	5 or 6 argc. (else it's wrong).
// 	argv[1] is an int
// 	argv[2-4] are long (atol)
// 	argv[5] is an int.

long	convert_input(char *argv)
{
	long	res;
	
	res = atol(argv);	
	return res;
}

t_input *init_input(char *argv[])
{
	t_input *input;
	input = calloc(1, sizeof(t_input));
	input->n_philos = atoi(argv[1]);
	input->time_to_eat = convert_input(argv[2]);
	input->time_to_sleep = convert_input(argv[3]);
	input->time_to_think = convert_input(argv[4]);
	input->rounds = atoi(argv[5]);
	return input;
}

int main(int argc, char *argv[])
{
	if (argc == 6 || argc == 5)
	{
		t_input *input;
		input = init_input(argv);
		printf("Philos:[%d], time to eat:%ld ms, time to sleep:%ld ms, time to think:%ld ms, rounds:%d\n", input->n_philos, input->time_to_eat, input->time_to_sleep, input->time_to_think, input->rounds);

		

		return 0;
	}
	else {printf("errror. argc\n");return 0;};
	
	pthread_mutex_init(&lock, NULL);

	int ids[2]={1,2};
	multithread(ids);

	pthread_mutex_destroy(&lock);
	printf("end of round %d\n", rounds);
}
