#include <iostream>
#include <iomanip>
#include <fstream>
#include <string>
#include <sstream>
#include <cstdlib>

using namespace std;

void closefiles();
void clearbuffers();

ifstream file_input;
ofstream file_output;

string buffer="";	//Buffer string for event header
string templine;

bool my_getline(istream& is, string& line_buffer)
{
	static stringstream sstream;
	static string firstword;

	firstword = "";

	getline(is, line_buffer);
	sstream.str(line_buffer);
	sstream.clear();
	sstream >> firstword;

	if(firstword.compare("UQMD")==0)
		return true;
	else
		return false;
}

int main(int argc, char** argv)
{
	if(argc != 4)
	{
		cout << "Usage: cleaner <inputfile> <outputfile> <multiplicity>" << endl;
		return 1;
	}

	string inputfile = argv[1];
	string outputfile = argv[2];
	unsigned int elastic_multiplicity = atoi(argv[3]);

	cout << "Reading from file: " << inputfile << " and writing out to file: " << outputfile << endl;
	
	//Opening files
	file_input.open(inputfile.c_str());
	file_output.open(outputfile.c_str());

	string check;
	stringstream ss;

	unsigned int i, mult, events=0;;

	while(true)
	{	
		//Getting first line
		if(!(file_input.good()))
		{
			cout << "Reading finished" << endl;
			if(file_input.eof())
				cout << "End of file reached" << endl;
			else
			{
				cerr << "Reading error" << endl;
				closefiles();
				return 2;
			}

			break;
		}

		if(!(my_getline(file_input,templine)))	//I expect "UQMD" word here
		{
			cerr << "Inputfile corrupted. Last good event: " << events << endl;
			closefiles();

			return 3;
		}
		else
		{
			buffer.append(templine);
			buffer.append("\n");
		}
		
		if((++events%100)==0)
			cout << "Event: " << events << endl;


		//Next 16 lines from header
		for(i=0;i<16;i++)
		{
			if(my_getline(file_input,templine))
			{
				cout << "Event: " << events << " - corrupted header" << endl;
				return 4;
			}
			else
			{
				buffer.append(templine);
				buffer.append("\n");
			}
		}
		
		//Multiplicity line
		if(my_getline(file_input, templine))
		{
			cout << "Event: " << events << " - could not find multiplicity line" << endl;
			return 5;
		}
		else
		{
			buffer.append(templine);
			buffer.append("\n");
		}

		ss.str(templine);
		ss.clear();
		ss >> mult;

		//Collisions line
		if(my_getline(file_input, templine))
		{
			cout << "Event: " << events << " - could not find collisions line" << endl;
			return 6;
		}
		else
		{
			buffer.append(templine);
			buffer.append("\n");
		}
		
		//Particle list
		for(i=0;i<mult;i++)
		{
			if(my_getline(file_input, templine))
			{
				cout << "Event: " << events << " - corrupted particle list." << endl;
				return 7;
			}
			else
			{
				buffer.append(templine);
				buffer.append("\n");
			}
		}
		
		//If we are here, then the event has been read correctly. Now it will be re-written to the output file

		//Writing to output file from buffer
		file_output << buffer << endl;

		//Clear strings
		clearbuffers();
	}

	closefiles();

	return 0;
}

void closefiles()
{
	file_input.close();
	file_output.close();
}

void clearbuffers()
{
	templine.clear();
	buffer.clear();
}
