#!/bin/bash

#cd ../../
#make clean
#make
#cd scripts/parallel/

input_path="../../../workloads/"

#rbtree-dram workload-a
n_threads=1
while [ $n_threads -le 16 ]
do
  counter=1
  file="${input_path}workloada.spec"
  echo "[Benchmark] rbtree-dram, #of_threads: " $n_threads ", workload: $file"
  while [ $counter -le 10 ]
  do
    ./../../../ycsbc -db storeds -threads $n_threads -dbpath /pmem/rbtree.pmem -type rbtree-dram-conc-mlock -P $file
    ((counter++))
  done
  echo "*****************<>*****************"
  ((n_threads*=2))
done
echo "~~~~~~~~~~~~~~~~~~~~~~~~~<>~~~~~~~~~~~~~~~~~~~~~~~~~"

#rbtree-dram workload-e
#n_threads=1
#while [ $n_threads -le 16 ]
#do
#  counter=1
#  file="${input_path}workloade_${n_threads}.spec"
#  echo "[Benchmark] rbtree-dram, #of_threads: " $n_threads ", workload: $file"
#  while [ $counter -le 10 ]
#  do
#    ./../../../ycsbc -db storeds -threads $n_threads -dbpath /pmem/rbtree.pmem -type rbtree-dram-conc-mlock -P $file
#    ((counter++))
#  done
#  echo "*****************<>*****************"
#  ((n_threads*=2))
#done
#echo "~~~~~~~~~~~~~~~~~~~~~~~~~<>~~~~~~~~~~~~~~~~~~~~~~~~~"

#rbtree-vmem workload-a
n_threads=1
while [ $n_threads -le 16 ]
do
  counter=1
  file="${input_path}workloada.spec"
  echo "[Benchmark] rbtree-vmem, #of_threads: " $n_threads ", workload: $file"
  while [ $counter -le 10 ]
  do
    ./../../../ycsbc -db storeds -threads $n_threads -dbpath /pmem -type rbtree-vmem-conc-mlock -P $file
    ((counter++))
  done
  echo "*****************<>*****************"
  ((n_threads*=2))
done
echo "~~~~~~~~~~~~~~~~~~~~~~~~~<>~~~~~~~~~~~~~~~~~~~~~~~~~"

#rbtree-vmem workload-e
#n_threads=1
#while [ $n_threads -le 16 ]
#do
#  counter=1
#  file="${input_path}workloade_${n_threads}.spec"
#  echo "[Benchmark] rbtree-vmem, #of_threads: " $n_threads ", workload: $file"
#  while [ $counter -le 10 ]
#  do
#    ./../../../ycsbc -db storeds -threads $n_threads -dbpath /pmem -type rbtree-vmem-conc-mlock -P $file
#    ((counter++))
#  done
#  echo "*****************<>*****************"
#  ((n_threads*=2))
#done
#echo "~~~~~~~~~~~~~~~~~~~~~~~~~<>~~~~~~~~~~~~~~~~~~~~~~~~~"

#rbtree-pmem workload-a
n_threads=1
while [ $n_threads -le 16 ]
do
  counter=1
  file="${input_path}workloada.spec"
  echo "[Benchmark] rbtree-pmem, #of_threads: " $n_threads ", workload: $file"
  while [ $counter -le 10 ]
  do
    ./../../../ycsbc -db storeds -threads $n_threads -dbpath /pmem/rbtree.pmem -type rbtree-pmem-conc-mlock -P $file
    ((counter++))
    rm /pmem/rbtree.pmem
  done
  echo "*****************<>*****************"
  ((n_threads*=2))
done
echo "~~~~~~~~~~~~~~~~~~~~~~~~~<>~~~~~~~~~~~~~~~~~~~~~~~~~"

#rbtree-pmem workload-e
#n_threads=1
#while [ $n_threads -le 16 ]
#do
#  counter=1
#  file="${input_path}workloade_${n_threads}.spec"
#  echo "[Benchmark] rbtree-pmem, #of_threads: " $n_threads ", workload: $file"
#  while [ $counter -le 10 ]
#  do
#    ./../../../ycsbc -db storeds -threads $n_threads -dbpath /pmem/rbtree.pmem -type rbtree-pmem-conc-mlock -P $file
#    ((counter++))
#    rm /pmem/rbtree.pmem
#  done
#  echo "*****************<>*****************"
#  ((n_threads*=2))
#done
#echo "~~~~~~~~~~~~~~~~~~~~~~~~~<>~~~~~~~~~~~~~~~~~~~~~~~~~"

#rbtree-pmem-tx workload-a
n_threads=1
while [ $n_threads -le 16 ]
do
  counter=1
  file="${input_path}workloada.spec"
  echo "[Benchmark] rbtree-pmem-tx, #of_threads: " $n_threads ", workload: $file"
  while [ $counter -le 10 ]
  do
    ./../../../ycsbc -db storeds -threads $n_threads -dbpath /pmem/rbtree.pmem -type rbtree-pmem-tx-conc-mlock -P $file
    ((counter++))
    rm /pmem/rbtree.pmem
  done
  echo "*****************<>*****************"
  ((n_threads*=2))
done
echo "~~~~~~~~~~~~~~~~~~~~~~~~~<>~~~~~~~~~~~~~~~~~~~~~~~~~"

#rbtree-pmem-tx workload-e
#n_threads=1
#while [ $n_threads -le 16 ]
#do
#  counter=1
#  file="${input_path}workloade_${n_threads}.spec"
#  echo "[Benchmark] rbtree-pmem-tx, #of_threads: " $n_threads ", workload: $file"
#  while [ $counter -le 10 ]
#  do
#    ./../../../ycsbc -db storeds -threads $n_threads -dbpath /pmem/rbtree.pmem -type rbtree-pmem-tx-conc-mlock -P $file
#    ((counter++))
#    rm /pmem/rbtree.pmem
#  done
#  echo "*****************<>*****************"
#  ((n_threads*=2))
#done
#echo "~~~~~~~~~~~~~~~~~~~~~~~~~<>~~~~~~~~~~~~~~~~~~~~~~~~~"
