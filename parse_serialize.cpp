#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>
#include <map>
#include <set>
#include <sys/time.h> //for reporting time throughput statistics

typedef unsigned long long timestamp_t;

//function to get the time of day in microseconds
static timestamp_t get_timestamp()
{
  struct timeval now;
  gettimeofday(&now, NULL); //system call from sys.h to get current time of day
  return (now.tv_usec + (timestamp_t) now.tv_sec * 1000000); //convert from seconds to microseconds
}


using namespace std;

int main(int argc, char *argv[])
{
  //check if name of input file has been provided
  if (argc != 2)
    {
      cout << "Need name of quote file...Exiting." << endl;
      return 1;
    }
  
  string name_of_quote_file = argv[1]; //get name of the input file
  std::ifstream infile(name_of_quote_file); //open input file

  //in case the input file passed does not exist, then exit program
  if(!infile)
    {
      cout << "Cannot open input quote file.\n";
      return 1;
    }

 
  int num_lines = 0; //this variable is needed to find the mapping between line number within the input file and the string/line text it represents
  string line; //to read each input line
  std::set<string> names_of_files; //the set of binary files storing the serialized results per symbol - each symbol has a corresponding binary file

  timestamp_t t_before_parsing_serialization = get_timestamp(); //right before parsing & serialization process, get the time stamp - to subtract it later

  //We serialized as we parse each line of the input file.

  //in this step - parsing and serialization combined, we read line by line the input file, extract name of symbol representing each line, find the corresponding line number within the file, and then write in binary form the line number and the original line to the corresponding binary file.
  while(getline(infile, line)) //for each line in the input file
    {
      num_lines++; //increment line number variable
      
      std::vector<string> vect; //this construct is used so as to split each line into its constituents, and then extract the name of the symbol which is always in location 2
      std::stringstream ss(line); //read the line as a string stream

      while (ss.good()) //keep reading off of the split string
	{
	  string substring;
	  getline(ss, substring, ',');
	  vect.push_back(substring); //vector fill	 
	}
     
      string symbol_name = vect.at(1); //symbol name is at 2nd location

      string name_of_file = "serialization-files/" + symbol_name + ".bin"; //name of binary file corresponding to the symbol name. The first string is hardcoded as the name of the folder where the binaries will be placed. The Makefile will create the folder during execution and remove it during clean.

      names_of_files.insert(name_of_file); //the set consisting of file names so as to create a text file for input to the deserializing step that automatically locates the location of the serialized binaries

      //now, open and append in binary format the mapping of line number and line contents 
      ofstream ofs(name_of_file, std::ofstream::app | std::ofstream::binary);
      string text = to_string(num_lines) + "#" + line + "\n"; //the form of binary per line serialization dump
      unsigned int size = text.size();
      // write it to the file in binary
      ofs.write(reinterpret_cast<char *>(&size), sizeof(size)  );  
      ofs.write( text.c_str(), text.size() );
      ofs.close();
      
      
    }

  //now, we have read all of the lines of the input file
  infile.close();

  timestamp_t t_after_parsing_serialization = get_timestamp(); //time stamp after the parsing and serialization process
  double time_for_parsing_serialization = (t_after_parsing_serialization -
					   t_before_parsing_serialization) / 1000000.0L; //get the time needed for parsing and serialization

  //now, write the throughput timing stats per function to an output file
  ofstream stats("statistics.txt");
  stats << "Printing statistics per functionality for quote file : " << name_of_quote_file << endl;
  stats << "Time taken for parsing and serialization = " << time_for_parsing_serialization << " seconds." << endl;
  stats.close();
  
    

  //now, we need to do prepare the input to the next step - deserialization.
  //Here, from the set containing names of binaries along with their locations per symbol, we get each each binary file name and location and store into another file which will be fed as input to the deserialization process, which automatically will pull out the input serialized binaries.

  //an iterator to traverse the set of binary files
  std::set<string>::iterator set_it = names_of_files.begin();

  string file_names_serialized = "serialized_files.txt"; //this text file will be input to the deserialization process
  ofstream ofs(file_names_serialized); //output for write

  
  while (set_it != names_of_files.end())
    {
      ofs << *set_it << endl; //write name of each binary to text file
      set_it++;
    }


  //end of parsing, serialization, and creating of input for deserialization process.
  
  return 0;
}
  
