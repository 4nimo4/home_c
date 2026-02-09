all: prog

prog: report.o temp_function.o
	gcc -Wall -g -o prog report.o temp_function.o

report.o: report.c temp_function.h
	gcc -Wall -g -c -o report.o report.c

temp_function.o: temp_function.c temp_function.h
	gcc -Wall -g -c -o temp_function.o temp_function.c

clean:
	rm -rf *.o prog