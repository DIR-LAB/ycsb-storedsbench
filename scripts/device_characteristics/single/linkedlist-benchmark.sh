#!/bin/bash

#make clean
#make

input_path="../../../workloads/"

#linkedlist-dram
for file in $input_path*.spec; do
  counter=1
  echo "[Benchmark] linkedlist-dram, workload: ${file##*/}"
  while [ $counter -le 10 ]
  do
    ./../../../ycsbc -db storeds -threads 1 -dbpath /pmem/linkedlist.pmem -type linkedlist-dram -P $input_path${file##*/}
    ((counter++))
  done
  echo "*****************<>*****************"
done

#linkedlist-vmem
for file in $input_path*.spec; do
  counter=1
  echo "[Benchmark] linkedlist-vmem, workload: ${file##*/}"
  while [ $counter -le 10 ]
  do
    ./../../../ycsbc -db storeds -threads 1 -dbpath /pmem -type linkedlist-vmem -P $input_path${file##*/}
    ((counter++))
  done
  echo "*****************<>*****************"
done

#linkedlist-pmem
for file in $input_path*.spec; do
  counter=1
  echo "[Benchmark] linkedlist-pmem, workload: ${file##*/}"
  while [ $counter -le 10 ]
  do
    ./../../../ycsbc -db storeds -threads 1 -dbpath /pmem/linkedlist.pmem -type linkedlist-pmem -P $input_path${file##*/}
    ((counter++))
    rm /pmem/linkedlist.pmem
  done
  echo "*****************<>*****************"
done

#linkedlist-pmem-tx
for file in $input_path*.spec; do
  counter=1
  echo "[Benchmark] linkedlist-pmem-tx, workload: ${file##*/}"
  while [ $counter -le 10 ]
  do
    ./../../../ycsbc -db storeds -threads 1 -dbpath /pmem/linkedlist.pmem -type linkedlist-pmem-tx -P $input_path${file##*/}
    ((counter++))
    rm /pmem/linkedlist.pmem
  done
  echo "*****************<>*****************"
done
