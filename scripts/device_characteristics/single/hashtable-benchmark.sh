#!/bin/bash

#make clean
#make

input_path="../../../workloads/"

#hashtable-dram
for file in $input_path*.spec; do
  counter=1
  echo "[Benchmark] hashtable-dram, workload: ${file##*/}"
  while [ $counter -le 10 ]
  do
    ./../../../ycsbc -db storeds -threads 1 -dbpath /pmem/hashtable.pmem -type hashtable-dram -P $input_path${file##*/}
    ((counter++))
  done
  echo "*****************<>*****************"
done

#hashtable-vmem
for file in $input_path*.spec; do
  counter=1
  echo "[Benchmark] hashtable-vmem, workload: ${file##*/}"
  while [ $counter -le 10 ]
  do
    ./../../../ycsbc -db storeds -threads 1 -dbpath /pmem -type hashtable-vmem -P $input_path${file##*/}
    ((counter++))
  done
  echo "*****************<>*****************"
done

#hashtable-pmem
for file in $input_path*.spec; do
  counter=1
  echo "[Benchmark] hashtable-pmem, workload: ${file##*/}"
  while [ $counter -le 10 ]
  do
    ./../../../ycsbc -db storeds -threads 1 -dbpath /pmem/hashtable.pmem -type hashtable-pmem -P $input_path${file##*/}
    ((counter++))
    rm /pmem/hashtable.pmem
  done
  echo "*****************<>*****************"
done

#hashtable-pmem-tx
for file in $input_path*.spec; do
  counter=1
  echo "[Benchmark] hashtable-pmem-tx, workload: ${file##*/}"
  while [ $counter -le 10 ]
  do
    ./../../../ycsbc -db storeds -threads 1 -dbpath /pmem/hashtable.pmem -type hashtable-pmem-tx -P $input_path${file##*/}
    ((counter++))
    rm /pmem/hashtable.pmem
  done
  echo "*****************<>*****************"
done
