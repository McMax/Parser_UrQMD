#include <iostream>
#include <iomanip>
#include <fstream>
#include <string>

#include "TMath.h"
#include "ParticleTree.h"

using namespace std;

int main(int argc, char** argv)
{
	TString inputfile_path, outputfile_path;

	if(argc != 3)
	{
		inputfile_path = "outputfile";
		outputfile_path = "ParticleTree.root";
	}
	else
	{
		inputfile_path = argv[1];
		outputfile_path = argv[2];
	}

	cout << "Reading from file: " << inputfile_path << " and writing out to file: " << outputfile_path << endl;

	bool verbose = false;
	bool debug = false;
	string line, check;
	string thrash;
	int event_read, a1, a2, a3;
	float r, E, px, py, pz, m;
	int id;
	short ch;
	unsigned int nPa, iter, event_no=0, skipped=0;

	ifstream inputfile(inputfile_path,ifstream::in);
	ParticleTree particletree(outputfile_path);

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
		//for(int i=0; i<9; i++)
		//	getline(inputfile, line);
		//Skip 12 lines
		for(int i=0; i<12; i++)
			getline(inputfile, line);

		inputfile >> nPa >> thrash;
		if(verbose)
			cout << "Number of particles: " << nPa << endl;

		inputfile >> a1 >> a2 >> a3;
		getline(inputfile,line);

		if(verbose)
			cout << "a1: " << a1 << " a2: " << a2 << " a3: " << a3 << endl;

		//if((a1==0) || (a3==0) || (nPa <= 16))
		//{
		//	//cout << "Wrong event " << event_no << endl << "a1=" << a1 << " a3=" << a3 << " npart=" << nPa << endl << "Skipping" << endl;
		//	++skipped;
		//	if(debug)
		//		cout << event_no << " 0" << endl;

		//	for(iter=0; iter<nPa; iter++)
		//		getline(inputfile, line);
		//}
		//else
		{
			particletree.BeginEvent();
			if(debug)
				cout << event_no << " " << nPa << endl;

			for(iter=0; iter<nPa; iter++)
			{
				inputfile >> r >> r >> r >> r >> E >> px >> py >> pz >> m >> id >> r >> ch;
				getline(inputfile, line);

				particletree.AddParticle(id,ch,px,py,pz,m);
				if(debug)
					cout << (iter+1) << " " << id << " " << ch << " " << setprecision(4) << px << " " << py << " " << pz << endl;

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

	return 0;
}
