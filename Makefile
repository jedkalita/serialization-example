#input name of input quote records file
CINPUT=q.csv

compile:
		gcc -x c++ parse_serialize.cpp -lstdc++ -o parse_serialize.o
		gcc -x c++ deserialize_write.cpp -lstdc++ -o deserialize_write.o

run:
		mkdir serialization-files
		./parse_serialize.o $(CINPUT)
		./deserialize_write.o serialized_files.txt		

diff:
		diff $(CINPUT) output.csv

display_stats:
		vi statistics.txt

display_binary_folder:
		ls serialization-files/

display_serialized_files:
		vi serialized_files.txt

clean:
		rm -rf serialization-files
		rm serialized_files.txt
		rm output.csv
		rm statistics.txt
