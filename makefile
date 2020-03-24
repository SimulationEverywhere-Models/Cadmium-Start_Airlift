CC=g++
CFLAGS=-std=c++17

INCLUDECADMIUM=-I ../../cadmium/include
INCLUDEDESTIMES=-I ../../DESTimes/include

#CREATE BIN AND BUILD FOLDERS TO SAVE THE COMPILED FILES DURING RUNTIME
bin_folder := $(shell mkdir -p bin)
build_folder := $(shell mkdir -p build)
results_folder := $(shell mkdir -p simulation_results)

#TARGET TO COMPILE ALL THE TESTS TOGETHER (NOT SIMULATOR)
# === COMMON ===
message.o: data_structures/message.cpp
	$(CC) -g -c $(CFLAGS) $(INCLUDECADMIUM) $(INCLUDEDESTIMES) data_structures/message.cpp -o build/message.o

StratAirliftUtils.o: usrlib/StratAirliftUtils.cpp
	$(CC) -g -c $(CFLAGS) $(INCLUDECADMIUM) $(INCLUDEDESTIMES) usrlib/StratAirliftUtils.cpp -o build/StratAirliftUtils.o

# === TESTS ===
PalletGen_Test.o: test/PalletGen_Test.cpp
	$(CC) -g -c $(CFLAGS) $(INCLUDECADMIUM) $(INCLUDEDESTIMES) test/PalletGen_Test.cpp -o build/PalletGen_Test.o

LoadPacking_Test.o: test/LoadPacking_Test.cpp
	$(CC) -g -c $(CFLAGS) $(INCLUDECADMIUM) $(INCLUDEDESTIMES) test/LoadPacking_Test.cpp -o build/LoadPacking_Test.o

AircraftLoader_Test.o: test/AircraftLoader_Test.cpp
	$(CC) -g -c $(CFLAGS) $(INCLUDECADMIUM) $(INCLUDEDESTIMES) test/AircraftLoader_Test.cpp -o build/AircraftLoader_Test.o

Aircraft_Test.o: test/Aircraft_Test.cpp
	$(CC) -g -c $(CFLAGS) $(INCLUDECADMIUM) $(INCLUDEDESTIMES) test/Aircraft_Test.cpp -o build/Aircraft_Test.o

Destination_Test.o: test/Destination_Test.cpp
	$(CC) -g -c $(CFLAGS) $(INCLUDECADMIUM) $(INCLUDEDESTIMES) test/Destination_Test.cpp -o build/Destination_Test.o

LoadingProcess_Test.o: test/LoadingProcess_Test.cpp
	$(CC) -g -c $(CFLAGS) $(INCLUDECADMIUM) $(INCLUDEDESTIMES) test/LoadingProcess_Test.cpp -o build/LoadingProcess_Test.o

tests: PalletGen_Test.o LoadPacking_Test.o AircraftLoader_Test.o Aircraft_Test.o Destination_Test.o LoadingProcess_Test.o message.o StratAirliftUtils.o
	$(CC) -g -o bin/PalletGen_Test build/PalletGen_Test.o build/message.o build/StratAirliftUtils.o
	$(CC) -g -o bin/LoadPacking_Test build/LoadPacking_Test.o build/message.o build/StratAirliftUtils.o
	$(CC) -g -o bin/AircraftLoader_Test build/AircraftLoader_Test.o build/message.o build/StratAirliftUtils.o
	$(CC) -g -o bin/Aircraft_Test build/Aircraft_Test.o build/message.o build/StratAirliftUtils.o
	$(CC) -g -o bin/Destination_Test build/Destination_Test.o build/message.o build/StratAirliftUtils.o
	$(CC) -g -o bin/LoadingProcess_Test build/LoadingProcess_Test.o build/message.o build/StratAirliftUtils.o

# === DEVELOPMENT ===
#dev: LoadingProcess_Test.o message.o StratAirliftUtils.o
#	$(CC) -g -o bin/LoadingProcess_Test build/LoadingProcess_Test.o build/message.o build/StratAirliftUtils.o

# === PRIMARY MODEL ===
StratAirlift.o: models/StratAirlift.cpp
	$(CC) -g -c $(CFLAGS) $(INCLUDECADMIUM) $(INCLUDEDESTIMES) models/StratAirlift.cpp -o build/StratAirlift.o

#TARGET TO COMPILE ONLY SIMULATOR
simulator: StratAirlift.o message.o StratAirliftUtils.o
	$(CC) -g -o bin/StratAirlift build/StratAirlift.o build/message.o build/StratAirliftUtils.o
	
#TARGET TO COMPILE EVERYTHING (SIMULATOR + TESTS TOGETHER)
all: simulator tests

#CLEAN COMMANDS
clean: 
	rm -f bin/* build/*
	
#RUN EXPERIMENTS
runexperiments:
	./bin/StratAirlift.exe StratAirlift_E1A ./input_data/StratAirlift_E1A.txt
	./bin/StratAirlift.exe StratAirlift_E1B ./input_data/StratAirlift_E1B.txt
	./bin/StratAirlift.exe StratAirlift_E1C ./input_data/StratAirlift_E1C.txt
	./bin/StratAirlift.exe StratAirlift_E2A ./input_data/StratAirlift_E2A.txt
	./bin/StratAirlift.exe StratAirlift_E2B ./input_data/StratAirlift_E2B.txt
	./bin/StratAirlift.exe StratAirlift_E2C ./input_data/StratAirlift_E2C.txt

#RUN ALL TESTS
runalltests:
	cd bin; ./PalletGen_Test.exe; ./LoadPacking_Test.exe; ./AircraftLoader_Test.exe; ./Aircraft_Test.exe; ./Destination_Test.exe; ./LoadingProcess_Test.exe; cd ..;