#ifndef AIRCRAFT_HPP
#define AIRCRAFT_HPP

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
struct Aircraft_defs{
    struct inLoad : public in_port<oACLoad> { };
	struct outACStatus : public out_port<oAircraftStatus> { };
	struct outLoad : public out_port<oACLoad> { };
};

template<typename TIME> class Aircraft{

	enum ePhase {WAITING, TRANSPORTING, UNLOADING, RETURN, MAINT, SENDBUSYSTATUS, SENDREADYSTATUS};
	
    public:
        // default constructor
		Aircraft()= default;
		
        Aircraft(int tId) noexcept{
			state.currPhase=WAITING;
			state.sigma=std::numeric_limits<TIME>::infinity();
			state.currLoad=oACLoad();
			state.iTotalFlyingTime=0;
			state.iDeliveryTime=0;
			state.myACId=tId;
        }
        
        // state definition
        struct state_type{
			ePhase currPhase;			
            TIME sigma;
			oACLoad currLoad;
			int iTotalFlyingTime;
			int myACId;
			int iDeliveryTime;
        }; 
        state_type state;
		
        // ports definition
        using input_ports=std::tuple<typename Aircraft_defs::inLoad>;
        using output_ports=std::tuple<typename Aircraft_defs::outACStatus,typename Aircraft_defs::outLoad>;

        // internal transition
        void internal_transition()
		{
			switch(state.currPhase)
			{
				case SENDBUSYSTATUS:
					state.currPhase=TRANSPORTING;
					state.iTotalFlyingTime+=ConvertToInt(state.currLoad.myLoad.tFlight);
					state.iDeliveryTime+=ConvertToInt(state.currLoad.myLoad.tFlight);
					state.sigma=state.currLoad.myLoad.tFlight;	
					break;
				case TRANSPORTING:
					state.currPhase=UNLOADING;
					state.sigma=TIME(30);
					state.iDeliveryTime+=30;
					break;
				case UNLOADING:	
					state.currPhase=RETURN;
					state.sigma=state.currLoad.myLoad.tFlight;
					break;
				case RETURN:
					state.iTotalFlyingTime+=ConvertToInt(state.currLoad.myLoad.tFlight);
					state.currPhase=state.iTotalFlyingTime>AIRCRAFT_MAINT_LIMIT ? MAINT : SENDREADYSTATUS;
					state.iDeliveryTime=0;
					state.sigma=state.iTotalFlyingTime>AIRCRAFT_MAINT_LIMIT ? AIRCRAFT_MAINT_TIME*DAYS_IN_MIN : TIME(0);
					break;
				case MAINT:
					state.currPhase=SENDREADYSTATUS;
					state.sigma=TIME(0);
					state.iTotalFlyingTime=0;
					break;
				case SENDREADYSTATUS:
					state.currLoad=oACLoad();
					state.currPhase=WAITING;
					state.sigma=std::numeric_limits<TIME>::infinity();
					break;
					
			}
        }

        // external transition
        void external_transition(TIME e, typename make_message_bags<input_ports>::type mbs)
		{ 
            for(const auto &tLoad : get_messages<typename Aircraft_defs::inLoad>(mbs))
			{
				if(tLoad.iACId==state.myACId)
				{
					state.currLoad=tLoad;
					state.currPhase=SENDBUSYSTATUS;
					state.sigma=TIME(0);
				}else if(state.myACId==state.currLoad.iACId)
				{
					state.sigma=state.sigma-e;
				}else
				{
					state.currPhase=WAITING;
					state.sigma=std::numeric_limits<TIME>::infinity();
				}
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

			if(state.currPhase==SENDBUSYSTATUS)
			{
				get_messages<typename Aircraft_defs::outACStatus>(bags).push_back(oAircraftStatus(state.myACId,false));
			}
			
			if(state.currPhase==SENDREADYSTATUS)
			{
				get_messages<typename Aircraft_defs::outACStatus>(bags).push_back(oAircraftStatus(state.myACId,true));
			}
			
			if(state.currPhase==UNLOADING)
			{
				get_messages<typename Aircraft_defs::outLoad>(bags).push_back(state.currLoad);
			}
			
			return bags;			
        }

        // time_advance function
        TIME time_advance() const {  
             return state.sigma;
        }

        friend std::ostringstream& operator<<(std::ostringstream& os, const typename Aircraft<TIME>::state_type& currState) {
			switch(currState.currPhase)
			{
				case SENDBUSYSTATUS:
					os << "Aircraft: " << currState.myACId << " Taking Off ...\n";					
					break;
				case SENDREADYSTATUS:
					os << "Aircraft: " << currState.myACId << " Avaliable ...\n";					
					break;
				case TRANSPORTING:
					os << "Aircraft: " << currState.myACId << " Transporting ...\n";					
					break;
				case UNLOADING:
					os << "Aircraft: " << currState.myACId << " Unloading ...\n";					
					break;
				case MAINT:
					os << "Aircraft: " << currState.myACId << " in Maintenance ...\n";					
					break;
				case RETURN:
					os << "Aircraft: " << currState.myACId << " Completed Trip, Delivery Time: " <<  currState.iDeliveryTime << "\n";
					break;
				case WAITING:
					os << "Aircraft: " << currState.myACId << " Waiting, Total Flying Time: " << currState.iTotalFlyingTime << "\n";
					break;
			}
			
			return os;
        }
};     
#endif // AIRCRAFT_HPP