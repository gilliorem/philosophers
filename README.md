# PHILOSOPHERS
*This project has been created as part of the 42 curriculum by regillio*

## Description
This project is an introduction to the use of threads and mutexes.
With shared resources
A thread is the smallest unit of execution that the OS can schedule.

Thread (worker/philosopher/cooker) can be seen as a specific task executor. something that will execute the production pipeline.
The main is part of the main thread
One example is "printing"
to print, we need :a printer (which is the fork)
and the workers can print 5 times, but that means that they will have to "queue" because the printer is a shared resource. one at a time. and it is a prequirement 
though, while not printing, our worker (philosophers) can do something else simultanously: (think or sleep) 

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
Code vault: Intro on thread and mutex use: `https://www.youtube.com/watch?v=d9s_d28yJq0&list=PLfqABt5AS4FmuQf70psXrsMLEDQXNkLq2`
Jamshidbek Ergashev
### FELLOW 42 STUDENTS
- chikoh helped me understand the project by visualizing it as in Overcook game.
- wchoo offers me to read his code as reference
- cedric helped managing routine's arguments and pass data around my program
- yucchen helped me testing my program and checking my argc
- ivho gave me great advices on how to earn living time by postponing the simulation

### HONORABLE MENTIONS
- Romain Gilliot helped me draw a parralelism and offer me large explanations on multithreaded program like a web server.
He also helped me cut down layers of abstraction to a small working prototype which introduce a lot of Object Oriented Programming.
Which I think this is a better way to represent the simulation.
