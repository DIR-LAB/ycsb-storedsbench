#!/bin/bash

make clean
make

#array-dram
echo "[workload a]"
echo "*****************<>*****************"

amplxe-cl -collect hotspots -app-working-dir /home/cyork5/dirlab/ycsb-storedsbench -- /home/cyork5/dirlab/ycsb-storedsbench/ycsbc_parallel -db storeds -threads 1 -dbpath /pmem/skiplist.pmem -type skiplist-dram -P workloads/workloada.spec 

amplxe-cl -collect hotspots -app-working-dir /home/cyork5/dirlab/ycsb-storedsbench -- /home/cyork5/dirlab/ycsb-storedsbench/ycsbc_parallel -db storeds -threads 2 -dbpath /pmem/skiplist.pmem -type skiplist-dram -P workloads/workloada.spec

amplxe-cl -collect hotspots -app-working-dir /home/cyork5/dirlab/ycsb-storedsbench -- /home/cyork5/dirlab/ycsb-storedsbench/ycsbc_parallel -db storeds -threads 4 -dbpath /pmem/skiplist.pmem -type skiplist-dram -P workloads/workloada.spec

amplxe-cl -collect hotspots -app-working-dir /home/cyork5/dirlab/ycsb-storedsbench -- /home/cyork5/dirlab/ycsb-storedsbench/ycsbc_parallel -db storeds -threads 8 -dbpath /pmem/skiplist.pmem -type skiplist-dram -P workloads/workloada.spec

amplxe-cl -collect hotspots -app-working-dir /home/cyork5/dirlab/ycsb-storedsbench -- /home/cyork5/dirlab/ycsb-storedsbench/ycsbc_parallel -db storeds -threads 16 -dbpath /pmem/skiplist.pmem -type skiplist-dram -P workloads/workloada.spec

amplxe-cl -collect hotspots -app-working-dir /home/cyork5/dirlab/ycsb-storedsbench -- /home/cyork5/dirlab/ycsb-storedsbench/ycsbc_parallel -db storeds -threads 32 -dbpath /pmem/skiplist.pmem -type skiplist-dram -P workloads/workloada.spec

#array-dram
echo "workload b"
echo "*****************<>*****************"


amplxe-cl -collect hotspots -app-working-dir /home/cyork5/dirlab/ycsb-storedsbench -- /home/cyork5/dirlab/ycsb-storedsbench/ycsbc_parallel -db storeds -threads 1 -dbpath /pmem/skiplist.pmem -type skiplist-dram -P workloads/workloadb.spec 

amplxe-cl -collect hotspots -app-working-dir /home/cyork5/dirlab/ycsb-storedsbench -- /home/cyork5/dirlab/ycsb-storedsbench/ycsbc_parallel -db storeds -threads 2 -dbpath /pmem/skiplist.pmem -type skiplist-dram -P workloads/workloadb.spec

amplxe-cl -collect hotspots -app-working-dir /home/cyork5/dirlab/ycsb-storedsbench -- /home/cyork5/dirlab/ycsb-storedsbench/ycsbc_parallel -db storeds -threads 4 -dbpath /pmem/skiplist.pmem -type skiplist-dram -P workloads/workloadb.spec

amplxe-cl -collect hotspots -app-working-dir /home/cyork5/dirlab/ycsb-storedsbench -- /home/cyork5/dirlab/ycsb-storedsbench/ycsbc_parallel -db storeds -threads 8 -dbpath /pmem/skiplist.pmem -type skiplist-dram -P workloads/workloadb.spec

amplxe-cl -collect hotspots -app-working-dir /home/cyork5/dirlab/ycsb-storedsbench -- /home/cyork5/dirlab/ycsb-storedsbench/ycsbc_parallel -db storeds -threads 16 -dbpath /pmem/skiplist.pmem -type skiplist-dram -P workloads/workloadabspec

amplxe-cl -collect hotspots -app-working-dir /home/cyork5/dirlab/ycsb-storedsbench -- /home/cyork5/dirlab/ycsb-storedsbench/ycsbc_parallel -db storeds -threads 32 -dbpath /pmem/skiplist.pmem -type skiplist-dram -P workloads/workloadb.spec


#array-dram
echo "workload c"
echo "*****************<>*****************"

amplxe-cl -collect hotspots -app-working-dir /home/cyork5/dirlab/ycsb-storedsbench -- /home/cyork5/dirlab/ycsb-storedsbench/ycsbc_parallel -db storeds -threads 1 -dbpath /pmem/skiplist.pmem -type skiplist-dram -P workloads/workloadc.spec 

amplxe-cl -collect hotspots -app-working-dir /home/cyork5/dirlab/ycsb-storedsbench -- /home/cyork5/dirlab/ycsb-storedsbench/ycsbc_parallel -db storeds -threads 2 -dbpath /pmem/skiplist.pmem -type skiplist-dram -P workloads/workloadc.spec

amplxe-cl -collect hotspots -app-working-dir /home/cyork5/dirlab/ycsb-storedsbench -- /home/cyork5/dirlab/ycsb-storedsbench/ycsbc_parallel -db storeds -threads 4 -dbpath /pmem/skiplist.pmem -type skiplist-dram -P workloads/workloadc.spec

