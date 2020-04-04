rm /pmem/*.pmem

echo "" >results/threemisses_$1.txt

echo ../ycsbc -db storeds -threads 1 -dbpath /dummy_path/dummy.pmem -type threemiss -P workloads/$1.spec >>results/threemisses_$1.txt

echo "" >>results/threemisses_$1.txt

date >>results/threemisses_$1.txt

./../ycsbc -db storeds -threads 1 -dbpath /dummy_path/dummy.pmem -type threemiss -P workloads/$1.spec >>results/threemisses_$1.txt

date >>results/threemisses_$1.txt
echo "" >>results/threemisses_$1.txt
