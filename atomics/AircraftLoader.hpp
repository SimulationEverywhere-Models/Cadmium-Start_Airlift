#ifndef AIRCRAFTLOADER_HPP
#define AIRCRAFTLOADER_HPP

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
struct AircraftLoader_defs{
    struct outLoad : public out_port<oACLoad> { };
	struct inACStatus : public in_port<oAircraftStatus> { };
	struct inLoad : public in_port<oLoad> { };
};

template<typename TIME> class AircraftLoader{
	
	enum ePhase {WAITING, LOADINGAC};
	
    public:
        // default constructor
        AircraftLoader() noexcept{
			state.currPhase=WAITING;
			state.sigma=std::numeric_limits<TIME>::infinity();
			for(int iAddAC=1;iAddAC<=AIRCRAFT_NUM;iAddAC++)
			{
				state.myAircrafts.push_back(oAircraftStatus(iAddAC,true));
			}
			state.iAircraftAvaliable=0;
        }
        
        // state definition
        struct state_type{
			ePhase currPhase;			
            TIME sigma;
			int iAircraftAvaliable;
			vector<oAircraftStatus> myAircrafts;
			vector<oLoad> myWaitingLoads;
        }; 
        state_type state;
		
        // ports definition
        using input_ports=std::tuple<typename AircraftLoader_defs::inLoad, typename AircraftLoader_defs::inACStatus>;
        using output_ports=std::tuple<typename AircraftLoader_defs::outLoad>;

        // internal transition
        void internal_transition()
		{
			switch(state.currPhase)
			{
				case LOADINGAC:
					state.myWaitingLoads.erase(state.myWaitingLoads.begin(),state.myWaitingLoads.begin()+1);
					state.currPhase=WAITING;
					state.sigma=std::numeric_limits<TIME>::infinity();
			}

        }

        // external transition
        void external_transition(TIME e, typename make_message_bags<input_ports>::type mbs)
		{ 
            for(const auto &tLoad : get_messages<typename AircraftLoader_defs::inLoad>(mbs))
			{
				state.myWaitingLoads.push_back(tLoad);
            }

            for(const auto &tACStatus : get_messages<typename AircraftLoader_defs::inACStatus>(mbs))
			{
				state.myAircrafts[tACStatus.iACId-1].bStatus=tACStatus.bStatus;
            }

			state.iAircraftAvaliable=0;
			auto myAircraft=state.myAircrafts.begin();
			while(myAircraft!=state.myAircrafts.end() && state.iAircraftAvaliable==0)
			{
				if(myAircraft->bStatus && state.iAircraftAvaliable==0)
				{
					state.iAircraftAvaliable=myAircraft->iACId;
				}
				++myAircraft;
			}			

			if(state.myWaitingLoads.size()>0 && state.iAircraftAvaliable>0)
			{
				state.currPhase=LOADINGAC;
				state.sigma=TIME(0);
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

			if(state.currPhase==LOADINGAC)
			{
				get_messages<typename AircraftLoader_defs::outLoad>(bags).push_back(oACLoad(state.iAircraftAvaliable,state.myWaitingLoads[0]));
			}
			
			return bags;			
        }

        // time_advance function
        TIME time_advance() const {  
             return state.sigma;
        }

        friend std::ostringstream& operator<<(std::ostringstream& os, const typename AircraftLoader<TIME>::state_type& currState) 
		{
			switch(currState.currPhase)
			{
				case WAITING:
					os << "Aircraft Loader - Waiting\n" ; 
					break;
				case LOADINGAC:
					os << "Aircraft Loader - Busy: " << currState.iAircraftAvaliable << " Flight Time: " << ConvertToHours(currState.myWaitingLoads[0].tFlight) << " Hours\n"; 
					break;
			}
        return os;
        }
};     
#endif // AIRCRAFTLOADER_HPP