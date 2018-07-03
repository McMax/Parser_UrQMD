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

	unsigned int i, mult, events=0;;

	while(true)
	{	
		//Getting first line
		getline(file_input,templine);
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

		stringstream ss_check;
		ss_check << templine;
		ss_check >> check;

		//Check if we are in the right place
		if(check.compare("UQMD")!=0)
		{
			cerr << "Inputfile corrupted. Last good event: " << events << endl;
			closefiles();

			return 3;
		}
		
		if((++events%100)==0)
			cout << "Event: " << events << endl;

		buffer.append(templine);
		buffer.append("\n");

		//Next 16 lines from header
		for(i=0;i<16;i++)
		{
			getline(file_input,templine);
			buffer.append(templine);
			buffer.append("\n");
		}
		
		//Multiplicity line
		getline(file_input, templine);
		stringstream ss;
		ss << templine;
		ss >> mult;

		if(mult == elastic_multiplicity)	//If elastic (i.e. no new particles produced)
		{
			//Skip collisions line and 16 next lines with particles
			for(i=0;i<17;i++)
				getline(file_input, templine);

			//Clear strings
			clearbuffers();

			continue;
		}
		
		//If we are here, then the event is elastic
		//Writing event header from buffer and multiplicity line residing in "templine"
		file_output << buffer << templine << endl;

		//Collisions line
		getline(file_input, templine);
		file_output << templine << endl;

		//Rewriting list of particles
		for(i=0; i<mult; i++)
		{
			getline(file_input, templine);
			file_output << templine << endl;
		}

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
