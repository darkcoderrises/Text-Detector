C = g++
CFLAGS = -std=c++11 -Wall -O3 -g
PROG = main

SRCS = main.cpp

all: $(PROG)

$(PROG):        $(SRCS)
	$(CC) $(CFLAGS) $(LIBS) -c *.cpp
	$(CC) -o $(PROG) *.o $(LIBS)

clear:
	rm -f *.o

clean:
	rm -f $(PROG) *.o
