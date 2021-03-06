#!/bin/bash
echo "[Benchmark] array started ..."
./array-benchmark.sh > ar.out
echo "[Benchmark] linkedlist started ..."
./hashlist-benchmark.sh > ll.out
echo "[Benchmark] bplustree started ..."
./bp-benchmark.sh > bp.out
echo "[Benchmark] btree started ..."
./bt-benchmark.sh > bt.out
echo "[Benchmark] hashtable started ..."
./hashtable-benchmark.sh > ht.out
echo "[Benchmark] rbtree started ..."
./rbtree-benchmark.sh > rb.out
echo "[Benchmark] skiplist started ..."
./skiplist-benchmark.sh > sk.out
echo "[Benchmark] end ..."
