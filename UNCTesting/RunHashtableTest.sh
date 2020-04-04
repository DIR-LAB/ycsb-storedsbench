rm /pmem/*.pmem

echo "" >results/hashtable_$1.txt

echo ../ycsbc -db storeds -threads 1 -dbpath mnt/PM0/index.ht -type hashtable-pmem -P workloads/$1.spec >>results/hashtable_$1.txt >>results/hashtable_$1.txt

echo "" >>results/hashtable_$1.txt

date >>results/hashtable_$1.txt

./../ycsbc -db storeds -threads 1 -dbpath /pmem/index.pmem -type hashtable-pmem -P workloads/$1.spec >>results/hashtable_$1.txt

date >>results/hashtable_$1.txt
echo "" >>results/hashtable_$1.txt
