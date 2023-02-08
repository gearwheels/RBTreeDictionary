CC = gcc
FLAGS = -Wall -g -pg -Werror -Wno-sign-compare -Wno-unused-result
FILES = solution.c
PROG = solution

all:
	$(CC) $(FLAGS) -o $(PROG) $(FILES)

clean:
	rm -f *.o run
