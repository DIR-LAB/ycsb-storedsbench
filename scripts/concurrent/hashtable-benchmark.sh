#!/bin/bash

#cd ../../
#make clean
#make
#cd scripts/parallel/

input_path="../../workloads/"

#hashtable-dram
for file in $input_path*.spec; do
  n_threads=1
  while [ $n_threads -le 16 ]
  do
    counter=1
    echo "[Benchmark] hashtable-dram, #of_threads: " $n_threads ", workload: ${file##*/}"
    while [ $counter -le 10 ]
    do
      ./../../ycsbc -db storeds -threads $n_threads -dbpath /pmem/hashtable.pmem -type hashtable-dram-conc-lock -P $input_path${file##*/}
      ((counter++))
    done
    echo "*****************<>*****************"
    ((n_threads*=2))
  done
  echo "~~~~~~~~~~~~~~~~~~~~~~~~~<>~~~~~~~~~~~~~~~~~~~~~~~~~"
done

#hashtable-pmem
for file in $input_path*.spec; do
  n_threads=1
  while [ $n_threads -le 16 ]
  do
    counter=1
    echo "[Benchmark] hashtable-pmem, #of_threads: " $n_threads ", workload: ${file##*/}"
    while [ $counter -le 10 ]
    do
      ./../../ycsbc -db storeds -threads $n_threads -dbpath /pmem/hashtable.pmem -type hashtable-pmem-conc-lock -P $input_path${file##*/}
      ((counter++))
      rm /pmem/hashtable.pmem
    done
    echo "*****************<>*****************"
    ((n_threads*=2))
  done
  echo "~~~~~~~~~~~~~~~~~~~~~~~~~<>~~~~~~~~~~~~~~~~~~~~~~~~~"
done

#hashtable-pmem-tx
for file in $input_path*.spec; do
  n_threads=1
  while [ $n_threads -le 16 ]
  do
    counter=1
    echo "[Benchmark] hashtable-pmem-tx, #of_threads: " $n_threads ", workload: ${file##*/}"
    while [ $counter -le 10 ]
    do
      ./../../ycsbc -db storeds -threads $n_threads -dbpath /pmem/hashtable.pmem -type hashtable-pmem-tx-conc-lock -P $input_path${file##*/}
      ((counter++))
      rm /pmem/hashtable.pmem
    done
    echo "*****************<>*****************"
    ((n_threads*=2))
  done
  echo "~~~~~~~~~~~~~~~~~~~~~~~~~<>~~~~~~~~~~~~~~~~~~~~~~~~~"
done
