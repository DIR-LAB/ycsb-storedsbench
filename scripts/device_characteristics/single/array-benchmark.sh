#!/bin/bash

#make clean
#make

input_path="../../../workloads/"

#array-dram
for file in $input_path*.spec; do
  counter=1
  echo "[Benchmark] array-dram, workload: ${file##*/}"
  while [ $counter -le 10 ]
  do
    ./../../../ycsbc -db storeds -threads 1 -dbpath /pmem/array.pmem -type array-dram -P $input_path${file##*/}
    ((counter++))
  done
  echo "*****************<>*****************"
done

#array-vmem
for file in $input_path*.spec; do
  counter=1
  echo "[Benchmark] array-vmem, workload: ${file##*/}"
  while [ $counter -le 10 ]
  do
    ./../../../ycsbc -db storeds -threads 1 -dbpath /pmem -type array-vmem -P $input_path${file##*/}
    ((counter++))
  done
  echo "*****************<>*****************"
done

#array-pmem
for file in $input_path*.spec; do
  counter=1
  echo "[Benchmark] array-pmem, workload: ${file##*/}"
  while [ $counter -le 10 ]
  do
    ./../../../ycsbc -db storeds -threads 1 -dbpath /pmem/array.pmem -type array-pmem -P $input_path${file##*/}
    ((counter++))
    rm /pmem/array.pmem
  done
  echo "*****************<>*****************"
done

#array-pmem-tx
for file in $input_path*.spec; do
  counter=1
  echo "[Benchmark] array-pmem-tx, workload: ${file##*/}"
  while [ $counter -le 10 ]
  do
    ./../../../ycsbc -db storeds -threads 1 -dbpath /pmem/array.pmem -type array-pmem-tx -P $input_path${file##*/}
    ((counter++))
    rm /pmem/array.pmem
  done
  echo "*****************<>*****************"
done
