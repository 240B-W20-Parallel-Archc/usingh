default: cacheperf

CXX=g++
CFLAGS=--std=c++11 -fopenmp

cacheperf: cache_perf.cpp
	$(CXX) $(CFLAGS) cache_perf.cpp -o cacheperf

clean:
	rm -f *.o cacheperf;