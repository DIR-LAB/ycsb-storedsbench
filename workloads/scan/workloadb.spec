# Yahoo! Cloud System Benchmark
# Workload A: Insert heavy workload
#   Application example: Session store recording recent actions
#                        
#   Read/Write ratio: 25/75
#   Default data size: 1 KB records (10 fields, 100 bytes each, plus key)
#   Request distribution: zipfian

recordcount=100000
operationcount=100000
workload=com.yahoo.ycsb.workloads.CoreWorkload

readallfields=true

readproportion=0
updateproportion=0
scanproportion=.25
insertproportion=.75

requestdistribution=zipfian

maxscanlength=100

scanlengthdistribution=uniform