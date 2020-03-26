#include<iostream>
#include<cstdlib>
#include<stdio.h>
#include<chrono>
#include<cmath>
#include<random>
#include <algorithm>
#include<omp.h>
#include<fstream>

using namespace std;
using namespace chrono;

int randomize(unsigned int indices_limit) {
  unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
  std::default_random_engine generator (seed);
  std::uniform_int_distribution<unsigned int> distribution(0,indices_limit-1);
  return distribution(generator);
}

void print_usage(){
  cout << "Usage: [-t | --threads] <number of threads> \n"
        "       [-rw | --read-write     Enables read-modify-write \n"
        "       [-s | --strided access     Enables sequential access \n"
        "       [-h | --help]   Shows Usage\n"
      <<     endl;
}

void** random_memory(size_t size) {
   size_t len = size / sizeof(void*);
   void** memory = new void*[len];

   size_t* indices = new size_t[len];
   for (size_t i = 0; i < len; ++i) {
      indices[i] = i;
   }

   for (size_t i = 0; i < len-1; ++i) {
      size_t j = i + randomize(len - i);
      if (i != j) {
        swap(indices[i], indices[j]);
      }
   }

   for (size_t i = 1; i < len; ++i) {
      memory[indices[i-1]] = (void*) &memory[indices[i]];
   }

   memory[indices[len-1]] = (void*) &memory[indices[0]];
   delete[] indices;
   return memory;
}

void** stride_memory(size_t size, int stride_count) {

  size_t len = size/sizeof(void*);
  void** memory = new void*[len];

  size_t* indices = new size_t[len];
  for(size_t i=0; i<len; i++) {
    indices[i] = i;
  }

  for(size_t i=0; i<len; i++) {
    int val = (i*stride_count/sizeof(void*))%len;
    indices[i] = val;
  }

  for(size_t i=0; i<len-1; i++) {
    memory[indices[i]] = (void*) &memory[indices[i+1]]; 
  }
  memory[indices[len-1]] = (void *) &memory[indices[0]];
  delete[] indices;
  return memory;
}

volatile void* chase_pointers_global;

double chase_pointers(void** memory, std::size_t count, bool rw) {
  auto end=system_clock::now();
  auto start=system_clock::now();

  void** p = (void**) memory;
  void** next;
  void** temp;

  if(rw) {
    start = system_clock::now();
    while (count-- > 0) {
       next = (void**) *p;
       temp = (void**) *p;
       temp = (void**) ((uintptr_t)temp & 0x7fffffffffffffff);
       *p= temp;
       p = next;
    }
    end = system_clock::now();
    } else {
     start=system_clock::now();
     while (count-- > 0) {
       p = (void**) *p;
     }
     end=system_clock::now();
  }

  auto elapsed = duration<double>(end-start).count();
  chase_pointers_global = *p;
  return elapsed;
}

int main(int argc, char *argv[]) {

  int num_threads = 1;
  bool rw = false;
  bool stride = false;
  int stride_size = 64;
  size_t min_size;
  size_t max_size;
  int stride_min;
  int stride_max;
  
  for(int i=1; i<argc; i++) {
    string arg = argv[i];
    if ((arg == "-h")) {
            print_usage();
            exit(0);
    } else if ((arg == "-t")) {
            if(i+1 < argc) {
                    num_threads = atoi(argv[++i]);
            } else {
                    exit(0);
            }
    } else if ((arg == "-rw")) {
            rw = true;
    } else if ((arg == "-s")) {
            stride = true;
    } else if ((arg == "-ss")) {
           if(i+1 < argc) {
              stride_size = atoi(argv[++i]);
           } else { 
              exit(0); 
          }
    } else {
            print_usage();  
            exit(0);
    }
  }

  if(stride) {
    min_size = 1024*1024*64;
    max_size = 1024*1024*64;
    stride_min=0;
    stride_max=1024;
  } else {
    min_size = 64;
    max_size = 1024*1024*64;
    stride_min=0;
    stride_max=0;
  }

  ofstream csv;
  string name_rw = (rw) ? "rw" : "read";
  string name_stride = (stride) ? "stride" : "random";
  string file_name = name_rw + "_" + to_string(num_threads) + "_" + name_stride + ".csv";
  csv.open(file_name);
  csv << "stride,time(ns),Threads,\n";
  csv.close();

  printf("memsize time_in_ns threads stride\n");
  for (size_t memsize = min_size; memsize <= max_size; memsize*=2) {
    for(int stride_count=stride_min; stride_count<=stride_max; stride_count+= stride_size) {
      size_t count;
      double time_allthread = 0;
      count = std::max(memsize*16, (size_t){1}<<30);

      if(stride) {
        #pragma omp parallel num_threads(num_threads)
        { 
           //printf("Hello from thread %d, nthreads %d\n", omp_get_thread_num(), omp_get_num_threads());
            void** memory = stride_memory(memsize, stride_count);
            double t = chase_pointers(memory, count, rw);
            #pragma omp atomic
                    time_allthread += t;
            delete[] memory;
        } 
      } else {
       	#pragma omp parallel num_threads(num_threads)
        { 
           //printf("Hello from thread %d, nthreads %d\n", omp_get_thread_num(), omp_get_num_threads());
           	void** memory = random_memory(memsize);
           	double t = chase_pointers(memory, count, rw);
          	#pragma omp atomic
                   	time_allthread += t;
           	delete[] memory;
        }
      }
      double time_ns = (time_allthread/num_threads) * 1000000000/count;
            //double ns = t * 1000000000 / count;
      csv.open(file_name, std::ios_base::app);
      csv << memsize << "," << time_ns << "," << num_threads << ",\n";
      csv.close();
      printf("%9u   %10.5lf   %d   %s%d\n", memsize, time_ns, num_threads, name_stride.c_str(), stride_count); std::cout.flush();
    }
  }
return 0;
}
