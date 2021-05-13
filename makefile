# Makefile for Logger unit tests.
objects  = TrackSort.o

headers  = TextFile.h

options = -std=c++20

TrackSort:	$(objects)	$(headers)
	g++ $(options) -o TrackSort $(objects)

%.o:	%.cpp	$(headers)
	g++ $(options) -c -o $@ $<

format:
	tfc -s -u -r TrackSort.cpp
	tfc -s -u -r TextFile.h

clean:
	rm -f *.exe *.o