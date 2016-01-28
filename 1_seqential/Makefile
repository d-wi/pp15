CC=gcc
CFLAGS=-Wall

all: seq

seq: seq.c
	gcc $(CFLAGS) -o seq seq.c -lrt

8x8: 10x10
10x10: seq
	./seq -m 10 -n 10 -i ../input/10x10.csv -o ../output/10x10_output.txt

16x16: 18x18
18x18: seq
	./seq -m 18 -n 18 -i ../input/18x18.csv -o ../output/18x18_output.txt

32x32: 34x34
34x34: seq
	./seq -m 34 -n 34 -i ../input/34x34.csv -o ../output/34x34_output.txt

64x64: 66x66
66x66: seq
	./seq -m 66 -n 66 -i ../input/66x66.csv -o ../output/66x66_output.txt

128x128: 130x130
130x130: seq
	./seq -m 130 -n 130 -i ../input/130x130.csv -o ../output/130x130_output.txt

512x512: 514x514
514x514: seq
	./seq -m 514 -n 514 -i ../input/514x514.csv -o ../output/514x514_output.txt

2048x2048: 2050x2050
2050x2050: seq
	./seq -m 2050 -n 2050 -i ../input/2050x2050.csv -o ../output/2050x2050_output.txt

8192x8192: 8194x8194
8194x8194: seq
	./seq -m 8194 -n 8194 -i ../input/8194x8194.csv -o ../output/8194x8194_output.txt

clean:
	rm -f seq
