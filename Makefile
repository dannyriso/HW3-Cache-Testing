CXX=g++-8
CXXFLAGS= -std=c++17 -g -Wall -Wextra -Werror -pedantic

default: test_cache_lib

all: test_cache_lib

test_cache_lib : test_cache_lib.o cache_lib.o lru_evictor.o fifo_evictor.o
	$(CXX) $(CXXFLAGS) -o $@ $^

clean:
	-rm -rf *.o test_cache_lib

test:
	./test_cache_lib

valgrind:
	valgrind ./test_cache_lib

debug:
	gdb ./test_cache_lib
