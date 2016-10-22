C = g++
CFLAGS = -std=c++11 -O3 -g
PROG = main

SRCS = main.cpp

all: $(PROG)

$(PROG):        $(SRCS)
	$(C) $(CFLAGS) $(LIBS) -c *.cpp
	$(C) -o $(PROG) *.o $(LIBS)

clear:
	rm -f *.o

clean:
	rm -f $(PROG) *.o
