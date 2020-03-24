#ifndef DESTINATION_HPP
#define DESTINATION_HPP

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
struct Destination_defs{
    struct inLoad : public in_port<oACLoad> { };
	struct outRecieved : public out_port<string> { };
};


template<typename TIME> class Destination{

	enum ePhase {WAITING, PROCESSING};

    public:
        // default constructor
        Destination() noexcept{
			state.currPhase=WAITING;
			state.sigma=std::numeric_limits<TIME>::infinity();
			state.currLoad=oACLoad();
        }
        
        // state definition
        struct state_type{
			ePhase currPhase;			
            TIME sigma;
			oACLoad currLoad;
        }; 
        state_type state;
		
        // ports definition
        using input_ports=std::tuple<typename Destination_defs::inLoad>;
        using output_ports=std::tuple<typename Destination_defs::outRecieved>;

        // internal transition
        void internal_transition()
		{
			switch(state.currPhase)
			{
				case PROCESSING:
					state.currPhase=WAITING;
					state.sigma=std::numeric_limits<TIME>::infinity();
					state.currLoad=oACLoad();
					break;
			}
        }

        // external transition
        void external_transition(TIME e, typename make_message_bags<input_ports>::type mbs)
		{ 
            for(const auto &tLoad : get_messages<typename Destination_defs::inLoad>(mbs))
			{
				state.currLoad=tLoad;
            }

			state.currPhase=PROCESSING;
			state.sigma=DEST_PROCESS_TIME;			
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

			if(state.currPhase==PROCESSING)
			{
				string sMessage="Recieved Shipment: " + to_string(state.currLoad.myLoad.iId) + " from Aircraft: " + to_string(state.currLoad.iACId) + "\n";
				get_messages<typename Destination_defs::outRecieved>(bags).push_back(sMessage);
			}
			
			return bags;			
        }

        // time_advance function
        TIME time_advance() const {  
             return state.sigma;
        }

        friend std::ostringstream& operator<<(std::ostringstream& os, const typename Destination<TIME>::state_type& currState) {
			switch(currState.currPhase)
			{
				case PROCESSING:
					os << "Destination: Processing Shipment " << currState.currLoad.myLoad.iId << " from Aircraft: " << currState.currLoad.iACId << "\n";					
					break;
				case WAITING:
					os << "Destination: Waiting ...\n";
					break;
			}
			
			return os;
        }
};     
#endif // DESTINATION_HPP