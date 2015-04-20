COMPILER = gcc
CCFLAGS = -ansi -pedantic -Wall

wordstat: wordstat.h wordstat.c
	$(COMPILER) $(CCFLAGS) -o wordstat wordstat.c

clean:
	rm -f wordstat
	rm -f *.o
