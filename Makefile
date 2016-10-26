C = g++
CFLAGS = -std=c++11 -O3 -g 
CFLAGS+=`pkg-config --cflags opencv`
LDFLAGS+=`pkg-config --libs opencv`
PROG = main

SRCS = main.cpp

all: $(PROG)

$(PROG):        $(SRCS)
	$(C) $(CFLAGS) $(LIBS) -c *.cpp $(LDFLAGS)
	$(C) -o $(PROG) *.o $(LIBS) $(LDFLAGS)

clear:
	rm -f *.o

clean:
	rm -f $(PROG) *.o
