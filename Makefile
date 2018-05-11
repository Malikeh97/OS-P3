all: neuron

neuron: parallel.cpp
	g++ parallel.cpp -o neuron -lpthread

clean:
	rm *.o neuron

run:
	./neuron
