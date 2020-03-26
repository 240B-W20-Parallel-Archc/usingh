CXX=g++
CFLAGS=--std=c++11 -fopenmp

all: cache_perf.cpp
        $(CXX) $(CFLAGS) cache_perf.cpp -o cacheperf

clean:
        rm -f *.o cacheperf;
