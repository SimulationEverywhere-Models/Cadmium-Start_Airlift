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
#include "../atomics/Aircraft.hpp"

//C++ libraries
#include <iostream>
#include <string>

using namespace std;
using namespace cadmium;
using namespace cadmium::basic_models::pdevs;

using TIME = EIRational;

/***** Define input port for coupled models *****/
struct inLoad : public cadmium::in_port<oACLoad>{};

/***** Define output ports for coupled model *****/
struct outLoad : public cadmium::out_port<oACLoad>{};
struct outACStatus : public cadmium::out_port<oAircraftStatus>{};

/****** Input Reader atomic model declaration *******************/
template<typename T>
class InputReader_oLoad : public iestream_input<oACLoad,T> {
    public:
        InputReader_oLoad () = default;
        InputReader_oLoad (const char* file_path) : iestream_input<oACLoad,T>(file_path) {}
};

int main(int argc, char *argv[])
{
	if(argc==2)
	{
		ReadOptions(argv[1]);
	}
	PerformSetup();
	
    /****** Input Reader atomic models instantiation *******************/
	string myLoadInFile = INPUT_DIR + "AircraftTest_Load.txt";
    const char * i_input_data_getload = myLoadInFile.c_str();
    shared_ptr<dynamic::modeling::model> input_reader_getload = dynamic::translate::make_dynamic_atomic_model<InputReader_oLoad , TIME, const char* >("input_reader_getload" , move(i_input_data_getload));

    /****** Atomic model instantiation *******************/
    shared_ptr<dynamic::modeling::model> myAircraft = dynamic::translate::make_dynamic_atomic_model<Aircraft,TIME, int>("myAircraft",1);

    /*******TOP MODEL********/
    dynamic::modeling::Ports iports_TOP = {};
    dynamic::modeling::Ports oports_TOP = {typeid(outLoad),typeid(outACStatus)};
    dynamic::modeling::Models submodels_TOP = {input_reader_getload,myAircraft};
    dynamic::modeling::EICs eics_TOP = {};
    dynamic::modeling::EOCs eocs_TOP = {
        dynamic::translate::make_EOC<Aircraft_defs::outLoad,outLoad>("myAircraft"),
        dynamic::translate::make_EOC<Aircraft_defs::outACStatus,outACStatus>("myAircraft")
    };

    dynamic::modeling::ICs ics_TOP = {
        dynamic::translate::make_IC<iestream_input_defs<oACLoad>::out,Aircraft_defs::inLoad>("input_reader_getload","myAircraft")
    };
	
    shared_ptr<dynamic::modeling::coupled<TIME>> TOP;

    TOP = make_shared<dynamic::modeling::coupled<TIME>>(
        "TOP", submodels_TOP, iports_TOP, oports_TOP, eics_TOP, eocs_TOP, ics_TOP
    );

    /*************** Loggers *******************/
    static ofstream out_messages(LOG_DIR+"Aircraft_Test_output_messages.txt");
    struct oss_sink_messages{
        static ostream& sink(){          
            return out_messages;
        }
    };
	
    static ofstream out_state(LOG_DIR+"Aircraft_Test_output_state.txt");
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
