CC=gcc
CFLAGS=-Wall

all: gen

gen: gen.c
	gcc $(CFLAGS) -o gen gen.c

create: gen
	./gen -m 10 -n 10 -o 10x10.csv
	./gen -m 18 -n 18 -o 18x18.csv
	./gen -m 34 -n 34 -o 34x34.csv
	./gen -m 66 -n 66 -o 66x66.csv
	./gen -m 130 -n 130 -o 130x130.csv
	./gen -m 514 -n 514 -o 514x514.csv
	./gen -m 2050 -n 2050 -o 2050x2050.csv
	./gen -m 8194 -n 8194 -o 8194x8194.csv

clean:
	rm -f gen
