all: neuron

neuron: main.cpp
	g++ main.cpp -o neuron -lpthread

clean:
	rm *.o neuron

run:
	./neuron
