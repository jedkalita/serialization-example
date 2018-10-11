#include <iostream>
#include <fstream>
#include <sstream>
#include <map>
#include <vector>
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

  //check if name of binary files location has been provided
  if (argc != 2)
    {
      cout << "Need name of file containing location of binaries...Exiting." << endl;
      return 1;
    }
  
  std::ifstream infile; //ifstream for each binary file per symbol
  std::map<int, std::string> linenum_to_line; //the map that will be constructed while deserializing that will store line number to line - will be sorted in ascending order of keys (line number)
  std::vector<string> vect; //a vector for names of binary files which will each be iterated upon for deserializing

  std::ifstream binaries(argv[1]); //binary file location open

  //in case the binary files location passed does not exist, then exit program
  if(!binaries)
    {
      cout << "Cannot open binary file information.\n";
      return 1;
    }
  
  string names_of_files; //each binary file name
  
  //each binary file to be pushed into a vector 
  while (getline(binaries, names_of_files))
    {
      vect.push_back(names_of_files);
    }

  timestamp_t t_before_deserialization = get_timestamp(); //timestamp right before deserialization process

  
  //Deserialization process - for each binary file, read line by line, extract line number and line string and put into a map. Finally write back from the map the lines and the output of the deserialization process will recreate contents of the original input file.

  //iterate for each binary serialized file
  for (int i = 0; i < vect.size(); i++)
    {
      
      infile.open(vect.at(i), ifstream::binary); //open the binary file
  
      std::string line; //each line of the binary
      while(!infile.eof()) //until you reach end of binary
	{
	  unsigned int size = 0;        
	  infile.read(reinterpret_cast<char *>(&size), sizeof(size)  );
    
	  // Allocate a string, make it large enough to hold the input
	  string buffer;
	  buffer.resize(size);
    
	  // read the text into the string
	  infile.read(&buffer[0],  buffer.size() );

	  std::stringstream ss(buffer); //a stringstream to split based on a pound sign (#) that was created as a delimiter during serialization between the line number and the line it represents

	  int iteration = 0; //we know that there will be 2 splits based on the delimiter for each line of the binary
	  int key;
	  string value;
	  while (ss.good()) //each delimited string line
	    {
	      string substring;
	      getline(ss, substring, '#'); //delimit based on #
	      if (iteration == 0)
		{
		  key = atoi(substring.c_str()); //the first split will be integer key,i.e., the line number
		}
	      else
		{
		  value = substring; //the second split will be the line
		}
	      iteration++;
	    }
	  linenum_to_line.insert(std::make_pair(key, value)); //insert into map <linenumber, line> for final output in order

	} //the end of the deserialization process

      infile.close();
    }

  timestamp_t t_after_deserialization = get_timestamp(); //time stamp after the deserialization process
  double time_for_deserialization = (t_after_deserialization -
					   t_before_deserialization) / 1000000.0L; //get the time needed for deserialization

  //now, write the throughput timing stats per function to an output file - same output file as the parsing and serialization process
  ofstream stats("statistics.txt", std::ofstream::app | std::ofstream::binary);
  stats << "Time taken for deserialization = " << time_for_deserialization << " seconds." << endl;



  //The Output to file after deserialization process
  string name_of_output_file = "output.csv"; //the output .csv file
  ofstream ofs(name_of_output_file);
  //we will have to iterate over the created map from deserialization process and only extract out the value part, i.e., the lines represented by the keys (line numbers)
  std::map<int, std::string>::iterator it2 = linenum_to_line.begin(); //initialize the iterator over the map
  it2++;
  timestamp_t t_before_output_write = get_timestamp(); //get the time stamp before output process begins

  //iterate for the entire map
  while (it2 != linenum_to_line.end()) 
    {
      ofs << it2->second; //output to the output file
      it2++;
    }

  timestamp_t t_after_output_write = get_timestamp(); //get the time stamp after output process ends
  double time_for_output_write = (t_after_output_write -
				  t_before_output_write) / 1000000.0L; //find out the time it took for the output process to complete

  //now, write the throughput timing stats per function to an output file 
  stats << "Time taken for output write = " << time_for_output_write << " seconds." << endl; 
  stats.close();
  
  
  ofs.close();

  //end of deserialization + output write processes
}
