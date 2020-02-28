# Yahoo! Cloud System Benchmark
# Workload A: Insert only workload
#   Application example: Session store recording recent actions
#                        
#   Read/Write ratio: 0/100
#   Default data size: 1 KB records (10 fields, 100 bytes each, plus key)
#   Request distribution: zipfian

recordcount=1000000
operationcount=1000000
workload=com.yahoo.ycsb.workloads.CoreWorkload

readallfields=true

readproportion=0
updateproportion=0.9
scanproportion=0
insertproportion=0.1

requestdistribution=zipfian

