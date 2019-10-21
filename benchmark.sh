#!/bin/bash

make clean
make

#array-dram
echo "[Benchmark] array-dram"
./ycsbc -db storeds -threads 1 -dbpath /pmem/array.pmem -type array-dram -P workloads/workloada.spec
./ycsbc -db storeds -threads 1 -dbpath /pmem/array.pmem -type array-dram -P workloads/workloadb.spec
./ycsbc -db storeds -threads 1 -dbpath /pmem/array.pmem -type array-dram -P workloads/workloadc.spec
./ycsbc -db storeds -threads 1 -dbpath /pmem/array.pmem -type array-dram -P workloads/workloadd.spec
#./ycsbc -db storeds -threads 1 -dbpath /pmem/array.pmem -type array-dram -P workloads/workloade.spec
./ycsbc -db storeds -threads 1 -dbpath /pmem/array.pmem -type array-dram -P workloads/workloadf.spec
echo "*****************<>*****************"

#array-pmem
echo "[Benchmark] array-pmem"
./ycsbc -db storeds -threads 1 -dbpath /pmem/array.pmem -type array-pmem -P workloads/workloada.spec
./ycsbc -db storeds -threads 1 -dbpath /pmem/array.pmem -type array-pmem -P workloads/workloadb.spec
./ycsbc -db storeds -threads 1 -dbpath /pmem/array.pmem -type array-pmem -P workloads/workloadc.spec
./ycsbc -db storeds -threads 1 -dbpath /pmem/array.pmem -type array-pmem -P workloads/workloadd.spec
#./ycsbc -db storeds -threads 1 -dbpath /pmem/array.pmem -type array-pmem -P workloads/workloade.spec
./ycsbc -db storeds -threads 1 -dbpath /pmem/array.pmem -type array-pmem -P workloads/workloadf.spec
echo "*****************<>*****************"

#array-pmem-tx
echo "[Benchmark] array-pmem-tx"
./ycsbc -db storeds -threads 1 -dbpath /pmem/array.pmem -type array-pmem-tx -P workloads/workloada.spec
./ycsbc -db storeds -threads 1 -dbpath /pmem/array.pmem -type array-pmem-tx -P workloads/workloadb.spec
./ycsbc -db storeds -threads 1 -dbpath /pmem/array.pmem -type array-pmem-tx -P workloads/workloadc.spec
./ycsbc -db storeds -threads 1 -dbpath /pmem/array.pmem -type array-pmem-tx -P workloads/workloadd.spec
#./ycsbc -db storeds -threads 1 -dbpath /pmem/array.pmem -type array-pmem-tx -P workloads/workloade.spec
./ycsbc -db storeds -threads 1 -dbpath /pmem/array.pmem -type array-pmem-tx -P workloads/workloadf.spec
echo "*****************<>*****************"

#hashtable-dram
echo "[Benchmark] hashtable-dram"
./ycsbc -db storeds -threads 1 -dbpath /pmem/hashtable.pmem -type hashtable-dram -P workloads/workloada.spec
./ycsbc -db storeds -threads 1 -dbpath /pmem/hashtable.pmem -type hashtable-dram -P workloads/workloadb.spec
./ycsbc -db storeds -threads 1 -dbpath /pmem/hashtable.pmem -type hashtable-dram -P workloads/workloadc.spec
./ycsbc -db storeds -threads 1 -dbpath /pmem/hashtable.pmem -type hashtable-dram -P workloads/workloadd.spec
#./ycsbc -db storeds -threads 1 -dbpath /pmem/hashtable.pmem -type hashtable-dram -P workloads/workloade.spec
./ycsbc -db storeds -threads 1 -dbpath /pmem/hashtable.pmem -type hashtable-dram -P workloads/workloadf.spec
echo "*****************<>*****************"

#hashtable-pmem
echo "[Benchmark] hashtable-pmem"
./ycsbc -db storeds -threads 1 -dbpath /pmem/hashtable.pmem -type hashtable-pmem -P workloads/workloada.spec
./ycsbc -db storeds -threads 1 -dbpath /pmem/hashtable.pmem -type hashtable-pmem -P workloads/workloadb.spec
./ycsbc -db storeds -threads 1 -dbpath /pmem/hashtable.pmem -type hashtable-pmem -P workloads/workloadc.spec
./ycsbc -db storeds -threads 1 -dbpath /pmem/hashtable.pmem -type hashtable-pmem -P workloads/workloadd.spec
# ./ycsbc -db storeds -threads 1 -dbpath /pmem/hashtable.pmem -type hashtable-pmem -P workloads/workloade.spec
./ycsbc -db storeds -threads 1 -dbpath /pmem/hashtable.pmem -type hashtable-pmem -P workloads/workloadf.spec
echo "*****************<>*****************"

