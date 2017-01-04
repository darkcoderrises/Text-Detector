C = g++
CFLAGS = -std=c++11 -g 
CFLAGS+=`pkg-config --cflags opencv`
LDFLAGS+=`pkg-config --libs opencv`
PROG = main
FILES = er.cpp region.cpp main.cpp

SRCS = main.cpp

all: $(PROG)

$(PROG):        $(SRCS)
	$(C) $(CFLAGS) $(LIBS) -c $(FILES) $(LDFLAGS)
	$(C) -o $(PROG) $(FILES:.cpp=.o) $(LIBS) $(LDFLAGS)

clear:
	rm -f *.o

clean:
	rm -f $(PROG) *.o
