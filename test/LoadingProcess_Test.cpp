#ifndef STRATAIRLIFTUTILS_CPP
#define STRATAIRLIFTUTILS_CPP

//Cadmium Simulator headers
#include <cadmium/modeling/ports.hpp>
#include <cadmium/modeling/dynamic_model.hpp>
#include <cadmium/modeling/dynamic_model_translator.hpp>
#include <cadmium/engine/pdevs_dynamic_runner.hpp>
#include <cadmium/logger/common_loggers.hpp>

//Time class header
#include "../usrlib/EIRational.hpp"

//Messages structures
#include "../data_structures/message.hpp"

//Atomic model headers
#include <cadmium/basic_model/pdevs/iestream.hpp> //Atomic model for inputs
#include "../atomics/LoadPacking.hpp"
#include "../atomics/AircraftLoader.hpp"
#include "../atomics/Aircraft.hpp"

//C++ headers
#include <iostream>
#include <chrono>
#include <algorithm>
#include <string>


using namespace std;
using namespace cadmium;
using namespace cadmium::basic_models::pdevs;

using TIME = EIRational;

/***** Define input port for coupled models *****/
// Loading Process
struct LP_InputPallet : public in_port<oPallet>{};
// Strategic Airlift = N/A

/***** Define output ports for coupled model *****/
// Loading Process
struct LP_ExpiredPallets : public out_port<oPallet>{};
struct LP_ACLoad : public out_port<oACLoad>{};
// Strategic Airlift = N/A
struct SA_ExpiredPallets : public out_port<oPallet>{};
struct SA_ACLoad : public out_port<oACLoad>{};

/****** Input Reader atomic model declaration *******************/
template<typename T>
class InputReader_oPallet : public iestream_input<oPallet,T> {
    public:
        InputReader_oPallet () = default;
        InputReader_oPallet (const char* file_path) : iestream_input<oPallet,T>(file_path) {}
};


