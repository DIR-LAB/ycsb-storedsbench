#!/bin/bash

#cd ../../
#make clean
#make
#cd scripts/parallel/

input_path="../../../workloads/"

#bp-dram workload-a
n_threads=1
while [ $n_threads -le 16 ]
do
  counter=1
  file="${input_path}workloada.spec"
  echo "[Benchmark] bp-dram, #of_threads: " $n_threads ", workload: $file"
  while [ $counter -le 10 ]
  do
    ./../../../ycsbc -db storeds -threads $n_threads -dbpath /pmem/bp.pmem -type bp-dram-conc-mlock -P $file
    ((counter++))
  done
  echo "*****************<>*****************"
  ((n_threads*=2))
done
echo "~~~~~~~~~~~~~~~~~~~~~~~~~<>~~~~~~~~~~~~~~~~~~~~~~~~~"

#bp-dram workload-e
#n_threads=1
#while [ $n_threads -le 16 ]
#do
#  counter=1
#  file="${input_path}workloade_${n_threads}.spec"
#  echo "[Benchmark] bp-dram, #of_threads: " $n_threads ", workload: $file"
#  while [ $counter -le 10 ]
#  do
#    ./../../../ycsbc -db storeds -threads $n_threads -dbpath /pmem/bp.pmem -type bp-dram-conc-mlock -P $file
#    ((counter++))
#  done
#  echo "*****************<>*****************"
#  ((n_threads*=2))
#done
#echo "~~~~~~~~~~~~~~~~~~~~~~~~~<>~~~~~~~~~~~~~~~~~~~~~~~~~"

#bp-vmem workload-a
n_threads=1
while [ $n_threads -le 16 ]
do
  counter=1
  file="${input_path}workloada.spec"
  echo "[Benchmark] bp-vmem, #of_threads: " $n_threads ", workload: $file"
  while [ $counter -le 10 ]
  do
    ./../../../ycsbc -db storeds -threads $n_threads -dbpath /pmem -type bp-vmem-conc-mlock -P $file
    ((counter++))
  done
  echo "*****************<>*****************"
  ((n_threads*=2))
done
echo "~~~~~~~~~~~~~~~~~~~~~~~~~<>~~~~~~~~~~~~~~~~~~~~~~~~~"

#bp-vmem workload-e
#n_threads=1
#while [ $n_threads -le 16 ]
#do
#  counter=1
#  file="${input_path}workloade_${n_threads}.spec"
#  echo "[Benchmark] bp-vmem, #of_threads: " $n_threads ", workload: $file"
#  while [ $counter -le 10 ]
#  do
#    ./../../../ycsbc -db storeds -threads $n_threads -dbpath /pmem -type bp-vmem-conc-mlock -P $file
#    ((counter++))
#  done
#  echo "*****************<>*****************"
#  ((n_threads*=2))
#done
#echo "~~~~~~~~~~~~~~~~~~~~~~~~~<>~~~~~~~~~~~~~~~~~~~~~~~~~"

#bp-pmem workload-a
n_threads=1
while [ $n_threads -le 16 ]
do
  counter=1
  file="${input_path}workloada.spec"
  echo "[Benchmark] bp-pmem, #of_threads: " $n_threads ", workload: $file"
  while [ $counter -le 10 ]
  do
    ./../../../ycsbc -db storeds -threads $n_threads -dbpath /pmem/bp.pmem -type bp-pmem-conc-mlock -P $file
    ((counter++))
    rm /pmem/bp.pmem
  done
  echo "*****************<>*****************"
  ((n_threads*=2))
done
echo "~~~~~~~~~~~~~~~~~~~~~~~~~<>~~~~~~~~~~~~~~~~~~~~~~~~~"

#bp-pmem workload-e
#n_threads=1
#while [ $n_threads -le 16 ]
#do
#  counter=1
#  file="${input_path}workloade_${n_threads}.spec"
#  echo "[Benchmark] bp-pmem, #of_threads: " $n_threads ", workload: $file"
#  while [ $counter -le 10 ]
#  do
#    ./../../../ycsbc -db storeds -threads $n_threads -dbpath /pmem/bp.pmem -type bp-pmem-conc-mlock -P $file
#    ((counter++))
#    rm /pmem/bp.pmem
#  done
#  echo "*****************<>*****************"
#  ((n_threads*=2))
#done
#echo "~~~~~~~~~~~~~~~~~~~~~~~~~<>~~~~~~~~~~~~~~~~~~~~~~~~~"

#bp-pmem-tx workload-a
n_threads=1
while [ $n_threads -le 16 ]
do
  counter=1
  file="${input_path}workloada.spec"
  echo "[Benchmark] bp-pmem-tx, #of_threads: " $n_threads ", workload: $file"
  while [ $counter -le 10 ]
  do
    ./../../../ycsbc -db storeds -threads $n_threads -dbpath /pmem/bp.pmem -type bp-pmem-tx-conc-mlock -P $file
    ((counter++))
    rm /pmem/bp.pmem
  done
  echo "*****************<>*****************"
  ((n_threads*=2))
done
echo "~~~~~~~~~~~~~~~~~~~~~~~~~<>~~~~~~~~~~~~~~~~~~~~~~~~~"

#bp-pmem-tx workload-e
#n_threads=1
#while [ $n_threads -le 16 ]
#do
#  counter=1
#  file="${input_path}workloade_${n_threads}.spec"
#  echo "[Benchmark] bp-pmem-tx, #of_threads: " $n_threads ", workload: $file"
#  while [ $counter -le 10 ]
#  do
#    ./../../../ycsbc -db storeds -threads $n_threads -dbpath /pmem/bp.pmem -type bp-pmem-tx-conc-mlock -P $file
#    ((counter++))
#    rm /pmem/bp.pmem
#  done
#  echo "*****************<>*****************"
#  ((n_threads*=2))
#done
#echo "~~~~~~~~~~~~~~~~~~~~~~~~~<>~~~~~~~~~~~~~~~~~~~~~~~~~"
