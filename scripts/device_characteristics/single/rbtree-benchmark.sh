#!/bin/bash

#make clean
#make

input_path="../../../workloads/"

#rbtree-dram
for file in $input_path*.spec; do
  counter=1
  echo "[Benchmark] rbtree-dram, workload: ${file##*/}"
  while [ $counter -le 10 ]
  do
    ./../../../ycsbc -db storeds -threads 1 -dbpath /pmem/rbtree.pmem -type rbtree-dram -P $input_path${file##*/}
    ((counter++))
  done
  echo "*****************<>*****************"
done

#rbtree-vmem
for file in $input_path*.spec; do
  counter=1
  echo "[Benchmark] rbtree-vmem, workload: ${file##*/}"
  while [ $counter -le 10 ]
  do
    ./../../../ycsbc -db storeds -threads 1 -dbpath /pmem -type rbtree-vmem -P $input_path${file##*/}
    ((counter++))
  done
  echo "*****************<>*****************"
done

#rbtree-pmem
for file in $input_path*.spec; do
  counter=1
  echo "[Benchmark] rbtree-pmem, workload: ${file##*/}"
  while [ $counter -le 10 ]
  do
    ./../../../ycsbc -db storeds -threads 1 -dbpath /pmem/rbtree.pmem -type rbtree-pmem -P $input_path${file##*/}
    ((counter++))
    rm /pmem/rbtree.pmem
  done
  echo "*****************<>*****************"
done

#rbtree-pmem-tx
for file in $input_path*.spec; do
  counter=1
  echo "[Benchmark] rbtree-pmem-tx, workload: ${file##*/}"
  while [ $counter -le 10 ]
  do
    ./../../../ycsbc -db storeds -threads 1 -dbpath /pmem/rbtree.pmem -type rbtree-pmem-tx -P $input_path${file##*/}
    ((counter++))
    rm /pmem/rbtree.pmem
  done
  echo "*****************<>*****************"
done
