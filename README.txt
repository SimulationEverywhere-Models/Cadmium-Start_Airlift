This folder contains the Strategic Airlift DEVS model implemented in Cadmium

/**************************/
/****FILES ORGANIZATION****/
/**************************/

README.txt	
StrategicAirlift.docx
StratAirlift.xml
makefile

atomics [This folder contains atomic models implemented in Cadmium]
	\atomics\Aircraft.hpp
	\atomics\AircraftLoader.hpp
	\atomics\Destination.hpp
	\atomics\LoadPacking.hpp
	\atomics\PalletGenerator.hpp
bin [Where the executables are stored after make.  It will be auto created]
build [Auto created for the make build]
data_structures [Used to define the messages to be passed in the model]
	\data_structures\message.cpp
	\data_structures\message.hpp
input_data [Used to run the tests and experiments]
	\input_data\AircraftLoaderTest_AC.txt
	\input_data\AircraftLoaderTest_Load.txt
	\input_data\AircraftLoaderTest_Options.txt
	\input_data\AircraftTest_Load.txt
	\input_data\AircraftTest_Options.txt
	\input_data\DestinationTest_Load.txt
	\input_data\DestinationTest_Options.txt
	\input_data\LoadingProcessTest_Options.txt
	\input_data\LoadingProcessTest_Pallets.txt
	\input_data\LoadPackingTest_AC.txt
	\input_data\LoadPackingTest_Options.txt
	\input_data\LoadPackingTest_Pallet.txt
	\input_data\PalletGenTest_Options.txt
	\input_data\StratAirlift_Options.txt
models [The model]
	\models\StratAirlift.cpp
simulation_results [Auto created to store the simulation and test results]
test [The atomic and coupled tests]
	\test\AircraftLoader_Test.cpp
	\test\Aircraft_Test.cpp
	\test\Destination_Test.cpp
	\test\LoadingProcess_Test.cpp
	\test\LoadPacking_Test.cpp
	\test\PalletGen_Test.cpp
usrlib [Functions to support the simulation and tests]
	\usrlib\EIRational.hpp
	\usrlib\StratAirliftUtils.cpp
	\usrlib\StratAirliftUtils.hpp

/*************/
/****STEPS****/
/*************/

0 - StrategicAirliftModel.docx contains the explanation of this model.

1 - Update include path in the makefile in this folder and subfolders. You need to update the following lines:
	INCLUDECADMIUM=-I ../../cadmium/include
	INCLUDEDESTIMES=-I ../../DESTimes/include
    Update the relative path to cadmium/include from the folder where the makefile is. You need to take into account where you copied the folder during the installation process
	Example: INCLUDECADMIUM=-I ../../cadmium/include
	Do the same for the DESTimes library
	
2 - Compile the project and the tests
	1 - Open the terminal (Ubuntu terminal for Linux and Cygwin for Windows) in the StratAirflift folder
	3 - To compile the project and the tests, type in the terminal:
			make clean; make all

3 - Run tests:
	1 - To execute all tests you can use "make runalltests".
	2 - If you would like to excute tests individually please use the steps below:
		1 - Open the terminal in the bin folder. 
		2 - To run the test, type in the terminal "./NAME_OF_THE_COMPILED_FILE" (For windows, "./NAME_OF_THE_COMPILED_FILE.exe").  Each test has an optional ability to specify an options file as the first command line parameter.  This allows the user to redirect the output files and change parameters if required.  The test execulables in the bin directory are:
			a.	PalletGen_Test.exe
			b.	LoadPacking_Test.exe
			c.	AircraftLoader_Test.exe
			d.	Aircraft_Test.exe
			e.	Destination_Test.exe
			f.	LoadingProcess_Test.exe
		3 - To check the output of the test, go to the folder simulation_results and open  the appropriate output_state and output_messages file (i.e. PalletGen_Test_output_messages.txt).

4 - Run the top model
	1 - To execute all documented experiments at once you can use "make runexperiments"
	2 - If you would like to excute your own experiments please use the steps below:
		1 - Open the terminal in the bin folder. 
		2 - To run the model, type in the terminal "./StratAirlift {Log file prefix name}" (For windows, "./StratAirlift.exe {Log file prefix name}").  To change the parameters of the model you can include an options file as a parameter.  An example file is included in the input_data folder and labelled StratAirlift_Options.txt.  You would run the options file with "./StratAirlift StratAirlift_Test ../input_data/StratAirlift_Options.txt".
		3 - To check the output of the model, go to the folder simulation_results and open  {Log file prefix name}_state.txt and {Log file prefix name}_messages.txt file.

