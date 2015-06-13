CC=g++
STD=c++11
CVFLAGS=$(shell pkg-config --cflags --libs opencv)
FLAGS= 
CLINE = $(CC) -std=$(STD) $(CVFLAGS) $(FLAGS)

%.o: %.cpp %.hpp
	$(CLINE) -c $< -o $@

compile: Car.o TFlow.o
	$(CLINE) Car.o TFlow.o app.cpp -o flow.out
	
clean:
	rm *.o
	rm *.out