int main(int argc, char *argv[])
{
	if(argc==2)
	{
		ReadOptions(argv[1]);
	}
	PerformSetup();

	string myLoadInFile = INPUT_DIR + "LoadingProcessTest_Pallets.txt";
    const char * i_input_data_pallets = myLoadInFile.c_str();
    shared_ptr<dynamic::modeling::model> input_reader_getpallets = dynamic::translate::make_dynamic_atomic_model<InputReader_oPallet , TIME, const char* >("input_reader_getpallets" , move(i_input_data_pallets));

	/****** Atomic Model Instantiation ******/
    shared_ptr<dynamic::modeling::model> myLoadPacking = dynamic::translate::make_dynamic_atomic_model<LoadPacking, TIME>("myLoadPacking");
    shared_ptr<dynamic::modeling::model> myAircraftLoad = dynamic::translate::make_dynamic_atomic_model<AircraftLoader, TIME>("myAircraftLoad");
    shared_ptr<dynamic::modeling::model> myAircraft1 = dynamic::translate::make_dynamic_atomic_model<Aircraft,TIME, int>("myAircraft1",1);
    shared_ptr<dynamic::modeling::model> myAircraft2 = dynamic::translate::make_dynamic_atomic_model<Aircraft,TIME, int>("myAircraft2",2);

    /****** Loading Process (LP) Coupled Model ******/
    dynamic::modeling::Ports iports_LP = {typeid(LP_InputPallet)};
    dynamic::modeling::Ports oports_LP = {typeid(LP_ExpiredPallets),typeid(LP_ACLoad)};
    dynamic::modeling::Models submodels_LP = {myLoadPacking, myAircraftLoad, myAircraft1, myAircraft2};
    dynamic::modeling::EICs eics_LP = {
        dynamic::translate::make_EIC<LP_InputPallet, LoadPacking_defs::inPallet>("myLoadPacking")
    };
    dynamic::modeling::EOCs eocs_LP = {
        dynamic::translate::make_EOC<LoadPacking_defs::outExpired,LP_ExpiredPallets>("myLoadPacking"),
        dynamic::translate::make_EOC<Aircraft_defs::outLoad,LP_ACLoad>("myAircraft1"),
        dynamic::translate::make_EOC<Aircraft_defs::outLoad,LP_ACLoad>("myAircraft2")
    };
    dynamic::modeling::ICs ics_LP = {
        dynamic::translate::make_IC<LoadPacking_defs::outLoad, AircraftLoader_defs::inLoad>("myLoadPacking","myAircraftLoad"),
        dynamic::translate::make_IC<AircraftLoader_defs::outLoad, Aircraft_defs::inLoad>("myAircraftLoad","myAircraft1"),
        dynamic::translate::make_IC<AircraftLoader_defs::outLoad, Aircraft_defs::inLoad>("myAircraftLoad","myAircraft2"),
        dynamic::translate::make_IC<Aircraft_defs::outACStatus, LoadPacking_defs::inACStatus>("myAircraft1","myLoadPacking"),
        dynamic::translate::make_IC<Aircraft_defs::outACStatus, LoadPacking_defs::inACStatus>("myAircraft2","myLoadPacking"),
        dynamic::translate::make_IC<Aircraft_defs::outACStatus, AircraftLoader_defs::inACStatus>("myAircraft1","myAircraftLoad"),
        dynamic::translate::make_IC<Aircraft_defs::outACStatus, AircraftLoader_defs::inACStatus>("myAircraft2","myAircraftLoad")
	};
    shared_ptr<dynamic::modeling::coupled<TIME>> LP_MODEL;
    LP_MODEL = make_shared<dynamic::modeling::coupled<TIME>>(
        "LP_Model", submodels_LP, iports_LP, oports_LP, eics_LP, eocs_LP, ics_LP 
    );

    /****** Strategic Airlift (SA) Coupled Model ******/
    dynamic::modeling::Ports iports_TOP = {};
    dynamic::modeling::Ports oports_TOP = {typeid(SA_ExpiredPallets),typeid(SA_ACLoad)};
    dynamic::modeling::Models submodels_TOP = {input_reader_getpallets, LP_MODEL};
    dynamic::modeling::EICs eics_TOP = {};
    dynamic::modeling::EOCs eocs_TOP = {
        dynamic::translate::make_EOC<LP_ExpiredPallets,SA_ExpiredPallets>("LP_Model"),
        dynamic::translate::make_EOC<LP_ACLoad,SA_ACLoad>("LP_Model")
    };
    dynamic::modeling::ICs ics_TOP = {
        dynamic::translate::make_IC<iestream_input_defs<oPallet>::out, LP_InputPallet>("input_reader_getpallets","LP_Model")
    };
    shared_ptr<cadmium::dynamic::modeling::coupled<TIME>> TOP;
    TOP = make_shared<dynamic::modeling::coupled<TIME>>(
        "TOP", submodels_TOP, iports_TOP, oports_TOP, eics_TOP, eocs_TOP, ics_TOP 
    );

    /*************** Loggers *******************/
    static ofstream out_messages(LOG_DIR+"LoadingProcess_output_messages.txt");
    struct oss_sink_messages{
        static ostream& sink(){          
            return out_messages;
        }
    };
    static ofstream out_state(LOG_DIR+"LoadingProcess_output_state.txt");
    struct oss_sink_state{
        static ostream& sink(){          
            return out_state;
        }
    };
    
    using state=logger::logger<logger::logger_state, dynamic::logger::formatter<TIME>, oss_sink_state>;
    using log_messages=logger::logger<logger::logger_messages, dynamic::logger::formatter<TIME>, oss_sink_messages>;
    using global_time_mes=logger::logger<logger::logger_global_time, dynamic::logger::formatter<TIME>, oss_sink_messages>;
    using global_time_sta=logger::logger<logger::logger_global_time, dynamic::logger::formatter<TIME>, oss_sink_state>;

    using logger_top=logger::multilogger<state, log_messages, global_time_mes, global_time_sta>;

    /************** Runner call ************************/ 
    dynamic::engine::runner<TIME, logger_top> r(TOP, {0});
    r.run_until(TIME(SCENARIO_STOP));
	
    return 0;
}

#endif // STRATAIRLIFTUTILS_CPP