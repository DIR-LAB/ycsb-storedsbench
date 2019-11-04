#!/bin/bash

make clean
make

#array-dram
echo "[Benchmark-Test] array-dram"
./ycsbc_test -db storeds -threads 1 -dbpath /pmem/array.pmem -type array-dram -P workloads/workloada.spec
echo "*****************<>*****************"

#array-pmem
echo "[Benchmark-Test] array-pmem"
rm /pmem/array.pmem
./ycsbc_test -db storeds -threads 1 -dbpath /pmem/array.pmem -type array-pmem -P workloads/workloada.spec
echo "*****************<>*****************"

#array-pmem-tx
echo "[Benchmark-Test] array-pmem-tx"
rm /pmem/array.pmem
./ycsbc_test -db storeds -threads 1 -dbpath /pmem/array.pmem -type array-pmem-tx -P workloads/workloada.spec
echo "*****************<>*****************"

#array-pmem-raw
echo "[Benchmark-Test] array-pmem-raw"
rm /pmem/array.pmem
./ycsbc_test -db storeds -threads 1 -dbpath /pmem/array-raw.pmem -type array-pmem-raw -P workloads/workloada.spec
echo "*****************<>*****************"

#hashtable-dram
echo "[Benchmark-Test] hashtable-dram"
./ycsbc_test -db storeds -threads 1 -dbpath /pmem/hashtable.pmem -type hashtable-dram -P workloads/workloada.spec
echo "*****************<>*****************"

#hashtable-pmem
echo "[Benchmark-Test] hashtable-pmem"
rm /pmem/hashtable.pmem
./ycsbc_test -db storeds -threads 1 -dbpath /pmem/hashtable.pmem -type hashtable-pmem -P workloads/workloada.spec
echo "*****************<>*****************"

#hashtable-pmem-tx
echo "[Benchmark-Test] hashtable-pmem-tx"
rm /pmem/hashtable.pmem
./ycsbc_test -db storeds -threads 1 -dbpath /pmem/hashtable.pmem -type hashtable-pmem-tx -P workloads/workloada.spec
echo "*****************<>*****************"

##linkedlist-dram
#echo "[Benchmark-Test] linkedlist-dram"
#./ycsbc_test -db storeds -threads 1 -dbpath /pmem/linkedlist.pmem -type linkedlist-dram -P workloads/workloada.spec
#echo "*****************<>*****************"
#
##linkedlist-pmem
#echo "[Benchmark-Test] linkedlist-pmem"
#./ycsbc_test -db storeds -threads 1 -dbpath /pmem/linkedlist.pmem -type linkedlist-pmem -P workloads/workloada.spec
#echo "*****************<>*****************"
#
##linkedlist-pmem-tx
#echo "[Benchmark-Test] linkedlist-pmem-tx"
#./ycsbc_test -db storeds -threads 1 -dbpath /pmem/linkedlist.pmem -type linkedlist-pmem-tx -P workloads/workloada.spec
#echo "*****************<>*****************"

#skiplist-dram
echo "[Benchmark-Test] skiplist-dram"
./ycsbc_test -db storeds -threads 1 -dbpath /pmem/skiplist.pmem -type skiplist-dram -P workloads/workloada.spec
echo "*****************<>*****************"

#skiplist-pmem
echo "[Benchmark-Test] skiplist-pmem"
rm /pmem/skiplist.pmem
./ycsbc_test -db storeds -threads 1 -dbpath /pmem/skiplist.pmem -type skiplist-pmem -P workloads/workloada.spec
echo "*****************<>*****************"

#skiplist-pmem
echo "[Benchmark-Test] skiplist-pmem-tx"
rm /pmem/skiplist.pmem
./ycsbc_test -db storeds -threads 1 -dbpath /pmem/skiplist.pmem -type skiplist-pmem-tx -P workloads/workloada.spec
echo "*****************<>*****************"

#btree-dram
echo "[Benchmark-Test] btree-dram"
./ycsbc_test -db storeds -threads 1 -dbpath /pmem/btree.pmem -type btree-dram -P workloads/workloada.spec
echo "*****************<>*****************"

#btree-pmem
echo "[Benchmark-Test] btree-pmem"
rm /pmem/btree.pmem
./ycsbc_test -db storeds -threads 1 -dbpath /pmem/btree.pmem -type btree-pmem -P workloads/workloada.spec
echo "*****************<>*****************"

#btree-pmem-tx
echo "[Benchmark-Test] btree-pmem-tx"
rm /pmem/btree.pmem
./ycsbc_test -db storeds -threads 1 -dbpath /pmem/btree.pmem -type btree-pmem-tx -P workloads/workloada.spec
echo "*****************<>*****************"