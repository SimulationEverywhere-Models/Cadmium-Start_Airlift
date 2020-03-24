#ifndef LOADPACKING_HPP
#define LOADPACKING_HPP

#include <cadmium/modeling/ports.hpp>
#include <cadmium/modeling/message_bag.hpp>

#include <limits>
#include <assert.h>
#include <string>
#include <random>

#include <stdlib.h>

#include "../data_structures/message.hpp"
#include "../usrlib/StratAirliftUtils.hpp"

using namespace cadmium;
using namespace std;

//Port definition
struct LoadPacking_defs{
	struct outLoad : public out_port<oLoad> { };
	struct outExpired : public out_port<oPallet> { };
    struct inPallet : public in_port<oPallet> { };
    struct inACStatus : public in_port<oAircraftStatus> { };
};

template<typename TIME> class LoadPacking{

	enum ePhase {WAITING, LOADREADY};
	
    public:
        // default constructor
        LoadPacking() noexcept{
			state.currLoad=oLoad();
			state.currPhase=WAITING;
			state.sigma=std::numeric_limits<TIME>::infinity();
			state.currTime=TIME(0);
			state.iCurrLoadNum=1;
			state.iAircraftAvaliable=AIRCRAFT_NUM;
        }
        
        // state definition
        struct state_type{
			int iCurrLoadNum;
			oLoad currLoad;
			ePhase currPhase;			
            TIME sigma;
			TIME currTime;
			int iAircraftAvaliable;
			vector<oPallet> currPallets;
			vector<oPallet> expiredPallets;
        }; 
        state_type state;
		
        // ports definition
        using input_ports=std::tuple<typename LoadPacking_defs::inPallet, typename LoadPacking_defs::inACStatus>;
        using output_ports=std::tuple<typename LoadPacking_defs::outLoad, typename LoadPacking_defs::outExpired>;

        // internal transition
        void internal_transition()
		{
			switch(state.currPhase)
			{
				case LOADREADY:
					state.currPhase=WAITING;
					state.sigma=std::numeric_limits<TIME>::infinity();
					state.currLoad=oLoad();
					state.expiredPallets.clear();
			}			
        }

        // external transition
        void external_transition(TIME e, typename make_message_bags<input_ports>::type mbs)
		{ 
			state.currTime+=e;

            for(const auto &tPallet : get_messages<typename LoadPacking_defs::inPallet>(mbs))
			{
				state.currPallets.push_back(tPallet);
				sort(state.currPallets.begin(),state.currPallets.end());
           }
			
            for(const auto &tAircraftStatus : get_messages<typename LoadPacking_defs::inACStatus>(mbs))
			{
				if(tAircraftStatus.bStatus)
				{
					state.iAircraftAvaliable++;
				}else if(!tAircraftStatus.bStatus)
				{
					state.iAircraftAvaliable--;
				}
            }              

			auto myPallet=state.currPallets.begin();
			while(myPallet!=state.currPallets.end())
			{
				if( ConvertToInt(myPallet->tExpire) < (ConvertToInt(state.currTime)+LOAD_EXPIRE_BUFFER) )
				{
					state.expiredPallets.push_back((*myPallet));
					myPallet=state.currPallets.erase(myPallet);
				}else
				{
					++myPallet;
				}
			}
			
			if(state.currPallets.size()>=LOAD_AC_SIZE && state.iAircraftAvaliable>0)
			{
				state.currLoad.iId=state.iCurrLoadNum;
				state.iCurrLoadNum++;					
				state.currLoad.tExpire=state.currPallets[0].tExpire;
				state.currLoad.tFlight=TIME((rand()%(LOAD_FLIGHT_MAX-LOAD_FLIGHT_MIN)+LOAD_FLIGHT_MIN)*HOURS_IN_MIN);
				state.currPallets.erase(state.currPallets.begin(),state.currPallets.begin()+LOAD_AC_SIZE);
				state.sigma=TIME(0);
				state.currPhase=LOADREADY;
			}else
			{
				state.sigma=std::numeric_limits<TIME>::infinity();
 			}				
        }
		
        // confluence transition
        void confluence_transition(TIME e, typename make_message_bags<input_ports>::type mbs) {
            internal_transition();
            external_transition(TIME(), std::move(mbs));
        }

        // output function
        typename make_message_bags<output_ports>::type output() const
		{
            typename make_message_bags<output_ports>::type bags;

			if(state.currLoad.iId>0)
			{
				get_messages<typename LoadPacking_defs::outLoad>(bags).push_back(state.currLoad);
			}
			for(auto myExpiredPallet : state.expiredPallets)
			{
				get_messages<typename LoadPacking_defs::outExpired>(bags).push_back(myExpiredPallet);
			}
			return bags;			
        }

        // time_advance function
        TIME time_advance() const {  
             return state.sigma;
        }

        friend std::ostringstream& operator<<(std::ostringstream& os, const typename LoadPacking<TIME>::state_type& currState)
		{
			switch(currState.currPhase)
			{
				case WAITING:
					os << "Load Packing - Waiting\n" ; 
					break;
				case LOADREADY:
					os << "Load Packing - Busy: " << currState.currLoad.iId << " Flight Time: " << ConvertToHours(currState.currLoad.tFlight) << " Hours\n"; 
					break;
			}
			os << "Load Packing - Aircraft Avaliable: " << currState.iAircraftAvaliable  << "\n";
        return os;
        }
};     
#endif // LOADPACKING_HPP