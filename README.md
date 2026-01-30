# PHILOSOPHERS
*This project has been created as part of the 42 curriculum by regillio*

## DESCRIPTION
This project is an introduction to the use of threads and mutexes.
Thread is a unit of execution within a process.
*What does that mean ?*

A process has [1] to [many] threads.
Unlike a multi-sequence-program, a multi-threads-program will not execute the code in sequence (top down).
One program can execute multiple threads concurrently.
The main thread is the one running the program.

In a multi-threading program,
the instructions run synchronously (well not exactly), it depends on the OS scheduler.
That means that it is out of our control.

For instance, if we create 2 threads in our program, #thread2 can run before #thread1
and, the next the program run, the OS scheduler might decide to run #thread1 first...
"I thought that programming was deterministic."
You were wrong.

![visual representation](https://www.iitk.ac.in/esc101/05Aug/tutorial/figures/essential/threads-two.gif)

**Here are the key things you need to know to succeed in this assignment:**
- One or more philosophers sit at a round table.
There is a large bowl of spaghetti in the middle of the table.

- The philosophers take turns eating, thinking, and sleeping.
While they are eating, they are not thinking nor sleeping;
while thinking, they are not eating nor sleeping;
and, of course, while sleeping, they are not eating nor thinking

- Since eating spaghetti with just one fork is impractical, a philosopher must pick up
both the fork to their right and the fork to their left before eating.

- When a philosopher has finished eating, they put their forks back on the table and
start sleeping. Once awake, they start thinking again. The simulation stops when
a philosopher dies of starvation.

- Every philosopher needs to eat and should never starve.
- Philosophers do not communicate with each other.
- Philosophers do not know if another philosopher is about to die.
- Needless to say, philosophers should avoid dying!

*"Why the f do we even need threads to do this project ?"*

Because a philosopher isnt't a woman, he can only only *one thing at the same time*
And, to prevent death, the philosophers will need to do things *synchronously*.
 
While a philosopher is eating, another one might be sleeping or thinking.
Multithreading is making this possible.
The difficulty is that every philosopher needs to eat within `time_to_die`, and holding two forks.

![visual representation](https://media.tenor.com/f9aThtyRQS4AAAAM/jackie-chan-meme.gif)

Now,
at this point in the project, **I had no fucking clue of what I was suppose to do**.
Even though I roughly understand how threads can work together,
regarding the *dining philosophers* problem, I didn't know what to do.

So here, I will break down the whole *simulation* layer and explain it the way I understand it.

Let's say that a thread is a Writer, and there is one marker.

You hold the marker, you write.

You release the marker.

While a Writer writes, another can think about what he's gonna write next and vice-versa.

If someone else wants to write, he needs to holds the marker, write, release.

If there is only 1 pen and 2 writers, each writer will need to wait for the pen to be released.

Though, they can think with their own mind (no need to share any resource here).

And...

That is all.

The main idea is here. It is just that instead of being Writer, we are dealing with philosophers.
The whole difficulty of this project is actually to cut down the *abstract simulation* into a C-working program.

So let's implement a simpler simulation with basics elements so there is a program runing.
And from there, we can add complexity.

But first, let's define the two problems that show up when we run a multithreaded-program.

Remember ?

Threads run concurrently: they use share the same memory space.
So that might leed to some issue manipulating a variable that is being modify in two different threads.
Let's see an example.

```C

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
```

### DEADLOCK
- You hold the pen
- Someone hold the eraser
- You wait for the eraser
- The other guy waits for the pen
- → nobody writes, forever *waiting for a shared resource to be released*

Note that in this example one shared resource is the pen and another one is the eraser.
It can be the same that resource that is being shared.

In philosophers, the shared resource is the fork: a philosopher needs 2 forks to eat.
Picture this: 2 philos sit at a *round* table. there are 2 forks. (yes they share their forks... I guess they didnt care about covid backthen)
If they both pick their *left* fork first in the same time, they will never be able to pick up the second fork.
Since it is a round table, they are facing each other: one's left fork is the right fork of the other.
It is not to important at the begining of the project to worry about deadlock, eventually, as the program gets more complex, deadlock will happen and it will become easier to visualize what's happening.

### RACE CONDTION
Before printing your report, you want somebody to go through, to make sure everything is correct.
your report is long so you ask a second person to go through.
You endup having pages that have been review twice because your workers didn't coordinate properly together. 
and other not being review at all.
and at some point, of them put a none review page on top of the review pile because he thought you had review it! 
they move the report page to the "review pile" but since they were many pages some got into this pile without being review.
- Both workers read it at the same time (say it’s 10)
- Both decide to review page 10
- Both increment it to 11
- Result: page 11 gets skipped, page 10 reviewed twice
- Last page will never get review

Thread-local resources (such as a worker’s own data or tools) do not require synchronization, but any resource accessed by multiple threads must be protected to avoid race conditions or deadlocks.
If this explanation isn't clear enough, check Code Vault videos and Oceano's.

### STRUCTURE
Let's represent the elements we have in the simulation and try to run our first multithreaded program.
Since the whole program is a *simulation*, I find it good to use *Object Oriented* approach to represent the different elements of our program.
For instance, there is no such type as *philo* or *fork*... That's why it can be tricky and hard to visualize what is going on at a *C* level.
Representing those elements using structs is helpfull to better understand what is happening.

So we have
- a Table
- Philosophers
- a Timer

*Let's not worry so much about the elements, for now, the idea is just to have a program runing for now !*

```C
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
```

### PROGRAM FLOW
1. Parse the argument
	int input[5];
	`for (int i = 1; i <= 5; i++) {input[i] = atoi(av[i])}`
We are going to compare these values against time, which is why we turn this into numeric values.
2. Routines
	 | 		|
	odd		even

	take fork in spec order (to avoid  deadlock)
	eat				| other philos are thinking|sleeping
	put back fork			

4. Time module
	`now_ms = (now.tv_sec * 1000) + (now.tv_usec / 1000);`
5. Monitor 
Looking up for ending the simulation when needed (in two cases):
Constantly checking if someone is dead and has everyone eaten yet?

## Instructions
```bash
cd philo/
make
./philo <number_of_philosophers> <time_to_die> <time_to_eat> <time_to_sleep> [<number_of_times_each_philosopher_must_eat>]
```
Example:
```bash
./philo 5 800 200 200 7
```


## RESOURCES
### READING AND DOCUMENTATION
- Function Manuals - `gettimeofday - pthread_create - pthread_mutex_lock`
- Stack overflow: Found an interesting discussion expressing different ways to use time related functionnalities in C.
- Operating Systems: Three Easy Pieces (Concurrency part)
- GPT: Discussion on how to tackle the problem in a modular way. Examine and experiment solutions to avoid deadlock.
### VIDEO
- Code vault: Intro on thread and mutex use: *https://www.youtube.com/watch?v=d9s_d28yJq0&list=PLfqABt5AS4FmuQf70psXrsMLEDQXNkLq2*
- Jamshidbek Ergashev: *https://www.youtube.com/watch?v=UGQsvVKwe90*
- Chris Kanich, Deadlock in 3 minutes: *https://www.youtube.com/watch?v=oEbXlSH8hyE&pp=ygUfdGhyZWFkIGRlYWRsb2NrIGFuaW1hdGlvbiB0cmFpbtgGB9IHCQl8CgGHKiGM7w%3D%3D*
- Oceano: - *https://www.youtube.com/watch?v=zOpzGHwJ3MU&pp=ygUPcGhpbG9zb3BoZXJzIDQy2AaILQ%3D%3D*
	- Threads and Race condtions: *https://www.youtube.com/watch?v=mvZKu0DfFLQ*
	- Deadlock:* https://www.youtube.com/watch?v=rFYYifVj5gs*
### FELLOW 42 STUDENTS
- chikoh helped me understand the project by visualizing it as in Overcook game.
- wchoo offers me to read his code as reference
- cedric helped managing routine's arguments and pass data around my program
- yucchen helped me testing my program and checking my argc
- ivho gave me great advices on how to earn living time by postponing the simulation

### TEST AND COMPARE
- mcombeau: *https://github.com/mcombeau/philosophers*

### HONORABLE MENTIONS
- Romain Gilliot helped me draw a parralelism and offer me large explanations on multithreaded program like a web server.
He also helped me cut down layers of abstraction to a small working prototype which introduce a lot of Object Oriented Programming.
Which I think this is a better way to represent the simulation.
