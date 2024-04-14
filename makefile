CPP = "g++.exe"
CXXFLAGS = \
	-Wall \
	-O2 \
	-std=c++14 \
	-I$(BOOSTDIR)\include\boost-1_82 \
	-I$(BOOSTDIR)\lib

OBJECTS = \
	obj/InkDatNmb.o

LIB = \
	-L$(BOOSTDIR)\lib

BOOSTLIB = \
	-lboost_filesystem-mgw13-mt-d-x64-1_82 \
	-lboost_system-mgw13-mt-d-x64-1_82

.PHONEY: V1 V2 clean

V1: obj/InkDatNmb.o
	$(CPP) $(CXXFLAGS) $^ -o InkDatNmb.exe -s $(LIB) $(BOOSTLIB)

V2: obj/InkDatNmbV2.o
	$(CPP) $(CXXFLAGS) $^ -o InkDatNmbV2.exe -s $(LIB) $(BOOSTLIB)

obj/InkDatNmb.o: InkDatNmb.cpp
	-if not exist obj mkdir obj
	$(CPP) $(CXXFLAGS) -c $^ -o $@

obj/InkDatNmbV2.o: InkDatNmbV2.cpp
	-if not exist obj mkdir obj
	$(CPP) $(CXXFLAGS) -c $^ -o $@

clean:
	-if exist InkDatNmb.exe del InkDatNmb.exe
	-if exist InkDatNmbV2.exe del InkDatNmbV2.exe
	-if exist obj\InkDatNmb.o del obj\InkDatNmb.o
	-if exist obj\InkDatNmbV2.o del obj\InkDatNmbV2.o
