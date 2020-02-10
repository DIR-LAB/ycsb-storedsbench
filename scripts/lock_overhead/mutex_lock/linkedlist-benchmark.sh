#!/bin/bash

#cd ../../
#make clean
#make
#cd scripts/parallel/

input_path="../../../workloads/lock_overhead/"

#linkedlist-dram workload-a
n_threads=1
while [ $n_threads -le 16 ]
do
  counter=1
  file="${input_path}workloada_${n_threads}.spec"
  echo "[Benchmark] linkedlist-dram, #of_threads: " $n_threads ", workload: $file"
  while [ $counter -le 10 ]
  do
    ./../../../ycsbc -db storeds -threads $n_threads -dbpath /pmem/linkedlist.pmem -type linkedlist-dram-conc-mlock -P $file
    ((counter++))
  done
  echo "*****************<>*****************"
  ((n_threads*=2))
done
echo "~~~~~~~~~~~~~~~~~~~~~~~~~<>~~~~~~~~~~~~~~~~~~~~~~~~~"

#linkedlist-dram workload-e
n_threads=1
while [ $n_threads -le 16 ]
do
  counter=1
  file="${input_path}workloade_${n_threads}.spec"
  echo "[Benchmark] linkedlist-dram, #of_threads: " $n_threads ", workload: $file"
  while [ $counter -le 10 ]
  do
    ./../../../ycsbc -db storeds -threads $n_threads -dbpath /pmem/linkedlist.pmem -type linkedlist-dram-conc-mlock -P $file
    ((counter++))
  done
  echo "*****************<>*****************"
  ((n_threads*=2))
done
echo "~~~~~~~~~~~~~~~~~~~~~~~~~<>~~~~~~~~~~~~~~~~~~~~~~~~~"

#linkedlist-vmem workload-a
n_threads=1
while [ $n_threads -le 16 ]
do
  counter=1
  file="${input_path}workloada_${n_threads}.spec"
  echo "[Benchmark] linkedlist-vmem, #of_threads: " $n_threads ", workload: $file"
  while [ $counter -le 10 ]
  do
    ./../../../ycsbc -db storeds -threads $n_threads -dbpath /pmem -type linkedlist-vmem-conc-mlock -P $file
    ((counter++))
  done
  echo "*****************<>*****************"
  ((n_threads*=2))
done
echo "~~~~~~~~~~~~~~~~~~~~~~~~~<>~~~~~~~~~~~~~~~~~~~~~~~~~"

#linkedlist-vmem workload-e
n_threads=1
while [ $n_threads -le 16 ]
do
  counter=1
  file="${input_path}workloade_${n_threads}.spec"
  echo "[Benchmark] linkedlist-vmem, #of_threads: " $n_threads ", workload: $file"
  while [ $counter -le 10 ]
  do
    ./../../../ycsbc -db storeds -threads $n_threads -dbpath /pmem -type linkedlist-vmem-conc-mlock -P $file
    ((counter++))
  done
  echo "*****************<>*****************"
  ((n_threads*=2))
done
echo "~~~~~~~~~~~~~~~~~~~~~~~~~<>~~~~~~~~~~~~~~~~~~~~~~~~~"

#linkedlist-pmem workload-a
n_threads=1
while [ $n_threads -le 16 ]
do
  counter=1
  file="${input_path}workloada_${n_threads}.spec"
  echo "[Benchmark] linkedlist-pmem, #of_threads: " $n_threads ", workload: $file"
  while [ $counter -le 10 ]
  do
    ./../../../ycsbc -db storeds -threads $n_threads -dbpath /pmem/linkedlist.pmem -type linkedlist-pmem-conc-mlock -P $file
    ((counter++))
    rm /pmem/linkedlist.pmem
  done
  echo "*****************<>*****************"
  ((n_threads*=2))
done
echo "~~~~~~~~~~~~~~~~~~~~~~~~~<>~~~~~~~~~~~~~~~~~~~~~~~~~"

#linkedlist-pmem workload-e
n_threads=1
while [ $n_threads -le 16 ]
do
  counter=1
  file="${input_path}workloade_${n_threads}.spec"
  echo "[Benchmark] linkedlist-pmem, #of_threads: " $n_threads ", workload: $file"
  while [ $counter -le 10 ]
  do
    ./../../../ycsbc -db storeds -threads $n_threads -dbpath /pmem/linkedlist.pmem -type linkedlist-pmem-conc-mlock -P $file
    ((counter++))
    rm /pmem/linkedlist.pmem
  done
  echo "*****************<>*****************"
  ((n_threads*=2))
done
echo "~~~~~~~~~~~~~~~~~~~~~~~~~<>~~~~~~~~~~~~~~~~~~~~~~~~~"

#linkedlist-pmem-tx workload-a
n_threads=1
while [ $n_threads -le 16 ]
do
  counter=1
  file="${input_path}workloada_${n_threads}.spec"
  echo "[Benchmark] linkedlist-pmem-tx, #of_threads: " $n_threads ", workload: $file"
  while [ $counter -le 10 ]
  do
    ./../../../ycsbc -db storeds -threads $n_threads -dbpath /pmem/linkedlist.pmem -type linkedlist-pmem-tx-conc-mlock -P $file
    ((counter++))
    rm /pmem/linkedlist.pmem
  done
  echo "*****************<>*****************"
  ((n_threads*=2))
done
echo "~~~~~~~~~~~~~~~~~~~~~~~~~<>~~~~~~~~~~~~~~~~~~~~~~~~~"

#linkedlist-pmem-tx workload-e
n_threads=1
while [ $n_threads -le 16 ]
do
  counter=1
  file="${input_path}workloade_${n_threads}.spec"
  echo "[Benchmark] linkedlist-pmem-tx, #of_threads: " $n_threads ", workload: $file"
  while [ $counter -le 10 ]
  do
    ./../../../ycsbc -db storeds -threads $n_threads -dbpath /pmem/linkedlist.pmem -type linkedlist-pmem-tx-conc-mlock -P $file
    ((counter++))
    rm /pmem/linkedlist.pmem
  done
  echo "*****************<>*****************"
  ((n_threads*=2))
done
echo "~~~~~~~~~~~~~~~~~~~~~~~~~<>~~~~~~~~~~~~~~~~~~~~~~~~~"
