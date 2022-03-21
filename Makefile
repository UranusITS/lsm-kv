
LINK.o = $(LINK.cc)
CXXFLAGS = -std=c++14 -Wall

all: correctness persistence

correctness: kvstore.o memtable.o skiplist.o sstable.o bloomfilter.o correctness.o

persistence: kvstore.o memtable.o skiplist.o sstable.o bloomfilter.o persistence.o

clean:
	-rm -f all *.o
