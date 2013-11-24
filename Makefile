CPP=g++
LIB=-lfftw3f -lportaudio `fltk-config --ldflags`
CPPFLAGS=-c -g -std=c++11 -I./include/

all: audio.o BaseBlock.o Cartesian.o Plotter.o safecall.o main.o
	cd obj
	$(CPP) -g -std=c++11 obj/audio.o obj/BaseBlock.o obj/Cartesian.o obj/Plotter.o obj/safecall.o obj/main.o $(LIB)

audio.o:
	$(CPP) $(CPPFLAGS) -oobj/audio.o src/audio.cpp

BaseBlock.o:
	$(CPP) $(CPPFLAGS) -oobj/BaseBlock.o src/BaseBlock.cpp

Cartesian.o:
	$(CPP) $(CPPFLAGS) -oobj/Cartesian.o src/Cartesian.cpp

Plotter.o:
	$(CPP) $(CPPFLAGS) -oobj/Plotter.o src/Plotter.cpp

safecall.o:
	$(CPP) $(CPPFLAGS) -oobj/safecall.o src/safecall.cpp

main.o:
	$(CPP) $(CPPFLAGS) -oobj/main.o src/main.cpp