amplxe-cl -collect hotspots -app-working-dir /home/cyork5/dirlab/ycsb-storedsbench -- /home/cyork5/dirlab/ycsb-storedsbench/ycsbc_parallel -db storeds -threads 8 -dbpath /pmem/skiplist.pmem -type skiplist-dram -P workloads/workloadc.spec

amplxe-cl -collect hotspots -app-working-dir /home/cyork5/dirlab/ycsb-storedsbench -- /home/cyork5/dirlab/ycsb-storedsbench/ycsbc_parallel -db storeds -threads 16 -dbpath /pmem/skiplist.pmem -type skiplist-dram -P workloads/workloadc.spec

amplxe-cl -collect hotspots -app-working-dir /home/cyork5/dirlab/ycsb-storedsbench -- /home/cyork5/dirlab/ycsb-storedsbench/ycsbc_parallel -db storeds -threads 32 -dbpath /pmem/skiplist.pmem -type skiplist-dram -P workloads/workloadc.spec



#array-dram
echo "workload d"
echo "*****************<>*****************"


amplxe-cl -collect hotspots -app-working-dir /home/cyork5/dirlab/ycsb-storedsbench -- /home/cyork5/dirlab/ycsb-storedsbench/ycsbc_parallel -db storeds -threads 1 -dbpath /pmem/skiplist.pmem -type skiplist-dram -P workloads/workloadd.spec 

amplxe-cl -collect hotspots -app-working-dir /home/cyork5/dirlab/ycsb-storedsbench -- /home/cyork5/dirlab/ycsb-storedsbench/ycsbc_parallel -db storeds -threads 2 -dbpath /pmem/skiplist.pmem -type skiplist-dram -P workloads/workloadd.spec

amplxe-cl -collect hotspots -app-working-dir /home/cyork5/dirlab/ycsb-storedsbench -- /home/cyork5/dirlab/ycsb-storedsbench/ycsbc_parallel -db storeds -threads 4 -dbpath /pmem/skiplist.pmem -type skiplist-dram -P workloads/workloadd.spec

amplxe-cl -collect hotspots -app-working-dir /home/cyork5/dirlab/ycsb-storedsbench -- /home/cyork5/dirlab/ycsb-storedsbench/ycsbc_parallel -db storeds -threads 8 -dbpath /pmem/skiplist.pmem -type skiplist-dram -P workloads/workloadd.spec

amplxe-cl -collect hotspots -app-working-dir /home/cyork5/dirlab/ycsb-storedsbench -- /home/cyork5/dirlab/ycsb-storedsbench/ycsbc_parallel -db storeds -threads 16 -dbpath /pmem/skiplist.pmem -type skiplist-dram -P workloads/workloadd.spec

amplxe-cl -collect hotspots -app-working-dir /home/cyork5/dirlab/ycsb-storedsbench -- /home/cyork5/dirlab/ycsb-storedsbench/ycsbc_parallel -db storeds -threads 32 -dbpath /pmem/skiplist.pmem -type skiplist-dram -P workloads/workloadd.spec



#array-dram
echo "workload e"
echo "*****************<>*****************"

amplxe-cl -collect hotspots -app-working-dir /home/cyork5/dirlab/ycsb-storedsbench -- /home/cyork5/dirlab/ycsb-storedsbench/ycsbc_parallel -db storeds -threads 1 -dbpath /pmem/skiplist.pmem -type skiplist-dram -P workloads/workloade.spec 

amplxe-cl -collect hotspots -app-working-dir /home/cyork5/dirlab/ycsb-storedsbench -- /home/cyork5/dirlab/ycsb-storedsbench/ycsbc_parallel -db storeds -threads 2 -dbpath /pmem/skiplist.pmem -type skiplist-dram -P workloads/workloade.spec

amplxe-cl -collect hotspots -app-working-dir /home/cyork5/dirlab/ycsb-storedsbench -- /home/cyork5/dirlab/ycsb-storedsbench/ycsbc_parallel -db storeds -threads 4 -dbpath /pmem/skiplist.pmem -type skiplist-dram -P workloads/workloade.spec

amplxe-cl -collect hotspots -app-working-dir /home/cyork5/dirlab/ycsb-storedsbench -- /home/cyork5/dirlab/ycsb-storedsbench/ycsbc_parallel -db storeds -threads 8 -dbpath /pmem/skiplist.pmem -type skiplist-dram -P workloads/workloade.spec

amplxe-cl -collect hotspots -app-working-dir /home/cyork5/dirlab/ycsb-storedsbench -- /home/cyork5/dirlab/ycsb-storedsbench/ycsbc_parallel -db storeds -threads 16 -dbpath /pmem/skiplist.pmem -type skiplist-dram -P workloads/workloade.spec

amplxe-cl -collect hotspots -app-working-dir /home/cyork5/dirlab/ycsb-storedsbench -- /home/cyork5/dirlab/ycsb-storedsbench/ycsbc_parallel -db storeds -threads 32 -dbpath /pmem/skiplist.pmem -type skiplist-dram -P workloads/workloade.spec






