Parsing and Serialization code - parse_serialize.cpp
Deserialization and output write code - deserialize_write.cpp

Timing throughput statistics report - statistics.txt
Names of binary files (input for deserialization process) - serialized_files.txt
Final output write - output.csv

Miscellaneous folder to store serialized binary files - 'serialization-files/'

Here, I will explain the Makefile commands:- 

CINPUT=? (Replace ? with name of .csv file before every make run)

1. make compile - 
		Will compile the two C++ code (parse_serialize.cpp and deserialize_write.cpp) using gcc and create two object files, namely parse_serialize.o and deserialize_write.o

2. make run - 
		Will create a new folder called 'serialization-files/' to store the binaries of serialization process
		Will run parse_serialize.o on the given input file
		Will run deserialize_write.o on a file named 'serialized_files.txt' which will store locations of binaries of serialization step. Generates an output.csv file that will store the final contents of the deserialzation process

3. make diff - 
		diff input_file output.csv (to find out if they are similar)

4. make display_stats - 
		Display the timing throughput statistics in the file 'statistics.txt', generated as a result of executing the two executables. Will report the time taken to run parse+serialization, deserialization, and output write.

5. make display_binary_folder - 
		Display the contents of the folder 'serialization-files/' created during parsing and serialization process that store the serialized binary files per symbol.

6. make display_serialized_files - 
		Display the contents of file 'serialized_files.txt'

7. make clean - 
		Removes output.csv, serialized_files.txt, statistics.txt and serialization-files/
		NOTE: It is important to do a make clean before you run new input files.
		
