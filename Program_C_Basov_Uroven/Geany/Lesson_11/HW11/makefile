all: prog

prog: main.o temp_api.o
	gcc -Wall -o prog main.o temp_api.o

main.o: main.c temp_api.h
	gcc -Wall -c -o main.o main.c

temp_api.o: temp_api.c temp_api.h
	gcc -Wall -c -o temp_api.o temp_api.c

clean:
	rm -rf *.o prog