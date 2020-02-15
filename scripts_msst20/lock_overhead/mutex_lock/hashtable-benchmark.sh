#!/bin/bash

#cd ../../
#make clean
#make
#cd scripts/parallel/

input_path="../../../workloads/"

#hashtable-dram workload-a
n_threads=1
while [ $n_threads -le 16 ]
do
  counter=1
  file="${input_path}workloada.spec"
  echo "[Benchmark] hashtable-dram, #of_threads: " $n_threads ", workload: $file"
  while [ $counter -le 10 ]
  do
    ./../../../ycsbc -db storeds -threads $n_threads -dbpath /pmem/hashtable.pmem -type hashtable-dram-conc-mlock -P $file
    ((counter++))
  done
  echo "*****************<>*****************"
  ((n_threads*=2))
done
echo "~~~~~~~~~~~~~~~~~~~~~~~~~<>~~~~~~~~~~~~~~~~~~~~~~~~~"

#hashtable-dram workload-e
#n_threads=1
#while [ $n_threads -le 16 ]
#do
#  counter=1
#  file="${input_path}workloade_${n_threads}.spec"
#  echo "[Benchmark] hashtable-dram, #of_threads: " $n_threads ", workload: $file"
#  while [ $counter -le 10 ]
#  do
#    ./../../../ycsbc -db storeds -threads $n_threads -dbpath /pmem/hashtable.pmem -type hashtable-dram-conc-mlock -P $file
#    ((counter++))
#  done
#  echo "*****************<>*****************"
#  ((n_threads*=2))
#done
#echo "~~~~~~~~~~~~~~~~~~~~~~~~~<>~~~~~~~~~~~~~~~~~~~~~~~~~"

#hashtable-vmem workload-a
n_threads=1
while [ $n_threads -le 16 ]
do
  counter=1
  file="${input_path}workloada.spec"
  echo "[Benchmark] hashtable-vmem, #of_threads: " $n_threads ", workload: $file"
  while [ $counter -le 10 ]
  do
    ./../../../ycsbc -db storeds -threads $n_threads -dbpath /pmem -type hashtable-vmem-conc-mlock -P $file
    ((counter++))
  done
  echo "*****************<>*****************"
  ((n_threads*=2))
done
echo "~~~~~~~~~~~~~~~~~~~~~~~~~<>~~~~~~~~~~~~~~~~~~~~~~~~~"

#hashtable-vmem workload-e
#n_threads=1
#while [ $n_threads -le 16 ]
#do
#  counter=1
#  file="${input_path}workloade_${n_threads}.spec"
#  echo "[Benchmark] hashtable-vmem, #of_threads: " $n_threads ", workload: $file"
#  while [ $counter -le 10 ]
#  do
#    ./../../../ycsbc -db storeds -threads $n_threads -dbpath /pmem -type hashtable-vmem-conc-mlock -P $file
#    ((counter++))
#  done
#  echo "*****************<>*****************"
#  ((n_threads*=2))
#done
#echo "~~~~~~~~~~~~~~~~~~~~~~~~~<>~~~~~~~~~~~~~~~~~~~~~~~~~"

#hashtable-pmem workload-a
n_threads=1
while [ $n_threads -le 16 ]
do
  counter=1
  file="${input_path}workloada.spec"
  echo "[Benchmark] hashtable-pmem, #of_threads: " $n_threads ", workload: $file"
  while [ $counter -le 10 ]
  do
    ./../../../ycsbc -db storeds -threads $n_threads -dbpath /pmem/hashtable.pmem -type hashtable-pmem-conc-mlock -P $file
    ((counter++))
    rm /pmem/hashtable.pmem
  done
  echo "*****************<>*****************"
  ((n_threads*=2))
done
echo "~~~~~~~~~~~~~~~~~~~~~~~~~<>~~~~~~~~~~~~~~~~~~~~~~~~~"

#hashtable-pmem workload-e
#n_threads=1
#while [ $n_threads -le 16 ]
#do
#  counter=1
#  file="${input_path}workloade_${n_threads}.spec"
#  echo "[Benchmark] hashtable-pmem, #of_threads: " $n_threads ", workload: $file"
#  while [ $counter -le 10 ]
#  do
#    ./../../../ycsbc -db storeds -threads $n_threads -dbpath /pmem/hashtable.pmem -type hashtable-pmem-conc-mlock -P $file
#    ((counter++))
#    rm /pmem/hashtable.pmem
#  done
#  echo "*****************<>*****************"
#  ((n_threads*=2))
#done
#echo "~~~~~~~~~~~~~~~~~~~~~~~~~<>~~~~~~~~~~~~~~~~~~~~~~~~~"

#hashtable-pmem-tx workload-a
n_threads=1
while [ $n_threads -le 16 ]
do
  counter=1
  file="${input_path}workloada.spec"
  echo "[Benchmark] hashtable-pmem-tx, #of_threads: " $n_threads ", workload: $file"
  while [ $counter -le 10 ]
  do
    ./../../../ycsbc -db storeds -threads $n_threads -dbpath /pmem/hashtable.pmem -type hashtable-pmem-tx-conc-mlock -P $file
    ((counter++))
    rm /pmem/hashtable.pmem
  done
  echo "*****************<>*****************"
  ((n_threads*=2))
done
echo "~~~~~~~~~~~~~~~~~~~~~~~~~<>~~~~~~~~~~~~~~~~~~~~~~~~~"

#hashtable-pmem-tx workload-e
#n_threads=1
#while [ $n_threads -le 16 ]
#do
#  counter=1
#  file="${input_path}workloade_${n_threads}.spec"
#  echo "[Benchmark] hashtable-pmem-tx, #of_threads: " $n_threads ", workload: $file"
#  while [ $counter -le 10 ]
#  do
#    ./../../../ycsbc -db storeds -threads $n_threads -dbpath /pmem/hashtable.pmem -type hashtable-pmem-tx-conc-mlock -P $file
#    ((counter++))
#    rm /pmem/hashtable.pmem
#  done
#  echo "*****************<>*****************"
#  ((n_threads*=2))
#done
#echo "~~~~~~~~~~~~~~~~~~~~~~~~~<>~~~~~~~~~~~~~~~~~~~~~~~~~"
