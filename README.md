# pmemids_bench

**pmemids_bench** is a C++ version of YCSB based benchmark suite that includes seven commonly used indexing data structures implemented in four persistent modes and four parallel modes (shown in later table). You can learn more about this project in our [MSST 2020 paper](https://storageconference.us/2020/Papers/02.APerformanceStudyOptane.pdf).

### Table of Contents
**[Quick Start](#quick-start)**<br>
**[Benchmark Design and Implementation](#benchmark-design-and-implementation)**<br>
**[Benchmarking Results and Analysis](#benchmarking-results-and-analysis)**<br>
**[Usage](#usage)**<br>
**[Contribution](#contribution)**<br>
**[Credit](#credit)**<br>

### Relevant publications:
  * Islam, Abdullah Al Raqibul and Dai, Dong. "Understand the overheads of storage data structures on persistent memory," PPoPP '20: Proceedings of the 25th ACM SIGPLAN Symposium on Principles and Practice of Parallel Programming, 2020. doi:10.1145/3332466.3374509
  * Islam, Abdullah Al Raqibul and Dai, Dong and Narayanan, Anirudh and York, Christopher. "A Performance Study of Optane Persistent Memory: From Indexing Data Structures’ Perspective," MSST '20: 36th International Conference on Massive Storage Systems and Technology, 2020.

## Quick Start

### Prerequisites
* GNU Make
* C++ (we used version 11)
* PMDK (we used version 1.8)
* external libraries: pthreads, pmemobj, vmem, memkind

### Build & Run
To build `pmemids_bench`:

```
$ make
```

Run Workload A with a dram-based array implementation in single thread:
```
./ycsbc -db storeds -threads 1 -dbpath /pmem -type array-dram -P workloads/workloada.spec
```
You can run the program without any arguments (i.e. `./ycsbc`) to see the command line argument help information.

Note that we do not have load and run commands as the original YCSB. Specify
how many records to load by the recordcount property. Reference properties
files in the workloads dir.

## Benchmark Design and Implementation

### Indexing Data Structures

<table>
  <thead>
    <tr>
      <th>Data Structure</th>
      <th>Description</th>
      <th>Persistent Modes</th>
      <th>Parallel Modes</th>
      <th>Workloads</th>
    </tr>
  </thead>
  <tbody>
    <tr>
      <td align="middle">Arraylist</td>
      <td align="middle">Fixed items/size</td>
      <td align="middle" rowspan="7">DRAM<br><br>
                           PMEM-Volatile<br><br>
                           PMEM-Persist<br><br>
                           PMEM-Trans</td>
      <td align="middle" rowspan="7">Single thread<br><br>
                           Parallel, Saturated<br><br>
                           Concurrent, Contention<br><br>
                           NUMA</td>
      <td align="middle" rowspan="7">A (100% Read)<br><br>
                           B (100% Write)<br>
                           (90% Update, 10% Insert)</td>
    </tr>
    <tr>
      <td align="middle">Linkedlist</td>
      <td align="middle">Appended only</td>
    </tr>
    <tr>
      <td align="middle">Hashtable</td>
      <td align="middle">Chained Linkedlist</td>
    </tr>
    <tr>
      <td align="middle">Skiplist</td>
      <td align="middle">Fixed height, fair probability</td>
    </tr>
    <tr>
      <td align="middle">B-Tree</td>
      <td align="middle">Values referenced/embedded</td>
    </tr>
    <tr>
      <td align="middle">B+-Tree</td>
      <td align="middle">Values in the leaf nodes only</td>
    </tr>
    <tr>
      <td align="middle">RB-Tree</td>
      <td align="middle">Instant rebalancing</td>
    </tr>
    <tr>
      <td colspan="5" align="middle">Table 1: Summary of indexing data structures, their different running modes under various workloads included in the benchmark suite.</td>
    </tr>
  </tbody>
</table>

### Workloads

To help benchmark the storage systems, typical YCSB benchmarks include some representative workloads such as the five workloads (A to E) shown in the top part of Table 2.

However, in `pmemids_bench`, this is not feasible anymore, simply because unlike benchmarking storage systems, we do not know how developers will use the indexing data structures. So, in our implementation, we leverage the YCSB workload generator, but we do create different workloads for our evaluations, as placed in the bottom part of Table 2.

There are only two workloads included in `pmemids_bench` by default. But developers can configure their own workloads as we have placed several sample of them in "workloads" directory (i.e. check files with ".spec" file extension). We also reserved the original YCSB benchmarks, you can check it [here](https://github.com/DIR-LAB/ycsb-storedsbench/tree/master/workloads/sample).

<table>
  <thead>
    <tr>
      <th>YCSB Workload</th>
      <th>A</th>
      <th>B</th>
      <th>C</th>
      <th>D</th>
      <th>E</th>
    </tr>
  </thead>
  <tbody>
    <tr>
      <td align="middle">Read<br>
                           Update<br>
                           Insert<br>
                           Read & Update</td>
      <td align="middle">50<br>
                           50<br>
                           -<br>
                           -</td>
      <td align="middle">95<br>
                           5<br>
                           -<br>
                           -</td>
      <td align="middle">100<br>
                           -<br>
                           -<br>
                           -</td>
      <td align="middle">95<br>
                           -<br>
                           5<br>
                           -</td>
      <td align="middle">50<br>
                           -<br>
                           -<br>
                           50</td>
    </tr>
  </tbody>
  <thead>
    <tr>
      <th>pmemids_bench Workload</th>
      <th colspan="2">A (100% Read)</th>
      <th colspan="3">B (100% Write)</th>
    </tr>
  </thead>
  <tbody>
    <tr>
      <td align="middle">Read<br>
                           Update<br>
                           Insert</td>
      <td  colspan="2" align="middle">100<br>
                           -<br>
                           -</td>
      <td  colspan="3" align="middle">-<br>
                           90<br>
                           10</td>
    </tr>
    <tr>
      <td colspan="6" align="middle">Table 2: Workloads in % of differebt operations.</td>
    </tr>
  </tbody>
</table>

## Benchmarking Results and Analysis

### Evaluation Platform
We tested our benchmark suiteon a Dell R740 rack server
with two sockets. Each socket installs a 2nd generation Intel
Xeon Scalable Processor (Gold 6254 @ 3.10G) with 18 physical
(36 virtual) cores. The machine is running Ubuntu 18.04 with a
Linux kernel version 4.15.0. To enable the NUMA evaluation,
we put all the DRAM and Optane DC DIMMS into one socket
(node 0). This socket has 6 DRAM DIMMS with 32GB each
and 1 Optane DC DIMM with 128GB. In all the evaluations,
except the NUMA ones, we bind all the threads to node 0 to
enable local memory accesses. The total memory capacity is
192GB DRAM and 128GB Optane DC. We used PMDK 1.8
in the implementation.

### Benchmarking Results and Observations

#### Read/Write performance

<figure>
    <img align="middle" src="https://github.com/DIR-LAB/ycsb-storedsbench/blob/master/workloads/plotting-readme/msst20_eval_1-1.jpg" alt="single_thread_100%_write"/>
    <img align="middle" src="https://github.com/DIR-LAB/ycsb-storedsbench/blob/master/workloads/plotting-readme/msst20_eval_1-2.jpg" alt="single_thread_100%_read"/>
    <figcaption>Fig. 3. The benchmark results of seven indexing data structures in a single thread case. The x-axis lists all the persistent modes and y-axis shows the throughput (KTPS or k transactions per second) after running the workloads. We tested all five workloads, and showed two of them (100% read and 100% write) here. Note that, all the experiments were repeated 10 times. We plot both the average and the variations in the figure.</figcaption>
</figure>

#### Transaction overhead

<figure>
  <img width="50%" height="50%" align="middle" src="https://github.com/DIR-LAB/ycsb-storedsbench/blob/master/workloads/plotting-readme/msst20_eval_3-1.jpg" alt="btree_version_compare"/>
  <figcaption>Fig. 5. The performance of two B-Tree implementations (value referenced vs. value embedded) in different persistent modes. The tree node size changes from 256 byts to 2K btyes.</figcaption>
</figure>


#### Scalability performance

<figure>
    <img align="middle" src="https://github.com/DIR-LAB/ycsb-storedsbench/blob/master/workloads/plotting-readme/msst20_eval_4-1.jpg" alt="parallel_thread_100%_write"/>
    <img align="middle" src="https://github.com/DIR-LAB/ycsb-storedsbench/blob/master/workloads/plotting-readme/msst20_eval_4-2.jpg" alt="parallel_thread_100%_read"/>
    <figcaption>Fig. 6. The benchmark results of seven indexing data structures in the Parallel, Saturated mode. The x-axis is the thread number; the y-axis is the aggregated throughput (KTPS) across all threads. We used up to 16 threads to make sure each thread is running in a physical core. We show both 100% write and 100% read workloads here.</figcaption>
</figure>

#### Concurrent/Contention performance

<table>
  <tr>
    <td colspan="2" align="middle">
      <figure>
        <img align="middle" src="https://github.com/DIR-LAB/ycsb-storedsbench/blob/master/workloads/plotting-readme/msst20_eval_5-3.jpg" alt="parallel_thread_100%_write"/>
        </figure>
    </td>
  </tr>
  <tr>
    <td>
       <figure>
        <img align="middle" src="https://github.com/DIR-LAB/ycsb-storedsbench/blob/master/workloads/plotting-readme/msst20_eval_5-1.jpg" alt="concurrent_thread_1"/>
        </figure>
    </td>
    <td>
      <figure>
        <img align="middle" src="https://github.com/DIR-LAB/ycsb-storedsbench/blob/master/workloads/plotting-readme/msst20_eval_5-2.jpg" alt="concurrent_thread_16"/>
      </figure>
    </td>
  </tr>
  <tr>
    <td colspan="2" align="middle">Fig. 7. The 100% write performance comparison of two cases running in the Concurrent, Contention mode. Here, x-axis groups indexing data structures and each group contains results of different persistent modes. The y-axis shows the throughput in KTPS.</td>
  </tr>
</table>

#### Lock persistence overhead

<figure>
  <img width="50%" align="middle" src="https://github.com/DIR-LAB/ycsb-storedsbench/blob/master/workloads/plotting-readme/msst20_eval_7-2.jpg" alt="rbtree_lock_persistence_legend"/>
  <img width="50%" align="middle" src="https://github.com/DIR-LAB/ycsb-storedsbench/blob/master/workloads/plotting-readme/msst20_eval_7-4.jpg" alt="rbtree_lock_persistence"/>
  <figcaption>Fig. 8. The 100% write performance comparison of four different mutex lock usages in PMEM-based RBTree, with thread number increases from 1 to 16</figcaption>
</figure>

#### NUMA overhead

<figure>
    <img align="middle" src="https://github.com/DIR-LAB/ycsb-storedsbench/blob/master/workloads/plotting-readme/msst20_eval_6_1-1.jpg" alt="numa_100%_write"/>
    <img align="middle" src="https://github.com/DIR-LAB/ycsb-storedsbench/blob/master/workloads/plotting-readme/msst20_eval_6_1-2.jpg" alt="numa_100%_read"/>
    <figcaption>Fig. 9. The benchmark results of seven indexing data structures in different persistent modes accessing local memory and remote memory. All evaluations are based on single thread.</figcaption>
</figure>

## Usage

One of the key motivations of this project is to allow others to reuse our framework and implement their specific data structures for performance comparison (using [YCSB workload](https://github.com/brianfrankcooper/YCSB/tree/master/workloads)). Please have a look at our [wiki page](https://github.com/DIR-LAB/ycsb-storedsbench/wiki) to learn how to add new data structures to this project. From the [Workloads](#workloads) section, you will get some idea about workload generation.

The project is well suited for testing third-party libraries. We helped [TwoMisses](https://www.threemisses.com) (an updated version of `ThreeMisses`, the winner of the first SNIA NVDIMM Challenge!) to benchmark their library. The integration is done in the branch [threemiss](https://github.com/DIR-LAB/ycsb-storedsbench/tree/threemiss).

## Contribution

If you would like to contribute to `pmemids_bench`, we would certainly appreciate your help! Here are some of the ways you can contribute:

* Bug fixes, whether they be for performance or correctness of the existing data structure implementation
* Improvement of the existing benchmark implementation and documentation
* Support for additional data structure implementations; we are open to adding more data structures in this repo

Our future goal is to provide a set of portable, high-performance PMEM-aware data structure baselines. For code contributions, please focus on code simplicity and readability. If you open a pull request, we will do an immediate sanity check and respond to you as soon as possible.

## Credit
* We use Jinglei Ren's (Github handle: [basicthinker](https://github.com/basicthinker)) C++ implementation of YCSB benchmark [YCSB-C](https://github.com/basicthinker/YCSB-C) as the base of our project.
