#!/bin/bash

#make clean
#make

input_path="../../../workloads/"

#skiplist-dram
for file in $input_path*.spec; do
  counter=1
  echo "[Benchmark] skiplist-dram, workload: ${file##*/}"
  while [ $counter -le 10 ]
  do
    ./../../../ycsbc -db storeds -threads 1 -dbpath /pmem/skiplist.pmem -type skiplist-dram -P $input_path${file##*/}
    ((counter++))
  done
  echo "*****************<>*****************"
done

#skiplist-vmem
for file in $input_path*.spec; do
  counter=1
  echo "[Benchmark] skiplist-vmem, workload: ${file##*/}"
  while [ $counter -le 10 ]
  do
    ./../../../ycsbc -db storeds -threads 1 -dbpath /pmem -type skiplist-vmem -P $input_path${file##*/}
    ((counter++))
  done
  echo "*****************<>*****************"
done

#skiplist-pmem
for file in $input_path*.spec; do
  counter=1
  echo "[Benchmark] skiplist-pmem, workload: ${file##*/}"
  while [ $counter -le 10 ]
  do
    ./../../../ycsbc -db storeds -threads 1 -dbpath /pmem/skiplist.pmem -type skiplist-pmem -P $input_path${file##*/}
    ((counter++))
    rm /pmem/skiplist.pmem
  done
  echo "*****************<>*****************"
done

#skiplist-pmem-tx
for file in $input_path*.spec; do
  counter=1
  echo "[Benchmark] skiplist-pmem-tx, workload: ${file##*/}"
  while [ $counter -le 10 ]
  do
    ./../../../ycsbc -db storeds -threads 1 -dbpath /pmem/skiplist.pmem -type skiplist-pmem-tx -P $input_path${file##*/}
    ((counter++))
    rm /pmem/skiplist.pmem
  done
  echo "*****************<>*****************"
done
