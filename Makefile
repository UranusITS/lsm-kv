
LINK.o = $(LINK.cc)
CXXFLAGS = -std=c++14 -Wall

all: correctness persistence

correctness: kvstore.o memtable.o skiplist.o correctness.o

persistence: kvstore.o memtable.o skiplist.o persistence.o

clean:
	-rm -f all *.o
