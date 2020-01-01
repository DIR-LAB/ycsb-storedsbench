#!/bin/bash

#cd ../../
#make clean
#make
#cd scripts/parallel/

input_path="../../../workloads/"

#btree-dram
for file in $input_path*a.spec $input_path*e.spec; do
  n_threads=1
  while [ $n_threads -le 16 ]
  do
    counter=1
    echo "[Benchmark] btree-dram, #of_threads: " $n_threads ", workload: ${file##*/}"
    while [ $counter -le 10 ]
    do
      ./../../../ycsbc -db storeds -threads $n_threads -dbpath /pmem/btree.pmem -type btree-dram-conc-mlock -P $input_path${file##*/}
      ((counter++))
    done
    echo "*****************<>*****************"
    ((n_threads*=2))
  done
  echo "~~~~~~~~~~~~~~~~~~~~~~~~~<>~~~~~~~~~~~~~~~~~~~~~~~~~"
done

#btree-vmem
for file in $input_path*a.spec $input_path*e.spec; do
  n_threads=1
  while [ $n_threads -le 16 ]
  do
    counter=1
    echo "[Benchmark] btree-vmem, #of_threads: " $n_threads ", workload: ${file##*/}"
    while [ $counter -le 10 ]
    do
      ./../../../ycsbc -db storeds -threads $n_threads -dbpath /pmem -type btree-vmem-conc-mlock -P $input_path${file##*/}
      ((counter++))
    done
    echo "*****************<>*****************"
    ((n_threads*=2))
  done
  echo "~~~~~~~~~~~~~~~~~~~~~~~~~<>~~~~~~~~~~~~~~~~~~~~~~~~~"
done

#btree-pmem
for file in $input_path*a.spec $input_path*e.spec; do
  n_threads=1
  while [ $n_threads -le 16 ]
  do
    counter=1
    echo "[Benchmark] btree-pmem, #of_threads: " $n_threads ", workload: ${file##*/}"
    while [ $counter -le 10 ]
    do
      ./../../../ycsbc -db storeds -threads $n_threads -dbpath /pmem/btree.pmem -type btree-pmem-conc-mlock -P $input_path${file##*/}
      ((counter++))
      rm /pmem/btree.pmem
    done
    echo "*****************<>*****************"
    ((n_threads*=2))
  done
  echo "~~~~~~~~~~~~~~~~~~~~~~~~~<>~~~~~~~~~~~~~~~~~~~~~~~~~"
done

#btree-pmem-tx
for file in $input_path*a.spec $input_path*e.spec; do
  n_threads=1
  while [ $n_threads -le 16 ]
  do
    counter=1
    echo "[Benchmark] btree-pmem-tx, #of_threads: " $n_threads ", workload: ${file##*/}"
    while [ $counter -le 10 ]
    do
      ./../../../ycsbc -db storeds -threads $n_threads -dbpath /pmem/btree.pmem -type btree-pmem-tx-conc-mlock -P $input_path${file##*/}
      ((counter++))
      rm /pmem/btree.pmem
    done
    echo "*****************<>*****************"
    ((n_threads*=2))
  done
  echo "~~~~~~~~~~~~~~~~~~~~~~~~~<>~~~~~~~~~~~~~~~~~~~~~~~~~"
done
