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
	rm $(FILES:.cpp=.o)

boost:
	$(C) $(CFLAGS) $(LIBGS) -c boost.cpp $(LDFLAGS)
	$(C) -o boost boost.o $(LIBS) $(LDFLAGS)
	rm boost.o
 
train_2:
	$(C) $(CFLAGS) $(LIBGS) -c er.cpp region.cpp train_2.cpp $(LDFLAGS)
	$(C) -o train_2 er.o region.o train_2.o $(LIBS) $(LDFLAGS)
	rm er.o region.o train_2.o

train:
	$(C) $(CFLAGS) $(LIBGS) -c train.cpp $(LDFLAGS)
	$(C) -o train train.o $(LIBS) $(LDFLAGS)
	rm train.o
    
clear:
	rm -f *.o

clean:
	rm -f $(PROG) *.o train boost train_2
