import sys
import os
import random
from statistics import mean
from matplotlib import pyplot as plt
import numpy as np

def plot_values(wait_times, ta_times):
    xvals = [0, 5, 10, 15, 20, 25]
    yvals = []
    count = 0

    #plot average waiting times
    plt.figure(figsize=(8,4.8))
    for i in range(len(wait_times)):
        yvals.append(wait_times[i])

        #every 6 averages correspond to one quantum length. Plot per quantum length
        if (i+1)%6 == 0:
            count+=1
            if count == 1:
                plt.plot(xvals, yvals, color='red', marker='x', label='q=50')
            elif count == 2:
                plt.plot(xvals, yvals, color='grey', marker='+', label='q=100')
            elif count == 3:
                plt.plot(xvals, yvals, color='green', marker='.', label='q=250')
            elif count == 4:
                plt.plot(xvals, yvals, color='blue', marker='s', label='q=500')
                count = 0
            
            yvals.clear()

    plt.legend(loc='center', bbox_to_anchor = (1.2,0.5))
    plt.title("Round Robin Scheduler -- # tasks: 1000; 20 seed values")
    plt.xlabel("Dispatch overhead")
    plt.ylabel("Average waiting time")
    plt.tight_layout()
    plt.savefig('graph_waiting.pdf')
    plt.close()

    #plot average turnaround times
    plt.figure(figsize=(8, 4.8))
    for i in range(len(ta_times)):
        yvals.append(ta_times[i])

        #every 6 averages correspond to one quantum length. Plot per quantum length
        if (i+1)%6 == 0:
            count+=1
            if count == 1:
                plt.plot(xvals, yvals, color='blue', marker='x', label='q=50')
            elif count == 2:
                plt.plot(xvals, yvals, color='grey', marker='+', label='q=100')
            elif count == 3:
                plt.plot(xvals, yvals, color='red', marker='.', label='q=250')
            elif count == 4:
                plt.plot(xvals, yvals, color='green', marker='s', label='q=500')
                count = 0
            
            yvals.clear()

    plt.legend(loc='center', bbox_to_anchor = (1.2,0.5))
    plt.title("Round Robin Scheduler -- # tasks: 1000; 20 seed values")
    plt.xlabel("Dispatch overhead")
    plt.ylabel("Average turnaround time")
    plt.tight_layout()
    plt.savefig('graph_turnaround.pdf')
    plt.close()

    return


def get_data(temp_wait, temp_ta, qlen):
    fileOver = False
    file  = open("out.txt", "r")

    while not fileOver:
        line = file.readline()

        if "EXIT" in line:
            string = line.split()
            for elem in string:
                if elem.startswith("w="):
                    temp_wait.append(float(elem.strip("w=")))
                elif elem.startswith("ta="):
                    temp_ta.append(float(elem.strip("ta=")))
        
        #end of file
        if not line:
            fileOver = True
    
    file.close()

    return

def main():
    qlen = [50, 100, 250, 500]
    dlen = [0, 5, 10, 15, 20, 25]
    seed_values = []
    for i in range(20):
        seed_values.append(random.randint(0, 5000))

    #used to store the average values per pair of quantum and dispatch lengths
    wait_times = []
    ta_times = []

    #used to collect raw values of wait and turnaround times per pair of quantum and dispatch lengths
    temp_wait_times = []
    temp_ta_times = []
    
    print("Computing averages for...")
    for quantum in qlen:
        for dispatch in dlen:
            print("  Quantum: " + str(quantum) + "  Dispatch: " + str(dispatch) + ("..."))
            for i in range(len(seed_values)):
                command = "./simgen 1000 " + str(seed_values[i]) + " | ./rrsim --quantum " + str(quantum) + " --dispatch " + str(dispatch) + " > out.txt"
                os.system(command)

                #first obtain the raw times
                get_data(temp_wait_times, temp_ta_times, quantum)
                #then store their averages
                if i+1 == 20:
                    wait_times.append(mean(temp_wait_times))
                    temp_wait_times.clear()
                    ta_times.append(mean(temp_ta_times))
                    temp_ta_times.clear()
    print("Completed!\n")

    plot_values(wait_times, ta_times)

if __name__ == "__main__":
    main()