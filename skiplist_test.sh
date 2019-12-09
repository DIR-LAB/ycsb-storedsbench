#!/bin/bash

make clean
make

#skiplist-dram
echo "[Benchmark] skiplist-dram"
echo "a"
./ycsbc -db storeds -threads 1 -dbpath /pmem/skiplist.pmem -type skiplist-dram -P workloads/workloada.spec
./ycsbc -db storeds -threads 4 -dbpath /pmem/skiplist.pmem -type skiplist-dram -P workloads/workloada.spec
./ycsbc -db storeds -threads 8 -dbpath /pmem/skiplist.pmem -type skiplist-dram -P workloads/workloada.spec
./ycsbc -db storeds -threads 12 -dbpath /pmem/skiplist.pmem -type skiplist-dram -P workloads/workloada.spec
echo "b"
./ycsbc -db storeds -threads 1 -dbpath /pmem/skiplist.pmem -type skiplist-dram -P workloads/workloadb.spec
./ycsbc -db storeds -threads 4 -dbpath /pmem/skiplist.pmem -type skiplist-dram -P workloads/workloadb.spec
./ycsbc -db storeds -threads 8 -dbpath /pmem/skiplist.pmem -type skiplist-dram -P workloads/workloadb.spec
./ycsbc -db storeds -threads 12 -dbpath /pmem/skiplist.pmem -type skiplist-dram -P workloads/workloadb.spec
echo "c"
./ycsbc -db storeds -threads 1 -dbpath /pmem/skiplist.pmem -type skiplist-dram -P workloads/workloadb.spec
./ycsbc -db storeds -threads 4 -dbpath /pmem/skiplist.pmem -type skiplist-dram -P workloads/workloadc.spec
./ycsbc -db storeds -threads 8 -dbpath /pmem/skiplist.pmem -type skiplist-dram -P workloads/workloadc.spec
./ycsbc -db storeds -threads 12 -dbpath /pmem/skiplist.pmem -type skiplist-dram -P workloads/workloadc.spec
echo "d"
./ycsbc -db storeds -threads 1 -dbpath /pmem/skiplist.pmem -type skiplist-dram -P workloads/workloadd.spec
./ycsbc -db storeds -threads 4 -dbpath /pmem/skiplist.pmem -type skiplist-dram -P workloads/workloadd.spec
./ycsbc -db storeds -threads 8 -dbpath /pmem/skiplist.pmem -type skiplist-dram -P workloads/workloadd.spec
./ycsbc -db storeds -threads 12 -dbpath /pmem/skiplist.pmem -type skiplist-dram -P workloads/workloadd.spec
#./ycsbc -db storeds -threads 1 -dbpath /pmem/hashtable.pmem -type hashtable-dram -P workloads/workloade.spec
echo "f"
./ycsbc -db storeds -threads 1 -dbpath /pmem/skiplist.pmem -type skiplist-dram -P workloads/workloadf.spec
./ycsbc -db storeds -threads 4 -dbpath /pmem/skiplist.pmem -type skiplist-dram -P workloads/workloadf.spec
./ycsbc -db storeds -threads 8 -dbpath /pmem/skiplist.pmem -type skiplist-dram -P workloads/workloadf.spec
./ycsbc -db storeds -threads 12 -dbpath /pmem/skiplist.pmem -type skiplist-dram -P workloads/workloadf.spec

echo "*****************<>*****************"

#hashtable-pmem
echo "[Benchmark] skiplist-pmem"
echo "a"
sudo ./ycsbc -db storeds -threads 1 -dbpath /pmem/skiplist.pmem -type skiplist-pmem -P workloads/workloada.spec
sudo ./ycsbc -db storeds -threads 4 -dbpath /pmem/skiplist.pmem -type skiplist-pmem -P workloads/workloada.spec
sudo ./ycsbc -db storeds -threads 8 -dbpath /pmem/skiplist.pmem -type skiplist-pmem -P workloads/workloada.spec
sudo ./ycsbc -db storeds -threads 12 -dbpath /pmem/skiplist.pmem -type skiplist-pmem -P workloads/workloada.spec
echo "b"
sudo ./ycsbc -db storeds -threads 1 -dbpath /pmem/skiplist.pmem -type skiplist-pmem -P workloads/workloadb.spec
sudo ./ycsbc -db storeds -threads 4 -dbpath /pmem/skiplist.pmem -type skiplist-pmem -P workloads/workloadb.spec
sudo ./ycsbc -db storeds -threads 8 -dbpath /pmem/skiplist.pmem -type skiplist-pmem -P workloads/workloadb.spec
sudo ./ycsbc -db storeds -threads 12 -dbpath /pmem/skiplist.pmem -type skiplist-pmem -P workloads/workloadb.spec
echo "c"
sudo ./ycsbc -db storeds -threads 1 -dbpath /pmem/skiplist.pmem -type skiplist-pmem -P workloads/workloadc.spec
sudo ./ycsbc -db storeds -threads 4 -dbpath /pmem/skiplist.pmem -type skiplist-pmem -P workloads/workloadc.spec
sudo ./ycsbc -db storeds -threads 8 -dbpath /pmem/skiplist.pmem -type skiplist-pmem -P workloads/workloadc.spec
sudo ./ycsbc -db storeds -threads 12 -dbpath /pmem/skiplist.pmem -type skiplist-pmem -P workloads/workloadc.spec
echo "d"
sudo ./ycsbc -db storeds -threads 1 -dbpath /pmem/skiplist.pmem -type skiplist-pmem -P workloads/workloadd.spec
sudo ./ycsbc -db storeds -threads 4 -dbpath /pmem/skiplist.pmem -type skiplist-pmem -P workloads/workloadd.spec
sudo ./ycsbc -db storeds -threads 8 -dbpath /pmem/skiplist.pmem -type skiplist-pmem -P workloads/workloadd.spec
sudo ./ycsbc -db storeds -threads 12 -dbpath /pmem/skiplist.pmem -type skiplist-pmem -P workloads/workloadd.spec
echo "f"
# ./ycsbc -db storeds -threads 1 -dbpath /pmem/hashtable.pmem -type hashtable-pmem -P workloads/workloade.spec
sudo ./ycsbc -db storeds -threads 1 -dbpath /pmem/skiplist.pmem -type skiplist-pmem -P workloads/workloadf.spec
sudo ./ycsbc -db storeds -threads 4 -dbpath /pmem/skiplist.pmem -type skiplist-pmem -P workloads/workloadf.spec
sudo ./ycsbc -db storeds -threads 8 -dbpath /pmem/skiplist.pmem -type skiplist-pmem -P workloads/workloadf.spec
sudo ./ycsbc -db storeds -threads 12 -dbpath /pmem/skiplist.pmem -type skiplist-pmem -P workloads/workloadf.spec
echo "*****************<>*****************"
