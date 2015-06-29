ALL: t console
t : test.cpp GeoTuple.h 
	g++ -Wall -std=c++11 -o t test.cpp

console : GeoTuple.h console.cpp
	g++ -Wall -std=c++11 -o console console.cpp
