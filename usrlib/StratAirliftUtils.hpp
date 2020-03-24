#ifndef STRATAIRLIFTUTILS_HPP
#define STRATAIRLIFTUTILS_HPP

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

extern string LOG_DIR;					// Directory to store the logs
extern string INPUT_DIR;				// Directory for input files
extern int SCENARIO_STOP;				// Time to stop the scenario at

extern int PALLET_GEN_MAX_EXPIRE;		// Max number of days a pallet will live for
extern int PALLET_GEN_MIN_NEW;			// Minimum number of hours before the next pallet is generated
extern int PALLET_GEN_MAX_NEW;			// Max number of hours before the next pallet is generated

extern int LOAD_EXPIRE_BUFFER;			// Max flight time.  Buffer required to ensure load has reasonable chances of making it to destination.
extern int LOAD_AC_SIZE;				// Number of pallets an aircraft can carry
extern int LOAD_FLIGHT_MIN;				// Minimum flight time to a destination
extern int LOAD_FLIGHT_MAX;				// Maximum flight time to a destination

extern int AIRCRAFT_NUM;				// Number of aircraft in the scenario

extern int AIRCRAFT_MAINT_TIME;			// The number of days on average aircraft spend in maintenance when they reach a fixed number of hours
extern int AIRCRAFT_MAINT_LIMIT;		// The number of minutes before the next scheduled maintenance period

extern int DEST_PROCESS_TIME;			// The average processing time for a load at the destination

void ReadOptions(string sOptFile);		// Allows the user to override base options with an external file if required
void PerformSetup();					// Performs house keeping before the simulation is run

int ConvertToDays(EIRational tTime);	// Converts from EIRational time to number of days for output
int ConvertToHours(EIRational tTime);	// Converts from EIRational time to number of hours for output
int ConvertToInt(EIRational tTime);		// Converts from EIRational time to integer for output

#endif // STRATAIRLIFTUTILS_HPP