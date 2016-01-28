CC=gcc
CFLAGS=-Wall

all: openmpTest

openmpPi: openmpPi.c
	gcc -o openmpPi -fopenmp openmpPi.c

openmpTest: openmpTest.c
	gcc -o openmpTest -fopenmp openmpTest.c

openmp: openmp.c
	gcc $(CFLAGS) -o openmp -fopenmp openmp.c

clean:
	rm -f openmp openmpTest openmpPi
