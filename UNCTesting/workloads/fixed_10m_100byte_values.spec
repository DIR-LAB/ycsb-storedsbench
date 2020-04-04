# Yahoo! Cloud System Benchmark
# Workload 10m: 90% update, 10% insert
#                        
#   Read/Write ratio: 10/90
#   Default data size: 100 byte values

recordcount=10000000
operationcount=10000000
workload=com.yahoo.ycsb.workloads.CoreWorkload

readallfields=true

readproportion=0
updateproportion=0.9
scanproportion=0
insertproportion=0.1


