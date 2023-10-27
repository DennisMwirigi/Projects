#!/usr/bin/python3
#
# Author:	Eric Power

"""
Helper functions for testa4.py
"""

# IMPORT STATEMENTS
from colorama import Fore
from colorama import Style
import subprocess

# KEY VARIABLES
TEST_FOLDER = "./pwr_tests/"
VERBOSE_OUT = "verbose_out.txt"

# TEST CLASS
class Test:
	
	def __init__(self, test_id, input_file, frame_size, num_frames, scheme):
		# assert (n & (n-1) == 0) and n != 0, "frame_size must be a power of 2"
		self.test_id = test_id
		self.input_file = input_file
		self.frame_size = frame_size
		self.num_frames = num_frames
		self.scheme = scheme
		self.expected_out = f"{TEST_FOLDER}exp_out_{self.test_id}.txt"

	def run(self, verbose):

		printx("RUNNING: ", ['blue', 'bold'])
		print(f"Test {self.test_id}", end='\r')

		command = f"./virtmem --file={self.input_file} --framesize={self.frame_size} --numframes={self.num_frames} --replace={self.scheme}"
		try:
			output = subprocess.check_output(command, shell=True, text=True)
		except subprocess.CalledProcessError as e:
			
			printx("ERROR: ", ['red', 'bold'])
			print(f"Test {self.test_id} returned an error code.")
			printx("RAN COMMAND:", ['yellow'])
			print(command)
			printx("ERROR MESSAGE: ", ["yellow"])
			print(e)
			exit(1)

		exp_out_fp = open(self.expected_out, "r")
		exp_out = exp_out_fp.read().split('\n')
		output = output.split('\n')

		error = False
		if len(exp_out) != len(output):
			error = True
		for i in range(len(exp_out)):
			if i >= len(output):
				error = True
				break
			if exp_out[i] != output[i]:
				error = True
				break

		if error:
			printx("ERROR: ", ['red', 'bold'])
			printx(f"Test {self.test_id} created unexpected output.\n", ['bold'])
			printx("\n    RAN COMMAND:", ['yellow'])
			print(command)
			printx("\n    EXPECTED OUTPUT:", ["yellow"])
			[print("    " + str(i)) for i in exp_out]
			printx("\n    CREATED OUTPUT:", ["yellow"])
			[print("    " + str(i)) for i in output]
			print()
		if verbose:
			fp = open(VERBOSE_OUT, "a")
			fp.write(f"Test {self.test_id} Output:\n")
			fp.write(f"Command: {command}")
			for i in output:
				fp.write("    "+ i.strip() + "\n")
			fp.close()

	def create_output(self, verbose):

		printx("SAVING OUTPUT: ", ['blue', 'bold'])
		print(f"Test {self.test_id}", end='\r')

		command = f"./virtmem --file={self.input_file} --framesize={self.frame_size} --numframes={self.num_frames} --replace={self.scheme}"
		try:
			output = subprocess.check_output(command, shell=True, text=True)
		except subprocess.CalledProcessError as e:
			
			printx("ERROR: ", ['red', 'bold'])
			print(f"Test {self.test_id} returned an error code.")
			printx("RAN COMMAND:", ['yellow'])
			print(command)
			printx("ERROR MESSAGE: ", ["yellow"])
			print(e)
			exit(1)

		exp_out_fp = open(self.expected_out, "w")
		exp_out_fp.write(output)
		exp_out_fp.close()

		if verbose:
			fp = open(VERBOSE_OUT, "a")
			fp.write(f"Test {self.test_id} Output:\n")
			fp.write(f"Command: {command}")
			for i in output:
				fp.write("    "+ i.strip() + "\n")
			fp.close()


def get_test_folder():
	return TEST_FOLDER


def get_verbose_out_file():
	return VERBOSE_OUT


# OUTPUT HELPERS
style_dict = {
        "blue":Fore.BLUE,
        "cyan":Fore.CYAN,
        "green":Fore.GREEN,
        "red":Fore.RED,
        "white":Fore.WHITE,
        "yellow":Fore.YELLOW,
        "bold":Style.BRIGHT,
        "none":Style.RESET_ALL
}

def printx(msg, style="none"):
        strn = ""
        if type(style) is list:
                for i in style:
                        strn += style_dict[i]
        else:
                strn = style_dict[style]
        print(f"{strn}{msg}{Style.RESET_ALL}", end="")


