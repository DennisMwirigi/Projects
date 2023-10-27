# Task 1
## Overview
The solution for this task completely operates around the state of the event list. If there is nothing in the event list nothing happens. As time goes along the program continually takes elements out of the even list once their arrival tick time has been reached and places them at the back of the ready queue after which operations on the queue are performed until ultimately the ready queue as well as the event list is empty then the program completes.

## Functioning
* The program will display 'IDLE' until the first task's arrival time in the event list has reached as well as in between tasks if the ready queue is empty and the next task's arrival time in the even list has not reached.

* Some operations cost ticks to be performed, therefore the tick increases after the operation. Every time there is a tick increase, a check to see whether the next task's arrival time has been reached and if it has, the task is added to the ready queue immediately before anything else takes place.

* If a task has completed, it is immediately removed from the ready queue and the next task in the queue is dispatched. To implement this I used a flag to indicate when an exit needs to be performed so that I can remove the current task and dispatch the next without rotating the queue. 

### Interesting Case 
In the event that at a certain tick count, a task has used up its quantum (therefore the task at the front will be rotated to the back) and at the same time another task’s arrival time has reached, the adding of the new task to the ready queue will happen first after which the rotating of the queue will take place. This is because in my code **immediately** the tick increases the check specified above will take place thereby adding the task to the queue before the rotation happens.

# Task 2
## Overview
The program computes average waiting and turnaround times of a simulator run and plots the averages per quantum length on two graphs, one for waiting times and one for turnaround times, for comparison. To gather a large pool of data so to improve accuracy, four different quantum lengths, six different dispatch lengths and twenty different seed values are used. That is, for each pair of quantum and dispatch lengths, 20 runs of the simluator are performed each with a different seed value. The same batch of 20 different seed values are used for each pairing. However, this causes the computational time of the program to be rather long. It takes about 5 minutes for the program to run to completion.

## Functioning
* For each pair of quantum and dispatch lengths, a run of the simulator is performed per seed value (4 quantums * 6 dispatches * 20 seeds = 480 runs) and the output piped into a text file called "out.txt"
* Each time an output is generated, the program parses out.txt to collect all the waiting and turnaround times for that specific run.
* The values collected are compiled together per pair of quantum and dispatch lengths then the average of these values is taken and stored separately (4 * 6 = 24 averages).
* Every 6 of the 24 averages correspond to one the quantum lengths. They are plotted together for each quantum as the y-axis values with the x-axis values representing the dispatch lengths.

## Run the program
To run the program type the following command in the terminal window

    $ python3 rrstats.py
