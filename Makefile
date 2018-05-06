all: neuron

neuron: main.cpp util.o
	g++ main.cpp util.o -o neuron

util.o: util.cpp util.h
	g++ -c util.cpp

clean:
	rm *.o neuron

run:
	./neuron
