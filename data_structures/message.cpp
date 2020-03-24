#include <math.h> 
#include <assert.h>
#include <iostream>
#include <fstream>
#include <string>

#include "message.hpp"
#include "../usrlib/StratAirliftUtils.hpp"

// oPallet input and output streams
ostream& operator<<(ostream& os, const oPallet& msg)
{
	os << "Pallet: " << msg.iId << " Expires At: " << ConvertToDays(msg.tExpire) << " Days (" << ConvertToInt(msg.tExpire) << " Minutes)";
	return os;
}

istream& operator>> (istream& is, oPallet& msg)
{
	int tExpireTime;
	
	is >> msg.iId;
	is >> tExpireTime;
	msg.tExpire=TIME(tExpireTime);
	return is;
}

// oLoad input and output streams
ostream& operator<<(ostream& os, const oLoad& msg)
{
	os << "Load: " << msg.iId << " Expires At: " << ConvertToDays(msg.tExpire) << " Days (" << ConvertToInt(msg.tExpire) << " Minutes) " << " Flight Time: " << ConvertToHours(msg.tFlight) << " Hours (" << ConvertToInt(msg.tFlight) << " Minutes)";
	return os;
}

istream& operator>> (istream& is, oLoad& msg)
{
	is >> msg.iId;
	
	int tTime;
	is >> tTime;
	msg.tExpire=TIME(tTime);
	is >> tTime;
	msg.tFlight=TIME(tTime);
	return is;
}

// oAircraftStatus input and output streams
ostream& operator<<(ostream& os, const oAircraftStatus& msg)
{
	os << "Aircraft Status (" << msg.iACId << "): " << (msg.bStatus ? "Avaliable" : "Busy");
	return os;
}

istream& operator>> (istream& is, oAircraftStatus& msg)
{
	is >> msg.iACId;
	
	int tStatus;
	is >> tStatus;
	msg.bStatus = tStatus==1 ? true : false;
	return is;
}

// oACLoad input and output streams
ostream& operator<<(ostream& os, const oACLoad& msg)
{
	os << "Aircraft Load: " << msg.iACId << " Carrying Load: " << msg.myLoad.iId << " Expires At: " << ConvertToDays(msg.myLoad.tExpire) << " Days (" << ConvertToInt(msg.myLoad.tExpire) << " Minutes) " << " Flight Time: " << ConvertToHours(msg.myLoad.tFlight) << " Hours (" << ConvertToInt(msg.myLoad.tFlight) << " Minutes)";

	return os;
}

istream& operator>> (istream& is, oACLoad& msg)
{
	int tiId, ttExpire, ttFlight;
	
	is >> msg.iACId;
	is >> tiId;
	is >> ttExpire;
	is >> ttFlight;
	
	msg.myLoad=oLoad(tiId,TIME(ttExpire),TIME(ttFlight));
	return is;
}