#hashtable-pmem-tx
echo "[Benchmark] hashtable-pmem-tx"
./ycsbc -db storeds -threads 1 -dbpath /pmem/hashtable.pmem -type hashtable-pmem-tx -P workloads/workloada.spec
./ycsbc -db storeds -threads 1 -dbpath /pmem/hashtable.pmem -type hashtable-pmem-tx -P workloads/workloadb.spec
./ycsbc -db storeds -threads 1 -dbpath /pmem/hashtable.pmem -type hashtable-pmem-tx -P workloads/workloadc.spec
./ycsbc -db storeds -threads 1 -dbpath /pmem/hashtable.pmem -type hashtable-pmem-tx -P workloads/workloadd.spec
#./ycsbc -db storeds -threads 1 -dbpath /pmem/hashtable.pmem -type hashtable-pmem-tx -P workloads/workloade.spec
./ycsbc -db storeds -threads 1 -dbpath /pmem/hashtable.pmem -type hashtable-pmem-tx -P workloads/workloadf.spec
echo "*****************<>*****************"

#linkedlist-dram
echo "[Benchmark] linkedlist-dram"
./ycsbc -db storeds -threads 1 -dbpath /pmem/linkedlist.pmem -type linkedlist-dram -P workloads/workloada.spec
./ycsbc -db storeds -threads 1 -dbpath /pmem/linkedlist.pmem -type linkedlist-dram -P workloads/workloadb.spec
./ycsbc -db storeds -threads 1 -dbpath /pmem/linkedlist.pmem -type linkedlist-dram -P workloads/workloadc.spec
./ycsbc -db storeds -threads 1 -dbpath /pmem/linkedlist.pmem -type linkedlist-dram -P workloads/workloadd.spec
# ./ycsbc -db storeds -threads 1 -dbpath /pmem/linkedlist.pmem -type linkedlist-dram -P workloads/workloade.spec
./ycsbc -db storeds -threads 1 -dbpath /pmem/linkedlist.pmem -type linkedlist-dram -P workloads/workloadf.spec
echo "*****************<>*****************"

#linkedlist-pmem
echo "[Benchmark] linkedlist-pmem"
./ycsbc -db storeds -threads 1 -dbpath /pmem/linkedlist.pmem -type linkedlist-pmem -P workloads/workloada.spec
./ycsbc -db storeds -threads 1 -dbpath /pmem/linkedlist.pmem -type linkedlist-pmem -P workloads/workloadb.spec
./ycsbc -db storeds -threads 1 -dbpath /pmem/linkedlist.pmem -type linkedlist-pmem -P workloads/workloadc.spec
./ycsbc -db storeds -threads 1 -dbpath /pmem/linkedlist.pmem -type linkedlist-pmem -P workloads/workloadd.spec
# ./ycsbc -db storeds -threads 1 -dbpath /pmem/linkedlist.pmem -type linkedlist-pmem -P workloads/workloade.spec
./ycsbc -db storeds -threads 1 -dbpath /pmem/linkedlist.pmem -type linkedlist-pmem -P workloads/workloadf.spec
echo "*****************<>*****************"

#linkedlist-pmem
echo "[Benchmark] linkedlist-pmem-tx"
./ycsbc -db storeds -threads 1 -dbpath /pmem/linkedlist.pmem -type linkedlist-pmem-tx -P workloads/workloada.spec
./ycsbc -db storeds -threads 1 -dbpath /pmem/linkedlist.pmem -type linkedlist-pmem-tx -P workloads/workloadb.spec
./ycsbc -db storeds -threads 1 -dbpath /pmem/linkedlist.pmem -type linkedlist-pmem-tx -P workloads/workloadc.spec
./ycsbc -db storeds -threads 1 -dbpath /pmem/linkedlist.pmem -type linkedlist-pmem-tx -P workloads/workloadd.spec
# ./ycsbc -db storeds -threads 1 -dbpath /pmem/linkedlist.pmem -type linkedlist-pmem-tx -P workloads/workloade.spec
./ycsbc -db storeds -threads 1 -dbpath /pmem/linkedlist.pmem -type linkedlist-pmem-tx -P workloads/workloadf.spec
echo "*****************<>*****************"