#!/bin/bash

#make clean
#make

input_path="../../../workloads/"

#bp-dram
for file in $input_path*.spec; do
  counter=1
  echo "[Benchmark] bp-dram, workload: ${file##*/}"
  while [ $counter -le 10 ]
  do
    ./../../../ycsbc -db storeds -threads 1 -dbpath /pmem/bp.pmem -type bp-dram -P $input_path${file##*/}
    ((counter++))
  done
  echo "*****************<>*****************"
done

#bp-vmem
for file in $input_path*.spec; do
  counter=1
  echo "[Benchmark] bp-vmem, workload: ${file##*/}"
  while [ $counter -le 10 ]
  do
    ./../../../ycsbc -db storeds -threads 1 -dbpath /pmem -type bp-vmem -P $input_path${file##*/}
    ((counter++))
  done
  echo "*****************<>*****************"
done

#bp-pmem
for file in $input_path*.spec; do
  counter=1
  echo "[Benchmark] bp-pmem, workload: ${file##*/}"
  while [ $counter -le 10 ]
  do
    ./../../../ycsbc -db storeds -threads 1 -dbpath /pmem/bp.pmem -type bp-pmem -P $input_path${file##*/}
    ((counter++))
    rm /pmem/bp.pmem
  done
  echo "*****************<>*****************"
done

#bp-pmem-tx
for file in $input_path*.spec; do
  counter=1
  echo "[Benchmark] bp-pmem-tx, workload: ${file##*/}"
  while [ $counter -le 10 ]
  do
    ./../../../ycsbc -db storeds -threads 1 -dbpath /pmem/bp.pmem -type bp-pmem-tx -P $input_path${file##*/}
    ((counter++))
    rm /pmem/bp.pmem
  done
  echo "*****************<>*****************"
done
