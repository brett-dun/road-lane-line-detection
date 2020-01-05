# # Makefile

# executable : main.o
# # exectable depends on main.o

# #how it is made (remember the tab)
# 	g++ main.o -o exectable

# # main.o doesn't exist? make it
# main.o : main.cpp
# 	g++ -c main.cpp

CFLAGS = `pkg-config --cflags opencv`
LIBS = `pkg-config --libs opencv`

executable : main.cpp linearRegression.cpp linearRegression.hpp laneLines.cpp laneLines.hpp

#	g++ -std=c++0x linearRegression.cpp main.cpp -o executable `pkg-config --cflags --libs opencv` 
	g++ -std=c++0x $(CFLAGS) linearRegression.cpp laneLines.cpp main.cpp -o executable $(LIBS)