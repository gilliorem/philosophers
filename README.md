# PHILOSOPHERS
*This project has been created as part of the 42 curriculum by regillio*

## DESCRIPTION
This project is an introduction to the use of threads and mutexes.
Thread is a unit of execution within a process.
*What does that mean ?*

A process has [1] to [many] threads
Threads run simultaneously.
One program can execute multiple threads (concurrently), the `main()` being the #thread1.
Unlike sequence programming, thread-programming will not execute the code in sequence (top down).
The instructions run synchronously (well not exactly), it depends on the OS scheduler.
That means that it is out of our control.

For instance, if we create 2 threads in our programm, #thread2 can run before #thread1
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

Because philosophers aren't like women, they can do only one thing at the same time.
And, to prevent death, they will need to do things *synchronously*.
 
While a philosopher is eating, another one might be sleeping or thinking.
Multithreading is making this possible.
The difficulty is that every philosopher needs to eat within `time_to_die` holding two forks.

Now,
at this point in the project, **I had no fucking clue of what I was suppose to do**. Even though I roughly understand how threads can work together,
regarding the *dining philosophers* problem, I didn't know what to do.

So here, I will break down the whole *simulation* layer and explaining it the way I understand it.
A Thread (worker/philosopher/cooker) can be represented as a specific worker. someone that will execute a task in the *production pipeline*
Let's say you are an office worker that needs to use the printer 
if your colleague Karen, who loves to print shit tone of document pages, is printing in the same time as you are trying to print a 100-pages-report, things could go wrong...

Here is the main things than can happen:

### DEADLOCK
- the printer might don't print anything (*DEADLOCK*)
- You lock ink
- Karen locks paper
- You wait for paper
- Karen waits for ink
- → nobody prints, forever

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


### PROGRAM FLOW
1. Parse the arguments
	int input[5];
	`for (int i = 1; i <= 5; i++) {input[i] = atoi(av[i])}`

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
- go in philo/
- run `make`

`./philo <number_of_philosophers> <time_to_die> <time_to_eat> <time_to_sleep> [<number_of_times_each_philosopher_must_eat>]`
`./philo 5 800 200 200 7`


## RESOURCES
### READING AND DOCUMENTATION
- Function Manuals - `gettimeofday - pthread_create - pthread_mutex_lock`
- Stack overflow: Found an interesting discussion expressing different ways to use time related functionnalities in C.
- Operating Systems: Three Easy Pieces (Concurrency part)
- GPT: Discussion on how to tackle the problem in a modular way. Examine and experiment solutions to avoid deadlock.
### VIDEO
- Code vault: Intro on thread and mutex use: *https://www.youtube.com/watch?v=d9s_d28yJq0&list=PLfqABt5AS4FmuQf70psXrsMLEDQXNkLq2*
- Jamshidbek Ergashev: *https://www.youtube.com/watch?v=UGQsvVKwe90*
- Oceano: *https://www.youtube.com/watch?v=zOpzGHwJ3MU&pp=ygUPcGhpbG9zb3BoZXJzIDQy2AaILQ%3D%3D*
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
