all: prog

prog: main.o func.o
	gcc -Wall -o prog main.o func.o

main.o: main.c mylibrary.h
	gcc -Wall -c -o main.o main.c

func.o: func.c mylibrary.h lib2.h
	gcc -Wall -c -o func.o func.c

clean:
	rm -rf *.o prog