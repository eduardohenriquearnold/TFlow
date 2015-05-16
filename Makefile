CC=g++
STD=c++11
CVFLAGS=$(shell pkg-config --cflags --libs opencv)
FLAGS= 
CLINE = $(CC) -std=$(STD) $(CVFLAGS) $(FLAGS)

%.o: %.cpp %.hpp
	$(CLINE) -c $< -o $@

compile: TFlow.o
	$(CLINE) TFlow.o app.cpp -o flow.out
	
clean:
	rm *.o
	rm *.out
