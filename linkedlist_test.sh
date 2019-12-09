#!/bin/bash

make clean
make

#linkedlist-dram
echo "[Benchmark] linkedlist-dram"
echo "a"
./ycsbc -db storeds -threads 1 -dbpath /pmem/linkedlist.pmem -type linkedlist-dram -P workloads/workloada.spec
./ycsbc -db storeds -threads 4 -dbpath /pmem/linkedlist.pmem -type linkedlist-dram -P workloads/workloada.spec
./ycsbc -db storeds -threads 8 -dbpath /pmem/linkedlist.pmem -type linkedlist-dram -P workloads/workloada.spec
./ycsbc -db storeds -threads 12 -dbpath /pmem/linkedlist.pmem -type linkedlist-dram -P workloads/workloada.spec
echo "b"
./ycsbc -db storeds -threads 1 -dbpath /pmem/linkedlist.pmem -type linkedlist-dram -P workloads/workloadb.spec
./ycsbc -db storeds -threads 4 -dbpath /pmem/linkedlist.pmem -type linkedlist-dram -P workloads/workloadb.spec
./ycsbc -db storeds -threads 8 -dbpath /pmem/linkedlist.pmem -type linkedlist-dram -P workloads/workloadb.spec
./ycsbc -db storeds -threads 12 -dbpath /pmem/linkedlist.pmem -type linkedlist-dram -P workloads/workloadb.spec
echo "c"
./ycsbc -db storeds -threads 1 -dbpath /pmem/linkedlist.pmem -type linkedlist-dram -P workloads/workloadb.spec
./ycsbc -db storeds -threads 4 -dbpath /pmem/linkedlist.pmem -type linkedlist-dram -P workloads/workloadc.spec
./ycsbc -db storeds -threads 8 -dbpath /pmem/linkedlist.pmem -type linkedlist-dram -P workloads/workloadc.spec
./ycsbc -db storeds -threads 12 -dbpath /pmem/linkedlist.pmem -type linkedlist-dram -P workloads/workloadc.spec
echo "d"
./ycsbc -db storeds -threads 1 -dbpath /pmem/linkedlist.pmem -type linkedlist-dram -P workloads/workloadd.spec
./ycsbc -db storeds -threads 4 -dbpath /pmem/linkedlist.pmem -type linkedlist-dram -P workloads/workloadd.spec
./ycsbc -db storeds -threads 8 -dbpath /pmem/linkedlist.pmem -type linkedlist-dram -P workloads/workloadd.spec
./ycsbc -db storeds -threads 12 -dbpath /pmem/linkedlist.pmem -type linkedlist-dram -P workloads/workloadd.spec
#./ycsbc -db storeds -threads 1 -dbpath /pmem/hashtable.pmem -type hashtable-dram -P workloads/workloade.spec
echo "f"
./ycsbc -db storeds -threads 1 -dbpath /pmem/linkedlist.pmem -type linkedlist-dram -P workloads/workloadf.spec
./ycsbc -db storeds -threads 4 -dbpath /pmem/linkedlist.pmem -type linkedlist-dram -P workloads/workloadf.spec
./ycsbc -db storeds -threads 8 -dbpath /pmem/linkedlist.pmem -type linkedlist-dram -P workloads/workloadf.spec
./ycsbc -db storeds -threads 12 -dbpath /pmem/linkedlist.pmem -type linkedlist-dram -P workloads/workloadf.spec

echo "*****************<>*****************"

#linkedlist-pmem
echo "[Benchmark] linkedlist-pmem"
echo "a"
#hashtable-dram
echo "[Benchmark] linkedlist-dram"
echo "a"
./ycsbc -db storeds -threads 1 -dbpath /pmem/linkedlist.pmem -type linkedlist-pmem -P workloads/workloada.spec
./ycsbc -db storeds -threads 4 -dbpath /pmem/linkedlist.pmem -type linkedlist-pmem -P workloads/workloada.spec
./ycsbc -db storeds -threads 8 -dbpath /pmem/linkedlist.pmem -type linkedlist-pmem -P workloads/workloada.spec
./ycsbc -db storeds -threads 12 -dbpath /pmem/linkedlist.pmem -type linkedlist-pmem -P workloads/workloada.spec
echo "b"
./ycsbc -db storeds -threads 1 -dbpath /pmem/linkedlist.pmem -type linkedlist-pmem -P workloads/workloadb.spec
./ycsbc -db storeds -threads 4 -dbpath /pmem/linkedlist.pmem -type linkedlist-pmem -P workloads/workloadb.spec
./ycsbc -db storeds -threads 8 -dbpath /pmem/linkedlist.pmem -type linkedlist-pmem -P workloads/workloadb.spec
./ycsbc -db storeds -threads 12 -dbpath /pmem/linkedlist.pmem -type linkedlist-pmem -P workloads/workloadb.spec
echo "c"
./ycsbc -db storeds -threads 1 -dbpath /pmem/linkedlist.pmem -type linkedlist-pmem -P workloads/workloadb.spec
./ycsbc -db storeds -threads 4 -dbpath /pmem/linkedlist.pmem -type linkedlist-pmem -P workloads/workloadc.spec
./ycsbc -db storeds -threads 8 -dbpath /pmem/linkedlist.pmem -type linkedlist-pmem -P workloads/workloadc.spec
./ycsbc -db storeds -threads 12 -dbpath /pmem/linkedlist.pmem -type linkedlist-pmem -P workloads/workloadc.spec
echo "d"
./ycsbc -db storeds -threads 1 -dbpath /pmem/linkedlist.pmem -type linkedlist-pmem -P workloads/workloadd.spec
./ycsbc -db storeds -threads 4 -dbpath /pmem/linkedlist.pmem -type linkedlist-pmem -P workloads/workloadd.spec
./ycsbc -db storeds -threads 8 -dbpath /pmem/linkedlist.pmem -type linkedlist-pmem -P workloads/workloadd.spec
./ycsbc -db storeds -threads 12 -dbpath /pmem/linkedlist.pmem -type linkedlist-pmem -P workloads/workloadd.spec
#./ycsbc -db storeds -threads 1 -dbpath /pmem/hashtable.pmem -type hashtable-dram -P workloads/workloade.spec
echo "f"
./ycsbc -db storeds -threads 1 -dbpath /pmem/linkedlist.pmem -type linkedlist-pmem -P workloads/workloadf.spec
./ycsbc -db storeds -threads 4 -dbpath /pmem/linkedlist.pmem -type linkedlist-pmem -P workloads/workloadf.spec
./ycsbc -db storeds -threads 8 -dbpath /pmem/linkedlist.pmem -type linkedlist-pmem -P workloads/workloadf.spec
./ycsbc -db storeds -threads 12 -dbpath /pmem/linkedlist.pmem -type linkedlist-pmem -P workloads/workloadf.spec
echo "*****************<>*****************"
