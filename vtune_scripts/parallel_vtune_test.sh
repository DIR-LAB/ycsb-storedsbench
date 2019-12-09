#!/bin/bash

make clean
make

echo "Array DRAM"
./array_dram_parallel.sh >> array_dram_parallel_results.txt

echo "Array PMEM"
./array_dram_parallel.sh >> array_pmem_parallel_results.txt

echo "Array PMEMtc"
./array_pmemtx_parallel.sh >> array_pmemtx_parallel_results.txt

echo "Btree DRAM"
./btree_dram_parallel.sh >> btree_dram_parallel_results.txt

echo "Btree PMEM"
./btree_pmem_parallel.sh >> btree_pmem_parallel_results.txt

echo "Btree Pmemtx"
./btree_pmemtx_parallel.sh >> btree_pmemtx_parallelsh_results.txt

echo "Hashtable DRAm"
./hashtable_dram_parallel.sh >> hashtable_pmemtx_parallel_results.txt

echo "Hashtable Pmem"
./hashtable_pmem_parallel.sh >> hashtable_pmem_parallel_results.txt

echo "Hashtable Pmemtx"
./hashtable_pmemtx_parallel.sh >> hashtable_pmem_parallel_results.txt

echo "Skiplist DRAM"
./skiplist_dram_parallel.sh >> skiplist_dram_parallel_results.txt

echo "Skiplist PMEM"
./skiplist_pmem_parallel.sh >> skiplist_pmem_parallel_results.txt

echo "Skiplist PMEMtx"
./skiplist_pmemtx_parallel.sh >> skiplist_pmemtx_parallel_results.txt

