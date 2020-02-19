#!/bin/bash

#cd ../../
#make clean
#make
#cd scripts/parallel/

input_path="../../../workloads/"

#array-vmem workload-a
n_threads=1
while [ $n_threads -le 16 ]
do
  counter=1
  file="${input_path}workloada.spec"
  echo "[Benchmark] array-vmem, #of_threads: " $n_threads ", workload: $file"
  while [ $counter -le 10 ]
  do
    ./../../../ycsbc -db storeds -threads $n_threads -dbpath /pmem -type array-vmem-conc-pmlock -P $file
    ((counter++))
  done
  echo "*****************<>*****************"
  ((n_threads*=2))
done
echo "~~~~~~~~~~~~~~~~~~~~~~~~~<>~~~~~~~~~~~~~~~~~~~~~~~~~"

#array-pmem workload-a
n_threads=1
while [ $n_threads -le 16 ]
do
  counter=1
  file="${input_path}workloada.spec"
  echo "[Benchmark] array-pmem, #of_threads: " $n_threads ", workload: $file"
  while [ $counter -le 10 ]
  do
    ./../../../ycsbc -db storeds -threads $n_threads -dbpath /pmem/array.pmem -type array-pmem-conc-vmlock -P $file
    ((counter++))
    rm /pmem/array.pmem
  done
  echo "*****************<>*****************"
  ((n_threads*=2))
done
echo "~~~~~~~~~~~~~~~~~~~~~~~~~<>~~~~~~~~~~~~~~~~~~~~~~~~~"
