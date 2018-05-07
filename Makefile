all: neuron

neuron: main.cpp thread.o
	g++ main.cpp thread.o -o neuron -lpthread

thread.o: thread.cpp thread.h
	g++ -c thread.cpp

clean:
	rm *.o neuron

run:
	./neuron
