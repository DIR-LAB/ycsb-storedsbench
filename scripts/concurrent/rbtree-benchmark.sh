#!/bin/bash

#cd ../../
#make clean
#make
#cd scripts/parallel/

input_path="../../workloads/"

#rbtree-dram
for file in $input_path*.spec; do
  n_threads=1
  while [ $n_threads -le 16 ]
  do
    counter=1
    echo "[Benchmark] rbtree-dram, #of_threads: " $n_threads ", workload: ${file##*/}"
    while [ $counter -le 10 ]
    do
      ./../../ycsbc -db storeds -threads $n_threads -dbpath /pmem/rbtree.pmem -type rbtree-dram-conc-lock -P $input_path${file##*/}
      ((counter++))
    done
    echo "*****************<>*****************"
    ((n_threads*=2))
  done
  echo "~~~~~~~~~~~~~~~~~~~~~~~~~<>~~~~~~~~~~~~~~~~~~~~~~~~~"
done

#rbtree-pmem
for file in $input_path*.spec; do
  n_threads=1
  while [ $n_threads -le 16 ]
  do
    counter=1
    echo "[Benchmark] rbtree-pmem, #of_threads: " $n_threads ", workload: ${file##*/}"
    while [ $counter -le 10 ]
    do
      ./../../ycsbc -db storeds -threads $n_threads -dbpath /pmem/rbtree.pmem -type rbtree-pmem-conc-lock -P $input_path${file##*/}
      ((counter++))
      rm /pmem/rbtree.pmem
    done
    echo "*****************<>*****************"
    ((n_threads*=2))
  done
  echo "~~~~~~~~~~~~~~~~~~~~~~~~~<>~~~~~~~~~~~~~~~~~~~~~~~~~"
done

#rbtree-pmem-tx
for file in $input_path*.spec; do
  n_threads=1
  while [ $n_threads -le 16 ]
  do
    counter=1
    echo "[Benchmark] rbtree-pmem-tx, #of_threads: " $n_threads ", workload: ${file##*/}"
    while [ $counter -le 10 ]
    do
      ./../../ycsbc -db storeds -threads $n_threads -dbpath /pmem/rbtree.pmem -type rbtree-pmem-tx-conc-lock -P $input_path${file##*/}
      ((counter++))
      rm /pmem/rbtree.pmem
    done
    echo "*****************<>*****************"
    ((n_threads*=2))
  done
  echo "~~~~~~~~~~~~~~~~~~~~~~~~~<>~~~~~~~~~~~~~~~~~~~~~~~~~"
done
