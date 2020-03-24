#ifndef __PALLET_GEN_HPP__
#define __PALLET_GEN__

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
struct PalletGen_defs{
	struct outPallet : public out_port<oPallet> { };
};

template<typename TIME> class PalletGenerator{

	enum ePhase {WAITING, BUSY};								

    public:
        // default constructor
        PalletGenerator() noexcept{
			state.currPallet=oPallet();
			state.currPhase=WAITING;
			state.sigma=TIME(0);
			state.currTime=TIME(0);
			state.iCurrPalletNum=1;
        }
        
        // state definition
        struct state_type{
			int iCurrPalletNum;
			oPallet currPallet;
			ePhase currPhase;			
            TIME sigma;
			TIME currTime;
        }; 
        state_type state;
		
        // ports definition
        using input_ports=std::tuple<>;
        using output_ports=std::tuple<typename PalletGen_defs::outPallet>;

        // internal transition
        void internal_transition()
		{
			switch(state.currPhase)
			{
				case WAITING:
					state.currPhase=BUSY;
					state.currPallet=oPallet(state.iCurrPalletNum,state.currTime+TIME((rand() % PALLET_GEN_MAX_EXPIRE +1)*DAYS_IN_MIN));
					state.iCurrPalletNum++;
					state.sigma=TIME((rand() % (PALLET_GEN_MAX_NEW-PALLET_GEN_MIN_NEW) + PALLET_GEN_MIN_NEW)*HOURS_IN_MIN);
					state.currTime+=state.sigma;
					break;
				case BUSY:
					state.currPhase=WAITING;
					state.currPallet=oPallet();
					state.sigma=TIME(0);
					break;
			}			
        }

        // external transition
        void external_transition(TIME e, typename make_message_bags<input_ports>::type mbs)
		{ 
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

			if(state.currPallet.iId>0)
			{
				get_messages<typename PalletGen_defs::outPallet>(bags).push_back(state.currPallet);
			}
			
			return bags;			
        }

        // time_advance function
        TIME time_advance() const {  
             return state.sigma;
        }

        friend std::ostringstream& operator<<(std::ostringstream& os, const typename PalletGenerator<TIME>::state_type& currState)
		{
			switch(currState.currPhase)
			{
				case WAITING:
					os << "Pallet Generator - Waiting\n" ; 
					break;
				case BUSY:
					os << "Pallet Generator - Busy: " << currState.currPallet.iId << " Expiration Time: " << ConvertToDays(currState.currPallet.tExpire) << " Days\n"; 
					break;
			}			
        return os;
        }
};     
#endif // __PALLET_GEN__