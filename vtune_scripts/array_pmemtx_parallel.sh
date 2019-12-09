#!/bin/bash

make clean
make

#array-pmem
echo "[workload a]"
echo "*****************<>*****************"

amplxe-cl -collect hotspots -app-working-dir /home/cyork5/dirlab/ycsb-storedsbench -- /home/cyork5/dirlab/ycsb-storedsbench/ycsbc_parallel -db storeds -threads 1 -dbpath /pmem/array -type array-pmem-tx  -P workloads/workloada.spec 

amplxe-cl -collect hotspots -app-working-dir /home/cyork5/dirlab/ycsb-storedsbench -- /home/cyork5/dirlab/ycsb-storedsbench/ycsbc_parallel -db storeds -threads 2 -dbpath /pmem/array -type array-pmem-tx  -P workloads/workloada.spec

amplxe-cl -collect hotspots -app-working-dir /home/cyork5/dirlab/ycsb-storedsbench -- /home/cyork5/dirlab/ycsb-storedsbench/ycsbc_parallel -db storeds -threads 4 -dbpath /pmem/array -type array-pmem-tx -P workloads/workloada.spec

amplxe-cl -collect hotspots -app-working-dir /home/cyork5/dirlab/ycsb-storedsbench -- /home/cyork5/dirlab/ycsb-storedsbench/ycsbc_parallel -db storeds -threads 8 -dbpath /pmem/array -type array-pmem-tx -P workloads/workloada.spec

amplxe-cl -collect hotspots -app-working-dir /home/cyork5/dirlab/ycsb-storedsbench -- /home/cyork5/dirlab/ycsb-storedsbench/ycsbc_parallel -db storeds -threads 16 -dbpath /pmem/array -type array-pmem-tx  -P workloads/workloada.spec

amplxe-cl -collect hotspots -app-working-dir /home/cyork5/dirlab/ycsb-storedsbench -- /home/cyork5/dirlab/ycsb-storedsbench/ycsbc_parallel -db storeds -threads 32 -dbpath /pmem/array -type array-pmem-tx  -P workloads/workloada.spec

#array-pmem
echo "workload b"
echo "*****************<>*****************"


amplxe-cl -collect hotspots -app-working-dir /home/cyork5/dirlab/ycsb-storedsbench -- /home/cyork5/dirlab/ycsb-storedsbench/ycsbc_parallel -db storeds -threads 1 -dbpath /pmem/array -type array-pmem-tx  -P workloads/workloadb.spec 

amplxe-cl -collect hotspots -app-working-dir /home/cyork5/dirlab/ycsb-storedsbench -- /home/cyork5/dirlab/ycsb-storedsbench/ycsbc_parallel -db storeds -threads 2 -dbpath /pmem/array -type array-pmem-tx  -P workloads/workloadb.spec

amplxe-cl -collect hotspots -app-working-dir /home/cyork5/dirlab/ycsb-storedsbench -- /home/cyork5/dirlab/ycsb-storedsbench/ycsbc_parallel -db storeds -threads 4 -dbpath /pmem/array -type array-pmem-tx  -P workloads/workloadb.spec

amplxe-cl -collect hotspots -app-working-dir /home/cyork5/dirlab/ycsb-storedsbench -- /home/cyork5/dirlab/ycsb-storedsbench/ycsbc_parallel -db storeds -threads 8 -dbpath /pmem/array -type array-pmem-tx  -P workloads/workloadb.spec

amplxe-cl -collect hotspots -app-working-dir /home/cyork5/dirlab/ycsb-storedsbench -- /home/cyork5/dirlab/ycsb-storedsbench/ycsbc_parallel -db storeds -threads 16 -dbpath /pmem/array -type array-pmem-tx  -P workloads/workloadb.spec

amplxe-cl -collect hotspots -app-working-dir /home/cyork5/dirlab/ycsb-storedsbench -- /home/cyork5/dirlab/ycsb-storedsbench/ycsbc_parallel -db storeds -threads 32 -dbpath /pmem/array -type array-pmem-tx  -P workloads/workloadb.spec

#array-pmem
echo "workload c"
echo "*****************<>*****************"

amplxe-cl -collect hotspots -app-working-dir /home/cyork5/dirlab/ycsb-storedsbench -- /home/cyork5/dirlab/ycsb-storedsbench/ycsbc_parallel -db storeds -threads 1 -dbpath /pmem/array -type array-pmem-tx  -P workloads/workloadc.spec 

amplxe-cl -collect hotspots -app-working-dir /home/cyork5/dirlab/ycsb-storedsbench -- /home/cyork5/dirlab/ycsb-storedsbench/ycsbc_parallel -db storeds -threads 2 -dbpath /pmem/array -type array-pmem-tx  -P workloads/workloadc.spec

amplxe-cl -collect hotspots -app-working-dir /home/cyork5/dirlab/ycsb-storedsbench -- /home/cyork5/dirlab/ycsb-storedsbench/ycsbc_parallel -db storeds -threads 4 -dbpath /pmem/array -type array-pmem-tx  -P workloads/workloadc.spec

