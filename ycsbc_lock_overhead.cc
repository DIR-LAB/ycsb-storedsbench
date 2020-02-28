//
//  ycsbc.cc
//  YCSB-C
//
//  Created by Jinglei Ren on 12/19/14.
//  Copyright (c) 2014 Jinglei Ren <jinglei@ren.systems>.
//

#include <sched.h>
#include <cstring>
#include <string>
#include <iostream>
#include <vector>
#include <future>
#include "core/utils.h"
#include "core/timer.h"
#include "core/client.h"
#include "core/core_workload.h"
#include "db/db_factory.h"

using namespace std;

void LockOverheadUsageMessage(const char *command);

bool LockOverheadStrStartWith(const char *str, const char *pre);

string LockOverheadParseCommandLine(int argc, const char *argv[], utils::Properties &props);

atomic<int> start_barrier(0);

int LockOverheadDelegateClient(ycsbc::DB *db, ycsbc::CoreWorkload *wl,
        const int num_ops, bool is_loading, int lockContentionProportion,
        int num_threads, int thread_id) {
    // Add cpu affinity here:
    // Create a cpu_set_t object representing a set of CPUs. Clear it and mark only CPU "this_thread_data->tid" as set
    cpu_set_t cpuset;
    CPU_ZERO(&cpuset);
    CPU_SET(thread_id*2, &cpuset);
    int rc = pthread_setaffinity_np(pthread_self(), sizeof(cpu_set_t), &cpuset);
    if (rc != 0) {
        printf("Error calling pthread_setaffinity_np: %d\n", rc);
        exit(0);
    }

    bool *op_lock_mask = new bool[num_ops];
    if (!is_loading) {
        memset(op_lock_mask, 0, num_ops * sizeof(bool));
        int num_locked_ops = (num_ops * lockContentionProportion) / 100;
        for (int i = 0; i<num_locked_ops; i+=1) {
            op_lock_mask[i] = 1;
        }
        random_shuffle(op_lock_mask, op_lock_mask + num_ops);
    }

    // Warm up ------------------------------------------------
    start_barrier++;
    while (start_barrier != num_threads + 1) {
    }

    db->Init();
    ycsbc::Client client(*db, *wl);
    int oks = 0;
    for (int i = 0; i < num_ops; ++i) {
        if (is_loading) {
            oks += client.DoInsert();
        } else {
            //todo: pass value of op_lock_mask[i] as the function parameter
            oks += client.DoTransaction();
        }
    }
    db->Close();
    return oks;
}

int main(const int argc, const char *argv[]) {
    utils::Properties props;
    string file_name = LockOverheadParseCommandLine(argc, argv, props);
    const int num_threads = stoi(props.GetProperty("threadcount", "1"));
    const int lock_contention_proportion = stoi(props.GetProperty("lock_contention_proportion", "0"));

    //the next free cpu core
    cpu_set_t set;
    CPU_ZERO(&set);        // clear cpu mask
    CPU_SET((num_threads*2) + 2, &set);      // set cpu to the next free core
    sched_setaffinity(0, sizeof(cpu_set_t), &set);  // 0 is the calling process

    ycsbc::DB *db = ycsbc::DBFactory::CreateDB(props);
    if (!db) {
        cout << "Unknown database name " << props["dbname"] << endl;
        exit(0);
    }

    ycsbc::CoreWorkload wl;
    wl.Init(props);

    // Loads data
    vector <future<int>> actual_ops;
    int total_ops = stoi(props[ycsbc::CoreWorkload::RECORD_COUNT_PROPERTY]);
    for (int i = 0; i < num_threads; ++i) {
        actual_ops.emplace_back(async(launch::async, LockOverheadDelegateClient,
                db, &wl, total_ops / num_threads, true, lock_contention_proportion, num_threads, i));
    }
    assert((int) actual_ops.size() == num_threads);

    int sum = 0;
    for (auto &n : actual_ops) {
        assert(n.valid());
        sum += n.get();
    }
    cout << "# Loading records:\t" << sum << endl;

    // Peforms transactions
    actual_ops.clear();
    total_ops = stoi(props[ycsbc::CoreWorkload::OPERATION_COUNT_PROPERTY]);
    utils::Timer<double> timer;

    for (int i = 0; i < num_threads; ++i) {
        actual_ops.emplace_back(async(launch::async, LockOverheadDelegateClient,
                db, &wl, total_ops / num_threads, false, lock_contention_proportion, num_threads, i));
    }
    assert((int) actual_ops.size() == num_threads);

    // Warm up ------------------------------------------------
    // Main thread give his vote and check
    start_barrier++;
    while (start_barrier != num_threads + 1) {
    }

    timer.Start();
    sum = 0;
    for (auto &n : actual_ops) {
        assert(n.valid());
        sum += n.get();
    }
    double duration = timer.End();
    cout << "# Transaction throughput (KTPS)" << endl;
    cout << props["dbname"] << '\t' << file_name << '\t' << num_threads << '\t';
    cout << total_ops / duration / 1000 << endl;
    if (db) db->~DB();
}

