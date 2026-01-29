# PHILOSOPHERS
*This project has been created as part of the 42 curriculum by regillio*

## DESCRIPTION
This project is an introduction to the use of threads and mutexes.
Thread is a unit of execution within a process.
*wtf does that mean ?*

It simply means that a process have one to many threads.
Threads run simultaneously or we say **concurrently**.
One program can execute multiple threads (concurrently), the `main()` being the #thread1
Unlike sequence programming, thread-programming will not execute the code in sequence (one line after the other).
The instructions run simultaneously (at the same time) (well not exactly), it actually depends on the OS scheduler.
That means that it is out of your control.

For instance, if you create 2 threads in your programm, at start, #thread2 can run before #thread1
and the next time you run your program the OS scheduler might decide to run #thread1 first...
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

*"Why the fuck do we even need threads to do this project ?"*

Because philosophers are not like women, they can only do one thing at the same time:
and to prevent death, they will need to do things *synchronously*.
 
While a philosopher is eating for example, another one might be sleeping or thinking.
Multithreading is making this possible.
The difficulty is that every philosopher needs to eat within `time_to_die` holding two forks.

Now,
When I first started, at this point in the project, **I had no fucking clue of what I was suppose to do**. Even though I roughly understand how threads can work together,
regarding the *dining philosophers* problem, I didn't know what to do.

So here, I will break down the whole *simulation* layer and explaining it the way I understand it.
Thread (worker/philosopher/cooker) can be seen as a specific task executor. something that will execute a task in a *production pipeline*
For example is "printing"
to print, we need :a printer (which is the Thread)
if we have 5 printers we can print 5 different document in the same time
Now in printer there is ink.
Let's say that the 5 printers share the same ink
In order to have the correct color-printed-document, each printer must pick the correct amount of ink as they print.
so they will need to "lock ink black and blue for example", take the right amount and print and "unlock".
if we dont lock, we might endup having a document with 5 colors instead of one
though, while not printing, it can do something else: cleaning...

back to the printer/writer.
the sharable resource can be a printer and the local sharable resource (yes, another one, can be the pen.).
so there are two types of shareable resource. local and general.
since it is sharable, we have to apply a mutex to this resource. yes, we dont want two philos writing on the same time: then we will have some non sens being printed.
like instead of printing "hello world", "hworldllo" could be printed. 

## Instructions
./a.out [nphilos] [timetodie] [timetoeat] [timetothink]
./a.out 5 700 200 200 200


1. Parse the arguments
*for simplicity sake and because I do what I want, I am going to turn every argument to an int* 
	int input[5];
	`for (int i = 1; i <= 5; i++) {input[i] = atoi(av[i])}`
	
2. Timeconversion ?
	Do we need to turn the timevalues into times, or we can just use them as int ?
	lets pretend we can use them as int for now.

3. Routine
	 | 		|
	odd		even

	take fork in spec order (to avoid  deadlock)
	eat				| other philos are thinking|sleeping
	put back fork			

3.1 Time module
start; routine; usleep(timetoroutine); end 
	
3.2 Monitor (the reaper/watchdog/monitor/server)
Someone dead ?
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
