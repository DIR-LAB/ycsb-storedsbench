#!/bin/bash
echo "[Benchmark] array started ..."
./array-benchmark.sh > ar.out
echo "[Benchmark] rbtree started ..."
./rbtree-benchmark.sh > rb.out
echo "[Benchmark] end ..."