string LockOverheadParseCommandLine(int argc, const char *argv[], utils::Properties &props) {
    int argindex = 1;
    string filename;
    while (argindex < argc && LockOverheadStrStartWith(argv[argindex], "-")) {
        if (strcmp(argv[argindex], "-threads") == 0) {
            argindex++;
            if (argindex >= argc) {
                LockOverheadUsageMessage(argv[0]);
                exit(0);
            }
            props.SetProperty("threadcount", argv[argindex]);
            argindex++;
        } else if (strcmp(argv[argindex], "-db") == 0) {
            argindex++;
            if (argindex >= argc) {
                LockOverheadUsageMessage(argv[0]);
                exit(0);
            }
            props.SetProperty("dbname", argv[argindex]);
            argindex++;
        } else if (strcmp(argv[argindex], "-type") == 0) {
            // -type is only with storedsDB, it denotes the inner data structures we will use
            argindex++;
            if (argindex >= argc) {
                LockOverheadUsageMessage(argv[0]);
                exit(0);
            }
            props.SetProperty("type", argv[argindex]);
            argindex++;
        } else if (strcmp(argv[argindex], "-dbpath") == 0) {
            // -dbpath is only with storedsDB, it shows where the db file is
            argindex++;
            if (argindex >= argc) {
                LockOverheadUsageMessage(argv[0]);
                exit(0);
            }
            props.SetProperty("dbpath", argv[argindex]);
            argindex++;
        } else if (strcmp(argv[argindex], "-host") == 0) {
            argindex++;
            if (argindex >= argc) {
                LockOverheadUsageMessage(argv[0]);
                exit(0);
            }
            props.SetProperty("host", argv[argindex]);
            argindex++;
        } else if (strcmp(argv[argindex], "-port") == 0) {
            argindex++;
            if (argindex >= argc) {
                LockOverheadUsageMessage(argv[0]);
                exit(0);
            }
            props.SetProperty("port", argv[argindex]);
            argindex++;
        } else if (strcmp(argv[argindex], "-slaves") == 0) {
            argindex++;
            if (argindex >= argc) {
                LockOverheadUsageMessage(argv[0]);
                exit(0);
            }
            props.SetProperty("slaves", argv[argindex]);
            argindex++;
        } else if (strcmp(argv[argindex], "-lcp") == 0) {
            // -lcp -> "lock contention proportion" is only necessary for lock overhead benchmarking
            argindex++;
            if (argindex >= argc) {
                LockOverheadUsageMessage(argv[0]);
                exit(0);
            }
            props.SetProperty("lock_contention_proportion", argv[argindex]);
            argindex++;
        } else if (strcmp(argv[argindex], "-P") == 0) {
            argindex++;
            if (argindex >= argc) {
                LockOverheadUsageMessage(argv[0]);
                exit(0);
            }
            filename.assign(argv[argindex]);
            ifstream input(argv[argindex]);
            try {
                props.Load(input);
            } catch (const string &message) {
                cout << message << endl;
                exit(0);
            }
            input.close();
            argindex++;
        } else {
            cout << "Unknown option '" << argv[argindex] << "'" << endl;
            exit(0);
        }
    }

    if (argindex == 1 || argindex != argc) {
        LockOverheadUsageMessage(argv[0]);
        exit(0);
    }

    return filename;
}

void LockOverheadUsageMessage(const char *command) {
    cout << "Usage: " << command << " [options]" << endl;
    cout << "Options:" << endl;
    cout << "  -db dbname: specify the name of the DB to use (default: storeds)" << endl;
    cout << "  -threads n: execute using n threads (default: 1)" << endl;
    cout << "  -dbpath dbpath: specify the name of the PMEM memory pool file to be created" << endl;
    cout << "  -type dsname: specify the name of the data-structure to use" << endl;
    cout << "  -P propertyfile: load properties from the given file. Multiple files can" << endl;
    cout << "                   be specified, and will be processed in the order specified" << endl;
}

inline bool LockOverheadStrStartWith(const char *str, const char *pre) {
    return strncmp(str, pre, strlen(pre)) == 0;
}

