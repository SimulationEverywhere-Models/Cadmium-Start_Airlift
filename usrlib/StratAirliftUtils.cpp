#include <iostream>
#include <fstream>
#include <string>
#include <boost/rational.hpp>

#include "EIRational.hpp"
#include <sys/types.h>
#include <sys/stat.h>
#include <string>

using namespace std;

const int DAYS_IN_MIN=1440;				// 1440 - the number of minutes in a day
const int HOURS_IN_MIN=60;				// 60 - the number of minutes in an hour

string LOG_DIR="../simulation_results/";
string INPUT_DIR="../input_data/";
int SCENARIO_STOP=14400;

int PALLET_GEN_MAX_EXPIRE=10;
int PALLET_GEN_MIN_NEW=4;
int PALLET_GEN_MAX_NEW=8;

int LOAD_AC_SIZE=4;
int LOAD_FLIGHT_MIN=3;
int LOAD_FLIGHT_MAX=30;
int LOAD_EXPIRE_BUFFER=LOAD_FLIGHT_MAX*HOURS_IN_MIN;

int AIRCRAFT_NUM=2;

int AIRCRAFT_MAINT_TIME=2*DAYS_IN_MIN;
int AIRCRAFT_MAINT_LIMIT=100*HOURS_IN_MIN;

int DEST_PROCESS_TIME=1*HOURS_IN_MIN;

// Allows the user to override base options with an external file if required
void ReadOptions(string sOptFile)
{
	string sLine;
	string sKey;
	string sValue;
	
	cout << "Reading Options File: " << sOptFile << "\n";
	
	ifstream fOptFile (sOptFile);	
	if(fOptFile.is_open())
	{
		while(getline(fOptFile,sLine))
		{
			sKey=sLine.substr(0,sLine.find("="));
			sValue=sLine.substr(sLine.find("=")+1,sLine.length());
			sValue.erase(std::remove(sValue.begin(), sValue.end(), '\n'),sValue.end());
			sValue.erase(std::remove(sValue.begin(), sValue.end(), '\r'),sValue.end());			
			if(sKey.compare("LOG_DIR")==0)
			{
				LOG_DIR=sValue;
				cout << "Updated LOG_DIR to " << LOG_DIR << "\n";
			}else if(sKey.compare("INPUT_DIR")==0)
			{
				INPUT_DIR=sValue;
				cout << "Updated INPUT_DIR to " << INPUT_DIR << "\n";
			}else if(sKey.compare("SCENARIO_STOP")==0)
			{
				SCENARIO_STOP=stoi(sValue);
				cout << "Updated SCENARIO_STOP to " << SCENARIO_STOP << "\n";
			}else if(sKey.compare("PALLET_GEN_MAX_EXPIRE")==0)
			{
				PALLET_GEN_MAX_EXPIRE=stoi(sValue);
				cout << "Updated PALLET_GEN_MAX_EXPIRE to " << PALLET_GEN_MAX_EXPIRE << "\n";
			}else if(sKey.compare("PALLET_GEN_MIN_NEW")==0)
			{
				PALLET_GEN_MIN_NEW=stoi(sValue);
				cout << "Updated PALLET_GEN_MIN_NEW to " << PALLET_GEN_MIN_NEW << "\n";
			}else if(sKey.compare("PALLET_GEN_MAX_NEW")==0)
			{
				PALLET_GEN_MAX_NEW=stoi(sValue);
				cout << "Updated PALLET_GEN_MAX_NEW to " << PALLET_GEN_MAX_NEW << "\n";
			}else if(sKey.compare("LOAD_EXPIRE_BUFFER")==0)
			{
				LOAD_EXPIRE_BUFFER=stoi(sValue);
				cout << "Updated LOAD_EXPIRE_BUFFER to " << LOAD_EXPIRE_BUFFER << "\n";
			}else if(sKey.compare("LOAD_AC_SIZE")==0)
			{
				LOAD_AC_SIZE=stoi(sValue);
				cout << "Updated LOAD_AC_SIZE to " << LOAD_AC_SIZE << "\n";
			}else if(sKey.compare("LOAD_FLIGHT_MIN")==0)
			{
				LOAD_FLIGHT_MIN=stoi(sValue);
				cout << "Updated LOAD_FLIGHT_MIN to " << LOAD_FLIGHT_MIN << "\n";
			}else if(sKey.compare("LOAD_FLIGHT_MAX")==0)
			{
				LOAD_FLIGHT_MAX=stoi(sValue);
				cout << "Updated LOAD_FLIGHT_MAX to " << LOAD_FLIGHT_MAX << "\n";
			}else if(sKey.compare("AIRCRAFT_NUM")==0)
			{
				AIRCRAFT_NUM=stoi(sValue);
				cout << "Updated AIRCRAFT_NUM to " << AIRCRAFT_NUM << "\n";
			}else if(sKey.compare("AIRCRAFT_MAINT_TIME")==0)
			{
				AIRCRAFT_MAINT_TIME=stoi(sValue);
				cout << "Updated AIRCRAFT_MAINT_TIME to " << AIRCRAFT_MAINT_TIME << "\n";
			}else if(sKey.compare("AIRCRAFT_MAINT_LIMIT")==0)
			{
				AIRCRAFT_MAINT_LIMIT=stoi(sValue);
				cout << "Updated AIRCRAFT_MAINT_LIMIT to " << AIRCRAFT_MAINT_LIMIT << "\n";
			}else if(sKey.compare("DEST_PROCESS_TIME")==0)
			{
				DEST_PROCESS_TIME=stoi(sValue);
				cout << "Updated DEST_PROCESS_TIME to " << DEST_PROCESS_TIME << "\n";
			}
		}
		fOptFile.close();
	}else
	{
		cout << "Unable to Open Option File: " << sOptFile << "\n"; 
	}
}

// Performs house keeping before the simulation is run
void PerformSetup()
{
	struct stat dirInfo;
	
	// Ensures the log directory is created so that we can store results.
	if( stat( LOG_DIR.c_str(), &dirInfo ) != 0 )
	{
		if(mkdir(LOG_DIR.c_str(),0777)==0)
		{
			cout << "Log directory: " << LOG_DIR << " created.\n";
		}else
		{
			cout << "Unable to create log directory: " << LOG_DIR << ".\n";
		}
	}else if( dirInfo.st_mode & S_IFDIR )
	{
		cout << "Log directory: " << LOG_DIR << " found.\n";
	}		
}

// Converts from EIRational time to number of days for output
int ConvertToDays(EIRational tTime)
{
	return boost::rational_cast<int>(tTime._value)/DAYS_IN_MIN;
}

// Converts from EIRational time to number of hours for output
int ConvertToHours(EIRational tTime)
{
	return boost::rational_cast<int>(tTime._value)/HOURS_IN_MIN;
}

// Converts from EIRational time to integer for output
int ConvertToInt(EIRational tTime)
{
	return boost::rational_cast<int>(tTime._value);
}
