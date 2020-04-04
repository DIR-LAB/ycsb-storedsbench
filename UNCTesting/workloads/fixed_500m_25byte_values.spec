# Yahoo! Cloud System Benchmark
# Workload 100m: 90% update, 10% insert
#                        
#   Read/Write ratio: 90/10
#   Data size: 25 byte values

recordcount=500000000
operationcount=10000000
workload=com.yahoo.ycsb.workloads.CoreWorkload

readallfields=true

readproportion=0
updateproportion=0.9
scanproportion=0
insertproportion=0.1

fieldlength=25


