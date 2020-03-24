#ifndef STRAT_AIR_SIM_MESSAGE_HPP
#define STRAT_AIR_SIM_MESSAGE_HPP

#include <assert.h>
#include <iostream>
#include <string>

#include "../usrlib/EIRational.hpp"

using namespace std;
using TIME = EIRational;

struct oPallet
{
	oPallet()
		:iId(0){}
	oPallet(int tiID, TIME ttExpire)
		:iId(tiID), tExpire(ttExpire){}
		
	int iId;
	TIME tExpire;
	
	bool operator<(const oPallet& rhs) const { tExpire < rhs.tExpire; }
};

ostream& operator<<(ostream& os, const oPallet& msg);
istream& operator>> (istream& is, oPallet& msg);

struct oLoad
{
	oLoad()
		:iId(0){}
	oLoad(int tiID, TIME ttExpire, TIME ttFlight)
		:iId(tiID), tExpire(ttExpire), tFlight(ttFlight){}
		
	int iId;
	TIME tExpire;
	TIME tFlight;
};

ostream& operator<<(ostream& os, const oLoad& msg);
istream& operator>> (istream& is, oLoad& msg);

struct oAircraftStatus
{
	oAircraftStatus()
		:bStatus(false), iACId(0){}
	oAircraftStatus(int tiACId, bool tbStatus)
		:iACId(tiACId), bStatus(tbStatus){}
	
	int iACId;
	bool bStatus;
};

ostream& operator<<(ostream& os, const oAircraftStatus& msg);
istream& operator>> (istream& is, oAircraftStatus& msg);

struct oACLoad
{
	oACLoad()
		:iACId(0){}
	oACLoad(int tiACID, oLoad tmyLoad)
		:iACId(tiACID), myLoad(tmyLoad){}
		
	int iACId;
	oLoad myLoad;
};

ostream& operator<<(ostream& os, const oACLoad& msg);
istream& operator>> (istream& is, oACLoad& msg);

#endif // STRAT_AIR_SIM_MESSAGE_HPP