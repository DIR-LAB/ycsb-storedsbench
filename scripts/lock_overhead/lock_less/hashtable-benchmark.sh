#!/bin/bash

#cd ../../
#make clean
#make
#cd scripts/parallel/

input_path="../../../workloads/"

#hashtable-dram
for file in $input_path*a.spec $input_path*e.spec; do
  n_threads=1
  while [ $n_threads -le 16 ]
  do
    counter=1
    echo "[Benchmark] hashtable-dram, #of_threads: " $n_threads ", workload: ${file##*/}"
    while [ $counter -le 10 ]
    do
      ./../../../ycsbc_parallel -db storeds -threads $n_threads -dbpath /pmem/hashtable -type hashtable-dram -P $input_path${file##*/}
      ((counter++))
    done
    echo "*****************<>*****************"
    ((n_threads*=2))
  done
  echo "~~~~~~~~~~~~~~~~~~~~~~~~~<>~~~~~~~~~~~~~~~~~~~~~~~~~"
done

#hashtable-vmem
for file in $input_path*a.spec $input_path*e.spec; do
  n_threads=1
  while [ $n_threads -le 16 ]
  do
    counter=1
    echo "[Benchmark] hashtable-vmem, #of_threads: " $n_threads ", workload: ${file##*/}"
    while [ $counter -le 10 ]
    do
      ./../../../ycsbc_parallel -db storeds -threads $n_threads -dbpath /pmem -type hashtable-vmem -P $input_path${file##*/}
      ((counter++))
    done
    echo "*****************<>*****************"
    ((n_threads*=2))
  done
  echo "~~~~~~~~~~~~~~~~~~~~~~~~~<>~~~~~~~~~~~~~~~~~~~~~~~~~"
done

#hashtable-pmem
for file in $input_path*a.spec $input_path*e.spec; do
  n_threads=1
  while [ $n_threads -le 16 ]
  do
    counter=1
    echo "[Benchmark] hashtable-pmem, #of_threads: " $n_threads ", workload: ${file##*/}"
    while [ $counter -le 10 ]
    do
      ./../../../ycsbc_parallel -db storeds -threads $n_threads -dbpath /pmem/hashtable -type hashtable-pmem -P $input_path${file##*/}
      ((counter++))
      rm /pmem/hashtable*.pmem
    done
    echo "*****************<>*****************"
    ((n_threads*=2))
  done
  echo "~~~~~~~~~~~~~~~~~~~~~~~~~<>~~~~~~~~~~~~~~~~~~~~~~~~~"
done

#hashtable-pmem-tx
for file in $input_path*a.spec $input_path*e.spec; do
  n_threads=1
  while [ $n_threads -le 16 ]
  do
    counter=1
    echo "[Benchmark] hashtable-pmem-tx, #of_threads: " $n_threads ", workload: ${file##*/}"
    while [ $counter -le 10 ]
    do
      ./../../../ycsbc_parallel -db storeds -threads $n_threads -dbpath /pmem/hashtable -type hashtable-pmem-tx -P $input_path${file##*/}
      ((counter++))
      rm /pmem/hashtable*.pmem
    done
    echo "*****************<>*****************"
    ((n_threads*=2))
  done
  echo "~~~~~~~~~~~~~~~~~~~~~~~~~<>~~~~~~~~~~~~~~~~~~~~~~~~~"
done
