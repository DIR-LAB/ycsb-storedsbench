#!/bin/bash

#make clean
#make

input_path="../../../workloads/"

#btree-dram
for file in $input_path*.spec; do
  counter=1
  echo "[Benchmark] btree-dram, workload: ${file##*/}"
  while [ $counter -le 10 ]
  do
    ./../../../ycsbc -db storeds -threads 1 -dbpath /pmem/btree.pmem -type btree-dram -P $input_path${file##*/}
    ((counter++))
  done
  echo "*****************<>*****************"
done

#btree-vmem
for file in $input_path*.spec; do
  counter=1
  echo "[Benchmark] btree-vmem, workload: ${file##*/}"
  while [ $counter -le 10 ]
  do
    ./../../../ycsbc -db storeds -threads 1 -dbpath /pmem -type btree-vmem -P $input_path${file##*/}
    ((counter++))
  done
  echo "*****************<>*****************"
done

#btree-pmem
for file in $input_path*.spec; do
  counter=1
  echo "[Benchmark] btree-pmem, workload: ${file##*/}"
  while [ $counter -le 10 ]
  do
    ./../../../ycsbc -db storeds -threads 1 -dbpath /pmem/btree.pmem -type btree-pmem -P $input_path${file##*/}
    ((counter++))
    rm /pmem/btree.pmem
  done
  echo "*****************<>*****************"
done

#btree-pmem-tx
for file in $input_path*.spec; do
  counter=1
  echo "[Benchmark] btree-pmem-tx, workload: ${file##*/}"
  while [ $counter -le 10 ]
  do
    ./../../../ycsbc -db storeds -threads 1 -dbpath /pmem/btree.pmem -type btree-pmem-tx -P $input_path${file##*/}
    ((counter++))
    rm /pmem/btree.pmem
  done
  echo "*****************<>*****************"
done
