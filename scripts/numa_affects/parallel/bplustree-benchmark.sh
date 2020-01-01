#!/bin/bash

#cd ../../
#make clean
#make
#cd scripts/parallel/

input_path="../../../workloads/"

#bplustree-dram
for file in $input_path*.spec; do
  n_threads=1
  while [ $n_threads -le 16 ]
  do
    counter=1
    echo "[Benchmark] bplustree-dram, #of_threads: " $n_threads ", workload: ${file##*/}"
    while [ $counter -le 10 ]
    do
      ./../../../ycsbc_parallel_non_numa -db storeds -threads $n_threads -dbpath /pmem/bplustree -type bplustree-dram -P $input_path${file##*/}
      ((counter++))
    done
    echo "*****************<>*****************"
    ((n_threads*=2))
  done
  echo "~~~~~~~~~~~~~~~~~~~~~~~~~<>~~~~~~~~~~~~~~~~~~~~~~~~~"
done

#bplustree-vmem
for file in $input_path*.spec; do
  n_threads=1
  while [ $n_threads -le 16 ]
  do
    counter=1
    echo "[Benchmark] bplustree-vmem, #of_threads: " $n_threads ", workload: ${file##*/}"
    while [ $counter -le 10 ]
    do
      ./../../../ycsbc_parallel_non_numa -db storeds -threads $n_threads -dbpath /pmem -type bplustree-vmem -P $input_path${file##*/}
      ((counter++))
    done
    echo "*****************<>*****************"
    ((n_threads*=2))
  done
  echo "~~~~~~~~~~~~~~~~~~~~~~~~~<>~~~~~~~~~~~~~~~~~~~~~~~~~"
done

#bplustree-pmem
for file in $input_path*.spec; do
  n_threads=1
  while [ $n_threads -le 16 ]
  do
    counter=1
    echo "[Benchmark] bplustree-pmem, #of_threads: " $n_threads ", workload: ${file##*/}"
    while [ $counter -le 10 ]
    do
      ./../../../ycsbc_parallel_non_numa -db storeds -threads $n_threads -dbpath /pmem/bplustree -type bplustree-pmem -P $input_path${file##*/}
      ((counter++))
      rm /pmem/bplustree*.pmem
    done
    echo "*****************<>*****************"
    ((n_threads*=2))
  done
  echo "~~~~~~~~~~~~~~~~~~~~~~~~~<>~~~~~~~~~~~~~~~~~~~~~~~~~"
done

#bplustree-pmem-tx
for file in $input_path*.spec; do
  n_threads=1
  while [ $n_threads -le 16 ]
  do
    counter=1
    echo "[Benchmark] bplustree-pmem-tx, #of_threads: " $n_threads ", workload: ${file##*/}"
    while [ $counter -le 10 ]
    do
      ./../../../ycsbc_parallel_non_numa -db storeds -threads $n_threads -dbpath /pmem/bplustree -type bplustree-pmem-tx -P $input_path${file##*/}
      ((counter++))
      rm /pmem/bplustree*.pmem
    done
    echo "*****************<>*****************"
    ((n_threads*=2))
  done
  echo "~~~~~~~~~~~~~~~~~~~~~~~~~<>~~~~~~~~~~~~~~~~~~~~~~~~~"
done
