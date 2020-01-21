#!/bin/bash

#make clean
#make

input_path="../../../workloads/"

#bt-dram
for file in $input_path*.spec; do
  counter=1
  echo "[Benchmark] bt-dram, workload: ${file##*/}"
  while [ $counter -le 10 ]
  do
    ./../../../ycsbc -db storeds -threads 1 -dbpath /pmem/bt.pmem -type bt-dram -P $input_path${file##*/}
    ((counter++))
  done
  echo "*****************<>*****************"
done

#bt-vmem
for file in $input_path*.spec; do
  counter=1
  echo "[Benchmark] bt-vmem, workload: ${file##*/}"
  while [ $counter -le 10 ]
  do
    ./../../../ycsbc -db storeds -threads 1 -dbpath /pmem -type bt-vmem -P $input_path${file##*/}
    ((counter++))
  done
  echo "*****************<>*****************"
done

#bt-pmem
for file in $input_path*.spec; do
  counter=1
  echo "[Benchmark] bt-pmem, workload: ${file##*/}"
  while [ $counter -le 10 ]
  do
    ./../../../ycsbc -db storeds -threads 1 -dbpath /pmem/bt.pmem -type bt-pmem -P $input_path${file##*/}
    ((counter++))
    rm /pmem/bt.pmem
  done
  echo "*****************<>*****************"
done

#bt-pmem-tx
for file in $input_path*.spec; do
  counter=1
  echo "[Benchmark] bt-pmem-tx, workload: ${file##*/}"
  while [ $counter -le 10 ]
  do
    ./../../../ycsbc -db storeds -threads 1 -dbpath /pmem/bt.pmem -type bt-pmem-tx -P $input_path${file##*/}
    ((counter++))
    rm /pmem/bt.pmem
  done
  echo "*****************<>*****************"
done
