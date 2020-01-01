#!/bin/bash

#make clean
#make

input_path="../../../workloads/"

#bplustree-dram
for file in $input_path*.spec; do
  counter=1
  echo "[Benchmark] bplustree-dram, workload: ${file##*/}"
  while [ $counter -le 10 ]
  do
    ./../../../ycsbc -db storeds -threads 1 -dbpath /pmem/bplustree.pmem -type bplustree-dram -P $input_path${file##*/}
    ((counter++))
  done
  echo "*****************<>*****************"
done

#bplustree-vmem
for file in $input_path*.spec; do
  counter=1
  echo "[Benchmark] bplustree-vmem, workload: ${file##*/}"
  while [ $counter -le 10 ]
  do
    ./../../../ycsbc -db storeds -threads 1 -dbpath /pmem -type bplustree-vmem -P $input_path${file##*/}
    ((counter++))
  done
  echo "*****************<>*****************"
done

#bplustree-pmem
for file in $input_path*.spec; do
  counter=1
  echo "[Benchmark] bplustree-pmem, workload: ${file##*/}"
  while [ $counter -le 10 ]
  do
    ./../../../ycsbc -db storeds -threads 1 -dbpath /pmem/bplustree.pmem -type bplustree-pmem -P $input_path${file##*/}
    ((counter++))
    rm /pmem/bplustree.pmem
  done
  echo "*****************<>*****************"
done

#bplustree-pmem-tx
for file in $input_path*.spec; do
  counter=1
  echo "[Benchmark] bplustree-pmem-tx, workload: ${file##*/}"
  while [ $counter -le 10 ]
  do
    ./../../../ycsbc -db storeds -threads 1 -dbpath /pmem/bplustree.pmem -type bplustree-pmem-tx -P $input_path${file##*/}
    ((counter++))
    rm /pmem/bplustree.pmem
  done
  echo "*****************<>*****************"
done
