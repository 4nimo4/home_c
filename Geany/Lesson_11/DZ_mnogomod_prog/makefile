all: prog

prog: main.o sensor.o
	gcc -Wall -o prog main.o sensor.o

main.o: main.c
	gcc -Wall -c -o main.o main.c

func.o: sensor.c
	gcc -Wall -c -o sensor.o sensor.c

clean:
	rm -rf *.o prog