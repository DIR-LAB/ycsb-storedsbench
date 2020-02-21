#!/bin/bash

#cd ../../
#make clean
#make
#cd scripts/parallel/

input_path="../../../workloads/"

#hashlist-vmem workload-a
n_threads=1
while [ $n_threads -le 16 ]
do
  counter=1
  file="${input_path}workloada.spec"
  echo "[Benchmark] hashlist-vmem, #of_threads: " $n_threads ", workload: $file"
  while [ $counter -le 10 ]
  do
    ./../../../ycsbc -db storeds -threads $n_threads -dbpath /pmem -type hl-vmem-conc-pmlock -P $file
    ((counter++))
  done
  echo "*****************<>*****************"
  ((n_threads*=2))
done
echo "~~~~~~~~~~~~~~~~~~~~~~~~~<>~~~~~~~~~~~~~~~~~~~~~~~~~"

#hashlist-pmem workload-a
n_threads=1
while [ $n_threads -le 16 ]
do
  counter=1
  file="${input_path}workloada.spec"
  echo "[Benchmark] hashlist-pmem, #of_threads: " $n_threads ", workload: $file"
  while [ $counter -le 10 ]
  do
    ./../../../ycsbc -db storeds -threads $n_threads -dbpath /pmem/hl.pmem -type hl-pmem-conc-vmlock -P $file
    ((counter++))
    rm /pmem/hl.pmem
  done
  echo "*****************<>*****************"
  ((n_threads*=2))
done
echo "~~~~~~~~~~~~~~~~~~~~~~~~~<>~~~~~~~~~~~~~~~~~~~~~~~~~"
