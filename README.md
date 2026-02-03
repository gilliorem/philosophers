# PHILOSOPHERS
*This project has been created as part of the 42 curriculum by regillio*

## DESCRIPTION
This project is an introduction to the use of threads and mutexes.
A thread is a unit of execution within a process.

*What does that mean ?*

A process has [1] to [many] threads.
Unlike a sequenced-program, a multi-threaded-program will not execute the code in sequence (top down).
The program instructions will be run in parrallel.
The threads are running concurrently. *just a fancy word to say in parrallel*

*The main thread is the the one running the program.*

In a multi-threaded program,
the instructions run synchronously (well not exactly), it depends on the OS scheduler.

That means that it is out of our control.

For instance, if we create 2 threads in our program, #thread2 can run before #thread1
and, the next time the program runs, the OS scheduler might decide to run #thread1 first...
"I thought that programming was deterministic."
You were wrong.

![visual representation](https://www.iitk.ac.in/esc101/05Aug/tutorial/figures/essential/threads-two.gif)

**Here are the key things we need to know to succeed in this assignment:**
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

Because a philosopher isn't a woman, he can only do *one thing at the same time*.

And, to prevent death, philosophers will need to do things *concurrently*.
 
While a philosopher is eating, another one might be sleeping or thinking.
Multithreading is making this possible.
The difficulty is that every philosopher needs to eat within `time_to_die`, while holding two forks.

![visual representation](https://media.tenor.com/f9aThtyRQS4AAAAM/jackie-chan-meme.gif)

Now,
at this point in the project, **I had no fucking clue of what I was suppose to do**.
Even though I roughly understand how threads can work together,
regarding the *dining philosophers* problem, I didn't know what to do.

So here, I will break down the whole *simulation* layer and explain it the way I understand it.

Let's say that a thread is a Writer, and there is one marker (to share).

You hold the marker, you write.

You release the marker.

While a Writer *writes*, another can *think* about what he's gonna write next and vice-versa.

If someone else wants to write, he needs to holds the marker, write, release.

If there is only 1 marker and 2 writers, each writer will need to wait for the marker to be released.

Though, they can think with their own mind (no need to share any resource here),
they cannot write without the marker.

Suppose we have two Writers,
so the *routine* can be:
- Hold the marker, write
- Release the marker, think

Since writers are *threads running in parrallel*, one is gonna wait for the first round of writing and then, they will run together, simultaneously, until we stop the simulation.

And...

That is all.

The main idea is here. It is just that instead of Writers, we are dealing with philosophers.

The whole difficulty of this project is actually to cut down the *abstract simulation* into a C-working program.

So let's implement a simpler simulation with basic elements so there is a program runing.

And from there, we can add complexity.

But first, let's define two problems that will show up when we are dealing with multithreaded-program.

*Remember ?*

Threads run concurrently: they share the same memory space.
So that might lead to some issue manipulating a variable that is being modified in different threads.
Let's C an example. (LOL)
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
### RACE CONDITION
Threads do not communicate to each other, so every time a thread perfoms *more than a one-step-action* on the shared-variable, it might read it wrong.

Incrementing an integer is not an atomic operation. Even though meals++ looks like a single instruction in C, it actually expands into multiple machine-level steps.

Conceptually, it works like this:

- Read the current value from memory
- Modify the value (increment)
- Write the new value back to memory

This is similar to how memcpy works internally: data is first read into a temporary register before being written elsewhere.

```s

	movl	meals(%rip), %eax ; read value from memory into register
	addl	$1, %eax	  ; increment register
	movl	%eax, meals(%rip) ; write value back to memory
```

Assume meals == 20 in memory.

- Thread #1 reads meals → gets 20
- Thread #2 reads meals → also gets 20
- Thread #1 increments and writes → 21
- Thread #2 increments and writes → 21

Even though two increments happened, the final value is 21 instead of 22.

This is a lost update, caused by both threads reading the same initial value before either write occurred.

The final result depends on timing, not logic — which is the defining property of a race condition.

In a race condition: the order of execution is not guaranteed.

We use mutex to avoid it.
```C
pthread_mutex mutex_meals;

	pthread_mutex_lock(&mutex_meals);	
	meals++;
	pthread_mutex_unlock(&mutex_meals);	
```
General rule to apply:
- If the same variable is being modified by more than one thread, then that variable needs to be lock-unlock every time we interact with.

Even in an if statement:

```C
pthread_mutex_lock(&mutex_meals);	
if (meals > 100)
{
	printf("I am full\n");
	pthread_mutex_unlock(&mutex_meals);	
	return ;
}
pthread_mutex_unlock(&mutex_meals);	
```

### DEADLOCK
- You hold the marker
- Someone hold the eraser
- You wait for the eraser
- An other Writer waits for the marker
- → nobody writes, forever *waiting for a shared resource to be released*

In philosophers, the shared resource is the fork: a philosopher needs 2 forks to eat.

Picture this:

2 philosophers sit at a *round* table. there are 2 forks AND they need both of the 2 forks to be able to eat (because they are eating spaghetti, *I would personnaly have use a knife to... you know... hm whatever*). (so yes they share the same forks... they didn't care backthen).

If they both pick their *left* fork first at the same time, they will never be able to pick up the second *right* fork.

They end up waiting forever.

Since it is a round table, they are facing each other: one's left fork is the right fork of the other.

It is not to important to worry about deadlock now, as the program gets more complex, deadlock will happen and it will become easier to visualize what's happening.

### STRUCTURE
Let's represent the elements we have in the simulation and try to run our first multithreaded program.

Since the whole program is a *simulation*, I find it good to use an *Object Oriented* approach to represent the different elements.

For instance, there is no such data-type as *philo* or *fork*... That's why it can be tricky and hard to visualize what is going on at a *C* level.

Representing those elements using structs is helpfull to better understand what is happening.

So we have
- a Table
- Philosophers
- a Timer

*Let's not worry so much about all the elements, the idea is just to have a program runing for now !*

```C
#include <pthread.h> //pthread
#include <stdio.h> //printf
#include <unistd.h> //usleep
#include <stdlib.h> //calloc

typedef struct s_table t_table;
typedef struct s_philo t_philo;
typedef struct s_timer t_timer;

/* these are just pre-register instructions because I use
 * philos struct in table struct before defining it, 
 * see it as prototypes so compiler knows I will define 
 * those data type and remove the warnings */

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

Now that we have a multi-threaded-program running, it's time to check if those philos deserve to live
and if not, KILL THEM.

Finally, the fun part begins.

In order to do so we are going to use a Monitor, that will just keep track of time.

So we need get the starting time of simulation. *It is like starting a stopwatch*.

And from this point onwards, every x milliseconds we are going to get the time since we start *to eat* and check
if someone should be dead by now, and if it is the case, murder him and stop the simulation.

(same idea if we use the last argument `number_each_philosophers_must_eat`) but this time, we are not only checking if someone is dying, we also keep track of their meals.

#### IS MONITOR A THREAD ?
...
...

Of course.
Because this guy will run in parrallel to our philo threads, 
if not, how is he gonna know that someone died ?

Since he is constantly checking if the philos's `time_since_last_meal` isn't exceding his `time_to_die`.

*It will become obvious once you start the implementation.*

#### PREVENT DEADLOCK
Back to the picking each other's left fork problem (that leads to a deadlock).

In order to prevent this deadlock we need to break one of these two deadlock conditions:
- Mutual exclusion: only one process may use a resource at a time. *why not this one ?*
- Circular wait: #Thread1 waiting for #Thread2 to give up a shared-resource. #Thread3 is waiting for #Thread2 and so on.

To prevent this we need to use *Asymetry*

The simpliest form would be that *some* of the philosophers wait before taking the forks,
or (better), depending on where [this] philo sits around the table, he will have to pick the same
fork first...

### THAT'S IT ?
And yes, there you have it, that's the whole simulation...

### PROGRAM FLOW
1. Initialization and parsing
2. Create and "launch the Threads"
3. End the simulation (if needed) 
4. End the threads properly, destroy the mutexes
5. Free the allocated memory

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
- GPT: Discussions on:
-	How to tackle the problem in a modular way. 
-	Memory-wise: the proper way to initialize each datatype of the program.
-	Thread argument: how to properly uses `void *arg`		

### VIDEO
- Code vault: Intro on thread and mutex use: *https://www.youtube.com/watch?v=d9s_d28yJq0&list=PLfqABt5AS4FmuQf70psXrsMLEDQXNkLq2*
- Jamshidbek Ergashev: *https://www.youtube.com/watch?v=UGQsvVKwe90*
- Chris Kanich, Deadlock in 3 minutes: *https://www.youtube.com/watch?v=oEbXlSH8hyE&pp=ygUfdGhyZWFkIGRlYWRsb2NrIGFuaW1hdGlvbiB0cmFpbtgGB9IHCQl8CgGHKiGM7w%3D%3D*
- Oceano: - *https://www.youtube.com/watch?v=zOpzGHwJ3MU&pp=ygUPcGhpbG9zb3BoZXJzIDQy2AaILQ%3D%3D*
	- Threads and Race condtions: *https://www.youtube.com/watch?v=mvZKu0DfFLQ*
	- Deadlock:*https://www.youtube.com/watch?v=rFYYifVj5gs*
### FELLOW 42 STUDENTS
- chikoh helped me understand the project by visualizing it as in Overcook game.
- wchoo offers me to read his code as reference
- cedric helped managing routine's arguments and pass data around my program
- yucchen helped me testing my program and checking my argc and helped me check the end in printing function to avoid extra logs.
- ivho gave me great advices on how to earn living time by postponing the simulation

### TEST AND COMPARE
- mcombeau: *https://github.com/mcombeau/philosophers*

### HONORABLE MENTION
- Romain Gilliot helped me draw a parralelism with real-world working program runining using multiple threads, he offered me detailed explanations on (his) multithreaded web-server.
He also helped me cut down layers of abstraction to a small working prototype which introduce a lot of Object Oriented Programming.
Which I think this is a better way to represent the simulation.
- *https://github.com/Rrominet*
