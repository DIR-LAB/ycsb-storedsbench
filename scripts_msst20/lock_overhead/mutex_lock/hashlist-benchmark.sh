#!/bin/bash

#cd ../../
#make clean
#make
#cd scripts/parallel/

input_path="../../../workloads/"

#hashlist-dram workload-a
n_threads=1
while [ $n_threads -le 16 ]
do
  counter=1
  file="${input_path}workloada.spec"
  echo "[Benchmark] hashlist-dram, #of_threads: " $n_threads ", workload: $file"
  while [ $counter -le 5 ]
  do
    ./../../../ycsbc -db storeds -threads $n_threads -dbpath /pmem/hl.pmem -type hl-dram-conc-mlock -P $file
    ((counter++))
  done
  echo "*****************<>*****************"
  ((n_threads*=2))
done
echo "~~~~~~~~~~~~~~~~~~~~~~~~~<>~~~~~~~~~~~~~~~~~~~~~~~~~"

#hashlist-dram workload-e
#n_threads=1
#while [ $n_threads -le 16 ]
#do
#  counter=1
#  file="${input_path}workloade_${n_threads}.spec"
#  echo "[Benchmark] hashlist-dram, #of_threads: " $n_threads ", workload: $file"
#  while [ $counter -le 5 ]
#  do
#    ./../../../ycsbc -db storeds -threads $n_threads -dbpath /pmem/hl.pmem -type hl-dram-conc-mlock -P $file
#    ((counter++))
#  done
#  echo "*****************<>*****************"
#  ((n_threads*=2))
#done
#echo "~~~~~~~~~~~~~~~~~~~~~~~~~<>~~~~~~~~~~~~~~~~~~~~~~~~~"

#hashlist-vmem workload-a
n_threads=1
while [ $n_threads -le 16 ]
do
  counter=1
  file="${input_path}workloada.spec"
  echo "[Benchmark] hashlist-vmem, #of_threads: " $n_threads ", workload: $file"
  while [ $counter -le 5 ]
  do
    ./../../../ycsbc -db storeds -threads $n_threads -dbpath /pmem -type hl-vmem-conc-mlock -P $file
    ((counter++))
  done
  echo "*****************<>*****************"
  ((n_threads*=2))
done
echo "~~~~~~~~~~~~~~~~~~~~~~~~~<>~~~~~~~~~~~~~~~~~~~~~~~~~"

#hashlist-vmem workload-e
#n_threads=1
#while [ $n_threads -le 16 ]
#do
#  counter=1
#  file="${input_path}workloade_${n_threads}.spec"
#  echo "[Benchmark] hashlist-vmem, #of_threads: " $n_threads ", workload: $file"
#  while [ $counter -le 5 ]
#  do
#    ./../../../ycsbc -db storeds -threads $n_threads -dbpath /pmem -type hl-vmem-conc-mlock -P $file
#    ((counter++))
#  done
#  echo "*****************<>*****************"
#  ((n_threads*=2))
#done
#echo "~~~~~~~~~~~~~~~~~~~~~~~~~<>~~~~~~~~~~~~~~~~~~~~~~~~~"

#hashlist-pmem workload-a
n_threads=1
while [ $n_threads -le 16 ]
do
  counter=1
  file="${input_path}workloada.spec"
  echo "[Benchmark] hashlist-pmem, #of_threads: " $n_threads ", workload: $file"
  while [ $counter -le 5 ]
  do
    ./../../../ycsbc -db storeds -threads $n_threads -dbpath /pmem/hl.pmem -type hl-pmem-conc-mlock -P $file
    ((counter++))
    rm /pmem/hl.pmem
  done
  echo "*****************<>*****************"
  ((n_threads*=2))
done
echo "~~~~~~~~~~~~~~~~~~~~~~~~~<>~~~~~~~~~~~~~~~~~~~~~~~~~"

#hashlist-pmem workload-e
#n_threads=1
#while [ $n_threads -le 16 ]
#do
#  counter=1
#  file="${input_path}workloade_${n_threads}.spec"
#  echo "[Benchmark] hashlist-pmem, #of_threads: " $n_threads ", workload: $file"
#  while [ $counter -le 5 ]
#  do
#    ./../../../ycsbc -db storeds -threads $n_threads -dbpath /pmem/hl.pmem -type hl-pmem-conc-mlock -P $file
#    ((counter++))
#    rm /pmem/hl.pmem
#  done
#  echo "*****************<>*****************"
#  ((n_threads*=2))
#done
#echo "~~~~~~~~~~~~~~~~~~~~~~~~~<>~~~~~~~~~~~~~~~~~~~~~~~~~"

#hashlist-pmem-tx workload-a
n_threads=1
while [ $n_threads -le 16 ]
do
  counter=1
  file="${input_path}workloada.spec"
  echo "[Benchmark] hashlist-pmem-tx, #of_threads: " $n_threads ", workload: $file"
  while [ $counter -le 5 ]
  do
    ./../../../ycsbc -db storeds -threads $n_threads -dbpath /pmem/hl.pmem -type hl-pmem-tx-conc-mlock -P $file
    ((counter++))
    rm /pmem/hl.pmem
  done
  echo "*****************<>*****************"
  ((n_threads*=2))
done
echo "~~~~~~~~~~~~~~~~~~~~~~~~~<>~~~~~~~~~~~~~~~~~~~~~~~~~"

#hashlist-pmem-tx workload-e
#n_threads=1
#while [ $n_threads -le 16 ]
#do
#  counter=1
#  file="${input_path}workloade_${n_threads}.spec"
#  echo "[Benchmark] hashlist-pmem-tx, #of_threads: " $n_threads ", workload: $file"
#  while [ $counter -le 5 ]
#  do
#    ./../../../ycsbc -db storeds -threads $n_threads -dbpath /pmem/hl.pmem -type hl-pmem-tx-conc-mlock -P $file
#    ((counter++))
#    rm /pmem/hl.pmem
#  done
#  echo "*****************<>*****************"
#  ((n_threads*=2))
#done
#echo "~~~~~~~~~~~~~~~~~~~~~~~~~<>~~~~~~~~~~~~~~~~~~~~~~~~~"
