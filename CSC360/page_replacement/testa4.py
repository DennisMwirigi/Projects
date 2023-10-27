#!/usr/bin/python3
#
# Author:	Eric Power

"""
A test script for A4 of CSC 360, Spring 2021.
"""

# IMPORT STATEMENTS
import sys
import helpers as h

# KEY VARIABLES
TRACE_FOLDER = "./traces/"

# TESTS
tests = [
	# Easy Tests
	h.Test("E01", f"{h.get_test_folder()}pwr_in_01.txt",    2,    4,  "fifo"),
	h.Test("E02", f"{h.get_test_folder()}pwr_in_01.txt",    2,    4,  "lru"),
	h.Test("E03", f"{h.get_test_folder()}pwr_in_01.txt",    2,    4,  "secondchance"),
	h.Test("E04", f"{h.get_test_folder()}pwr_in_01.txt",    4,    8,  "fifo"),
	h.Test("E05", f"{h.get_test_folder()}pwr_in_01.txt",    4,    8,  "lru"),
	h.Test("E06", f"{h.get_test_folder()}pwr_in_01.txt",    4,    8,  "secondchance"),
	h.Test("E07", f"{h.get_test_folder()}pwr_in_02.txt",    4,    8,  "fifo"),
	h.Test("E08", f"{h.get_test_folder()}pwr_in_02.txt",    4,    8,  "lru"),
	h.Test("E09", f"{h.get_test_folder()}pwr_in_02.txt",    4,    8,  "secondchance"),
	h.Test("E10", f"{h.get_test_folder()}pwr_in_01.txt",    1,    8,  "fifo"),
	h.Test("E11", f"{h.get_test_folder()}pwr_in_02.txt",    1,    8,  "fifo"),
	h.Test("E12", f"{h.get_test_folder()}pwr_in_03.txt",    1,    8,  "fifo"),
	h.Test("E13", f"{h.get_test_folder()}pwr_in_01.txt",    1,    8,  "lru"),
	h.Test("E14", f"{h.get_test_folder()}pwr_in_02.txt",    1,    8,  "lru"),
	h.Test("E15", f"{h.get_test_folder()}pwr_in_03.txt",    1,    8,  "lru"),
	h.Test("E16", f"{h.get_test_folder()}pwr_in_01.txt",    1,    8,  "secondchance"),
	h.Test("E17", f"{h.get_test_folder()}pwr_in_02.txt",    1,    8,  "secondchance"),
	h.Test("E18", f"{h.get_test_folder()}pwr_in_03.txt",    1,    8,  "secondchance"),

	# Real FIFO Tests
	h.Test("F01", f"{TRACE_FOLDER}hello_out.txt",      12,  100,  "fifo"),
	h.Test("F02", f"{TRACE_FOLDER}ls_out.txt",         10,  64,   "fifo"),
	h.Test("F03", f"{TRACE_FOLDER}matrixmult_out.txt", 8,   256,  "fifo"),
	h.Test("F04", f"{TRACE_FOLDER}hello_out.txt",      6,   256,  "fifo"),
	h.Test("F05", f"{TRACE_FOLDER}matrixmult_out.txt", 5,   64,   "fifo"),
	h.Test("F06", f"{TRACE_FOLDER}ls_out.txt",         14,  128,  "fifo"),
	h.Test("F07", f"{TRACE_FOLDER}matrixmult_out.txt", 7,   32,   "fifo"),
	h.Test("F08", f"{TRACE_FOLDER}hello_out.txt",      3,   512,  "fifo"),
	h.Test("F09", f"{TRACE_FOLDER}ls_out.txt",         3,   8,    "fifo"),
	h.Test("F10", f"{TRACE_FOLDER}hello_out.txt",      4,   8,    "fifo"),
	
	# Real LRU Tests
	h.Test("L01", f"{TRACE_FOLDER}hello_out.txt",      12,  100,  "lru"),
	h.Test("L02", f"{TRACE_FOLDER}ls_out.txt",         10,  64,   "lru"),
	h.Test("L03", f"{TRACE_FOLDER}matrixmult_out.txt", 8,   256,  "lru"),
	h.Test("L04", f"{TRACE_FOLDER}hello_out.txt",      6,   256,  "lru"),
	h.Test("L05", f"{TRACE_FOLDER}matrixmult_out.txt", 5,   64,   "lru"),
	h.Test("L06", f"{TRACE_FOLDER}ls_out.txt",         14,  128,  "lru"),
	h.Test("L07", f"{TRACE_FOLDER}matrixmult_out.txt", 7,   32,   "lru"),
	h.Test("L08", f"{TRACE_FOLDER}hello_out.txt",      3,   512,  "lru"),
	h.Test("L09", f"{TRACE_FOLDER}ls_out.txt",         3,   8,    "lru"),
	h.Test("L10", f"{TRACE_FOLDER}hello_out.txt",      4,   8,    "lru"),

	# Real Second Chance Tests
	h.Test("S01", f"{TRACE_FOLDER}hello_out.txt",      12,  100,  "secondchance"),
	h.Test("S02", f"{TRACE_FOLDER}ls_out.txt",         10,  64,   "secondchance"),
	h.Test("S03", f"{TRACE_FOLDER}matrixmult_out.txt", 8,   256,  "secondchance"),
	h.Test("S04", f"{TRACE_FOLDER}hello_out.txt",      6,   256,  "secondchance"),
	h.Test("S05", f"{TRACE_FOLDER}matrixmult_out.txt", 5,   64,   "secondchance"),
	h.Test("S06", f"{TRACE_FOLDER}ls_out.txt",         14,  128,  "secondchance"),
	h.Test("S07", f"{TRACE_FOLDER}matrixmult_out.txt", 7,   32,   "secondchance"),
	h.Test("S08", f"{TRACE_FOLDER}hello_out.txt",      3,   512,  "secondchance"),
	h.Test("S09", f"{TRACE_FOLDER}ls_out.txt",         3,   8,    "secondchance"),
	h.Test("S10", f"{TRACE_FOLDER}hello_out.txt",      4,   8,    "secondchance")
]

# CONTROL FLOW
def main():
	"""
	
	:return:0 on success, 1 on failure.
	"""
	create_flag = False
	verbose_flag = False
	if '-v' in sys.argv:
		verbose_flag = True
	if '-c' in sys.argv:
		create_flag = True
	if 'h' in sys.argv:
		print_help()
		exit(0)

	if verbose_flag:  # Clears out the verbose_out.txt file
		fp = open( h.get_verbose_out_file(), "w")
		fp.close()

	if create_flag:
		for test in tests:
			test.create_output(verbose_flag)
	else:
		for test in tests:
			test.run(verbose_flag)


def print_help():
	h.printx("Useage: ", ['blue', 'bold'])
	h.printx("./testa4.py [flags]\n", 'bold')
	print("\nThis program tests A4 for CSC 360. The flags are optional, and described below.")
	print("The program will indicate which test it is running. If it runs into an error, it")
	print("prints all the relavant information and stops. If it finds our outputs don't match,")
	print("it will print out the differences, and continue.")
	print()
	print("Flags:")
	print("    -h : Prints this help message")
	print("    -v : Saves your output to verbose_o")
	print()
	h.printx("Note: ", ['blue', 'bold'])
	print("it's quite likely at this point that there are errors in my code, please let")
	print("me know if you think there's a bug in my code so I can update it, and these tests!")

if __name__ == '__main__':
	main()


