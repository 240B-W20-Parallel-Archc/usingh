#/bin/bash

make clean
make

#Read - Threads - Random
  for t in 1 2 4 8 12 16
  do
        echo -e "Read Random access with $t threads...\n"
        ./cacheperf -t $t
  done



 #Read-Write - Threads - Random
 for t in 1 2 4 8 12 16
 do
        echo -e "Read-Write Random access with $t threads..\n"
        ./cacheperf -t $t -rw
 done

 #Read - Threads - Strided
 for t in 1 2 4 8 12 16 
 do
 	      echo -e "Read Random Strided access with $t threads..\n"
 	      ./cacheperf  -t $t -s
 done
 
 
 
 #Read-Write - Threads - Strided
 for t in 1 2 4 8 12 16 
 do
 	      echo -e "Read-Write Strided access with $t threads..\n"
 	      ./cacheperf  -t $t -s -rw
 done