amplxe-cl -collect hotspots -app-working-dir /home/cyork5/dirlab/ycsb-storedsbench -- /home/cyork5/dirlab/ycsb-storedsbench/ycsbc_parallel -db storeds -threads 8 -dbpath /pmem/array -type array-pmem-tx  -P workloads/workloadc.spec

amplxe-cl -collect hotspots -app-working-dir /home/cyork5/dirlab/ycsb-storedsbench -- /home/cyork5/dirlab/ycsb-storedsbench/ycsbc_parallel -db storeds -threads 16 -dbpath /pmem/array -type array-pmem-tx  -P workloads/workloadc.spec

amplxe-cl -collect hotspots -app-working-dir /home/cyork5/dirlab/ycsb-storedsbench -- /home/cyork5/dirlab/ycsb-storedsbench/ycsbc_parallel -db storeds -threads 32 -dbpath /pmem/array -type array-pmem-tx  -P workloads/workloadc.spec



#array-pmem
echo "workload d"
echo "*****************<>*****************"


amplxe-cl -collect hotspots -app-working-dir /home/cyork5/dirlab/ycsb-storedsbench -- /home/cyork5/dirlab/ycsb-storedsbench/ycsbc_parallel -db storeds -threads 1 -dbpath /pmem/array -type array-pmem-tx  -P workloads/workloadd.spec 

amplxe-cl -collect hotspots -app-working-dir /home/cyork5/dirlab/ycsb-storedsbench -- /home/cyork5/dirlab/ycsb-storedsbench/ycsbc_parallel -db storeds -threads 2 -dbpath /pmem/array -type array-pmem-tx  -P workloads/workloadd.spec

amplxe-cl -collect hotspots -app-working-dir /home/cyork5/dirlab/ycsb-storedsbench -- /home/cyork5/dirlab/ycsb-storedsbench/ycsbc_parallel -db storeds -threads 4 -dbpath /pmem/array -type array-pmem-tx  -P workloads/workloadd.spec

amplxe-cl -collect hotspots -app-working-dir /home/cyork5/dirlab/ycsb-storedsbench -- /home/cyork5/dirlab/ycsb-storedsbench/ycsbc_parallel -db storeds -threads 8 -dbpath /pmem/array -type array-pmem -tx -P workloads/workloadd.spec

amplxe-cl -collect hotspots -app-working-dir /home/cyork5/dirlab/ycsb-storedsbench -- /home/cyork5/dirlab/ycsb-storedsbench/ycsbc_parallel -db storeds -threads 16 -dbpath /pmem/array -type array-pmem-tx  -P workloads/workloadd.spec

amplxe-cl -collect hotspots -app-working-dir /home/cyork5/dirlab/ycsb-storedsbench -- /home/cyork5/dirlab/ycsb-storedsbench/ycsbc_parallel -db storeds -threads 32 -dbpath /pmem/array -type array-pmem-tx  -P workloads/workloadd.spec



#array-pmem
echo "workload e"
echo "*****************<>*****************"

amplxe-cl -collect hotspots -app-working-dir /home/cyork5/dirlab/ycsb-storedsbench -- /home/cyork5/dirlab/ycsb-storedsbench/ycsbc_parallel -db storeds -threads 1 -dbpath /pmem/array -type array-pmem-tx  -P workloads/workloade.spec 

amplxe-cl -collect hotspots -app-working-dir /home/cyork5/dirlab/ycsb-storedsbench -- /home/cyork5/dirlab/ycsb-storedsbench/ycsbc_parallel -db storeds -threads 2 -dbpath /pmem/array -type array-pmem-tx  -P workloads/workloade.spec

amplxe-cl -collect hotspots -app-working-dir /home/cyork5/dirlab/ycsb-storedsbench -- /home/cyork5/dirlab/ycsb-storedsbench/ycsbc_parallel -db storeds -threads 4 -dbpath /pmem/array -type array-pmem-tx  -P workloads/workloade.spec

amplxe-cl -collect hotspots -app-working-dir /home/cyork5/dirlab/ycsb-storedsbench -- /home/cyork5/dirlab/ycsb-storedsbench/ycsbc_parallel -db storeds -threads 8 -dbpath /pmem/array -type array-pmem-tx  -P workloads/workloade.spec

amplxe-cl -collect hotspots -app-working-dir /home/cyork5/dirlab/ycsb-storedsbench -- /home/cyork5/dirlab/ycsb-storedsbench/ycsbc_parallel -db storeds -threads 16 -dbpath /pmem/array -type array-pmem-tx  -P workloads/workloade.spec

amplxe-cl -collect hotspots -app-working-dir /home/cyork5/dirlab/ycsb-storedsbench -- /home/cyork5/dirlab/ycsb-storedsbench/ycsbc_parallel -db storeds -threads 32 -dbpath /pmem/array -type array-pmem-tx  -P workloads/workloade.spec





