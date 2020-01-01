#!/bin/bash

#cd ../../
#make clean
#make
#cd scripts/parallel/

input_path="../../../workloads/"

#skiplist-dram
for file in $input_path*.spec; do
  n_threads=1
  while [ $n_threads -le 16 ]
  do
    counter=1
    echo "[Benchmark] skiplist-dram, #of_threads: " $n_threads ", workload: ${file##*/}"
    while [ $counter -le 10 ]
    do
      ./../../../ycsbc_parallel -db storeds -threads $n_threads -dbpath /pmem/skiplist -type skiplist-dram -P $input_path${file##*/}
      ((counter++))
    done
    echo "*****************<>*****************"
    ((n_threads*=2))
  done
  echo "~~~~~~~~~~~~~~~~~~~~~~~~~<>~~~~~~~~~~~~~~~~~~~~~~~~~"
done

#skiplist-vmem
for file in $input_path*.spec; do
  n_threads=1
  while [ $n_threads -le 16 ]
  do
    counter=1
    echo "[Benchmark] skiplist-vmem, #of_threads: " $n_threads ", workload: ${file##*/}"
    while [ $counter -le 10 ]
    do
      ./../../../ycsbc_parallel -db storeds -threads $n_threads -dbpath /pmem -type skiplist-vmem -P $input_path${file##*/}
      ((counter++))
    done
    echo "*****************<>*****************"
    ((n_threads*=2))
  done
  echo "~~~~~~~~~~~~~~~~~~~~~~~~~<>~~~~~~~~~~~~~~~~~~~~~~~~~"
done

#skiplist-pmem
for file in $input_path*.spec; do
  n_threads=1
  while [ $n_threads -le 16 ]
  do
    counter=1
    echo "[Benchmark] skiplist-pmem, #of_threads: " $n_threads ", workload: ${file##*/}"
    while [ $counter -le 10 ]
    do
      ./../../../ycsbc_parallel -db storeds -threads $n_threads -dbpath /pmem/skiplist -type skiplist-pmem -P $input_path${file##*/}
      ((counter++))
      rm /pmem/skiplist*.pmem
    done
    echo "*****************<>*****************"
    ((n_threads*=2))
  done
  echo "~~~~~~~~~~~~~~~~~~~~~~~~~<>~~~~~~~~~~~~~~~~~~~~~~~~~"
done

#skiplist-pmem-tx
for file in $input_path*.spec; do
  n_threads=1
  while [ $n_threads -le 16 ]
  do
    counter=1
    echo "[Benchmark] skiplist-pmem-tx, #of_threads: " $n_threads ", workload: ${file##*/}"
    while [ $counter -le 10 ]
    do
      ./../../../ycsbc_parallel -db storeds -threads $n_threads -dbpath /pmem/skiplist -type skiplist-pmem-tx -P $input_path${file##*/}
      ((counter++))
      rm /pmem/skiplist*.pmem
    done
    echo "*****************<>*****************"
    ((n_threads*=2))
  done
  echo "~~~~~~~~~~~~~~~~~~~~~~~~~<>~~~~~~~~~~~~~~~~~~~~~~~~~"
done
