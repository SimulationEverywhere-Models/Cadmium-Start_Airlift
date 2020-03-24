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

//C++ libraries
#include <iostream>
#include <string>

using namespace std;
using namespace cadmium;
using namespace cadmium::basic_models::pdevs;

using TIME = EIRational;

/***** Define input port for coupled models *****/
struct inPallet : public cadmium::in_port<oPallet>{};
struct inACStatus : public cadmium::in_port<oAircraftStatus>{};

/***** Define output ports for coupled model *****/
struct outExpiredPallet : public cadmium::out_port<oPallet>{};
struct outLoad : public cadmium::out_port<oLoad>{};

/****** Input Reader atomic model declaration *******************/
template<typename T>
class InputReader_oPallet : public iestream_input<oPallet,T> {
    public:
        InputReader_oPallet () = default;
        InputReader_oPallet (const char* file_path) : iestream_input<oPallet,T>(file_path) {}
};

template<typename T>
class InputReader_ACStatus : public iestream_input<oAircraftStatus,T> {
    public:
        InputReader_ACStatus () = default;
        InputReader_ACStatus (const char* file_path) : iestream_input<oAircraftStatus,T>(file_path) {}
};


int main(int argc, char *argv[])
{
	if(argc==2)
	{
		ReadOptions(argv[1]);
	}
	PerformSetup();
	
    /****** Input Reader atomic models instantiation *******************/
	string myPalletInFile = INPUT_DIR + "LoadPackingTest_Pallet.txt";
    const char * i_input_data_genpallet = myPalletInFile.c_str();
    shared_ptr<dynamic::modeling::model> input_reader_genpallet = dynamic::translate::make_dynamic_atomic_model<InputReader_oPallet , TIME, const char* >("input_reader_genpallet" , move(i_input_data_genpallet));

	string myACInFile = INPUT_DIR + "LoadPackingTest_AC.txt";
    const char * i_input_data_acstatus = myACInFile.c_str();
    shared_ptr<dynamic::modeling::model> input_reader_acstatus = dynamic::translate::make_dynamic_atomic_model<InputReader_ACStatus , TIME, const char* >("input_reader_acstatus" , move(i_input_data_acstatus));	
	
    /****** Pallet Generator atomic model instantiation *******************/
    shared_ptr<dynamic::modeling::model> myLoadPacking = dynamic::translate::make_dynamic_atomic_model<LoadPacking, TIME>("myLoadPacking");
    /*******TOP MODEL********/
    dynamic::modeling::Ports iports_TOP = {};
    dynamic::modeling::Ports oports_TOP = {typeid(outExpiredPallet),typeid(outLoad)};
    dynamic::modeling::Models submodels_TOP = {input_reader_genpallet,input_reader_acstatus,myLoadPacking};
    dynamic::modeling::EICs eics_TOP = {};
    dynamic::modeling::EOCs eocs_TOP = {
        dynamic::translate::make_EOC<LoadPacking_defs::outExpired,outExpiredPallet>("myLoadPacking"),
        dynamic::translate::make_EOC<LoadPacking_defs::outLoad,outLoad>("myLoadPacking")
    };
    dynamic::modeling::ICs ics_TOP = {
        dynamic::translate::make_IC<iestream_input_defs<oPallet>::out,LoadPacking_defs::inPallet>("input_reader_genpallet","myLoadPacking"),
        dynamic::translate::make_IC<iestream_input_defs<oAircraftStatus>::out,LoadPacking_defs::inACStatus>("input_reader_acstatus","myLoadPacking")
    };
	
    shared_ptr<dynamic::modeling::coupled<TIME>> TOP;
    TOP = make_shared<dynamic::modeling::coupled<TIME>>(
        "TOP", submodels_TOP, iports_TOP, oports_TOP, eics_TOP, eocs_TOP, ics_TOP
    );

    /*************** Loggers *******************/
    static ofstream out_messages(LOG_DIR+"LoadPacking_Test_output_messages.txt");
    struct oss_sink_messages{
        static ostream& sink(){          
            return out_messages;
        }
    };
	
    static ofstream out_state(LOG_DIR+"LoadPacking_Test_output_state.txt");
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
