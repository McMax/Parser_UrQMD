#include <iostream>
#include <iomanip>
#include <fstream>
#include <string>
#include <sstream>
#include <cstdlib>

using namespace std;

void closefiles();
void clearbuffers();
bool my_getline(istream&, string&);
string skipToNextEvent(istream&);

ifstream file_input;
ofstream file_output;

string buffer="";	//Buffer string for event header
string templine;

int main(int argc, char** argv)
{
	if(argc != 3)
	{
		cout << "Usage: cleaner <inputfile> <outputfile> <multiplicity>" << endl;
		return 1;
	}

	string inputfile = argv[1];
	string outputfile = argv[2];

	cout << "Reading from file: " << inputfile << " and writing out to file: " << outputfile << endl;
	
	//Opening files
	file_input.open(inputfile.c_str());
	file_output.open(outputfile.c_str());

	string check;
	stringstream ss;

	unsigned int i, mult, events=0, bad_events=0;
	bool last_event_good = true;

	while(true)
	{	
		if(last_event_good)		//If the previous event was skipped, then the first line of header is already read
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
				//cout << "Line: " << templine << endl;
				return 3;
			}
			else
			{
				buffer.append(templine);
				buffer.append("\n");
			}
		}
		else
			last_event_good = true;
		
		if((++events%100)==0)
			cout << "Event: " << events << endl;


		//Next 16 lines from header
		for(i=0;i<16;i++)
		{
			if(my_getline(file_input,templine))
			{
				cout << "Event: " << events << " - corrupted header" << endl;
				//cout << "Line: " << templine << endl;
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
			closefiles();
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
			closefiles();
			return 6;
		}
		else
		{
			buffer.append(templine);
			buffer.append("\n");
		}
		
		last_event_good = true;
		
		//Particle list
		for(i=0;i<mult;i++)
		{
			if(my_getline(file_input, templine))
			{
				string uqmdline = templine;
				cout << "Event: " << events << " - corrupted particle list" << endl;
				//Cleaning buffers because we don't need corrupted event
				clearbuffers();
				//Appending first line from new event to fresh buffer
				buffer.append(uqmdline);
				buffer.append("\n");
				//Jumping back to the top of this loop
				last_event_good = false;
				++bad_events;
				break;
			}
			else
			{
				buffer.append(templine);
				buffer.append("\n");
			}
		}

		if(!last_event_good)
			continue;
		
		//If we are here, then the event has been read correctly. Now it will be re-written to the output file

		//Writing to output file from buffer
		file_output << buffer;

		//Clear strings
		clearbuffers();
	}

	closefiles();

	cout << "Number of skipped bad events: " << bad_events << endl;

	return 0;
}

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

string skipToNextEvent(istream& is)
{
	static string nothing = "";
	unsigned int counter = 0;

	while(!my_getline(is,nothing))	//Fast skipping until UQMD word is found. TODO - handle the case of end-of-file
		++counter;
	
	//If we are here, the "UQMD" word was finally found
	cout << "Skipped " << counter << " lines" << endl;
	
	//Returning the line which was found
	return nothing;
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


