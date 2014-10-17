#include <iostream>
#include <fstream>
#include <string>

#include "ParticleTree.h"

using namespace std;

int main()
{
	bool verbose = false;
	string line, check;
	string thrash;
	int event_read, nPa, a1, a2, a3;
	float r, E, px, py, pz, m;
	int id;
	short ch;
	unsigned int iter, event_no=0, skipped=0;

	ParticleTree particletree("ParticleTree_UrQMD_158.root");
	ifstream inputfile("outputfile_pp158",ifstream::in);

	while(true)
	{
		inputfile >> check;

		if(!(inputfile.good()))
		{
			cout << "Reading finished" << endl;
			if(inputfile.eof())
				cout << "End of file reached" << endl;
			else
			{
				cout << "Reading error" << endl;
				return 0;
			}

			break;
		}

		if(check.compare("UQMD")!=0)
		{
			cerr << "Inputfile corrupted. Last good event: " << event_read << endl;
			return 0;
		}

		//Skip 5 lines
		for(int i=0; i<5; i++)
			getline(inputfile, line);

		inputfile >> check;

		if(check.compare("event#")!=0)
		{
			cerr << "Inputfile corrupted. Last good event: " << event_read << endl;
			return 0;
		}

		inputfile >> event_read;
		++event_no;

		if((verbose) || ((event_no%1000)==0))
			cout << "Event#: " << event_no << " | Event read: " << event_read << endl;

		//Skip 9 lines
		for(int i=0; i<9; i++)
			getline(inputfile, line);
		//Skip 12 lines
		//for(int i=0; i<12; i++)
		//	getline(inputfile, line);

		inputfile >> nPa >> thrash;
		if(verbose)
			cout << "Number of particles: " << nPa << endl;

		inputfile >> a1 >> a2 >> a3;
		getline(inputfile,line);

		if(verbose)
			cout << "a1: " << a1 << " a2: " << a2 << " a3: " << a3 << endl;

		if((a1==0) || (a3==0) || (nPa <= 2))
		{
			cout << "Wrong event " << event_no << endl << "a1=" << a1 << " a3=" << a3 << " npart=" << nPa << endl << "Skipping" << endl;
			++skipped;

			for(iter=0; iter<nPa; iter++)
				getline(inputfile, line);
		}
		else
		{
			particletree.BeginEvent();

			for(iter=0; iter<nPa; iter++)
			{
				inputfile >> r >> r >> r >> r >> E >> px >> py >> pz >> m >> id >> r >> ch;
				getline(inputfile, line);

				if(ch!=0)
				particletree.AddParticle(ch,0,0,px,py,pz,0,0,0,0,0,0,0,0,m);

				if(verbose)
					cout << "Particle " << iter << ":" << endl << "E=" << E << " px=" << px << " py=" << py << " pz=" << pz << " m=" << m << " id=" << id << " ch=" << ch << endl;
			}

			particletree.EndEvent();
		}
	}

	particletree.Close();
	inputfile.close();

	cout << "Events parsed: " << event_no << endl;

	if(skipped>0)
		cout << "Events skipped: " << skipped << endl;

	return 1;
}
