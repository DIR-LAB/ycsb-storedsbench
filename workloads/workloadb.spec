# Yahoo! Cloud System Benchmark
# Workload A: Insert heavy workload
#   Application example: Session store recording recent actions
#                        
#   Read/Write ratio: 25/75
#   Default data size: 1 KB records (10 fields, 100 bytes each, plus key)
#   Request distribution: zipfian

recordcount=10000000
operationcount=10000000
workload=com.yahoo.ycsb.workloads.CoreWorkload

readallfields=true

readproportion=0
updateproportion=1
scanproportion=0
insertproportion=0

requestdistribution=zipfian

