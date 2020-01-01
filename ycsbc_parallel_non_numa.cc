//
//  ycsbc.cc
//  YCSB-C
//
//  Created by Jinglei Ren on 12/19/14.
//  Copyright (c) 2014 Jinglei Ren <jinglei@ren.systems>.
//

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

void NonNumaParallelUsageMessage(const char *command);

bool NonNumaParallelStrStartWith(const char *str, const char *pre);

string NonNumaParallelParseCommandLine(int argc, const char *argv[], utils::Properties &props);

int NonNumaParallelDelegateClient(ycsbc::DB *db, ycsbc::CoreWorkload *wl, const int num_ops, bool is_loading, int thread_id) {
    // Add cpu affinity here:
    // Create a cpu_set_t object representing a set of CPUs. Clear it and mark only CPU "this_thread_data->tid" as set
    cpu_set_t cpuset;
    CPU_ZERO(&cpuset);
    CPU_SET((thread_id * 2) + 1, &cpuset);
    int rc = pthread_setaffinity_np(pthread_self(), sizeof(cpu_set_t), &cpuset);
    if (rc != 0) {
        printf("Error calling pthread_setaffinity_np: %d\n", rc);
        exit(0);
    }

    db->Init();
    ycsbc::Client client(*db, *wl);
    int oks = 0;
    for (int i = 0; i < num_ops; ++i) {
        if (is_loading) {
            oks += client.DoInsert();
        } else {
            oks += client.DoTransactionOfflineV1(i);
        }
    }
    db->Close();
    return oks;
}

int main(const int argc, const char *argv[]) {
    utils::Properties props;
    string file_name = NonNumaParallelParseCommandLine(argc, argv, props);

    const int num_threads = stoi(props.GetProperty("threadcount", "1"));
    const string dbpath = props.GetProperty("dbpath", "/pmem/array");
    const string db_file_extension = ".pmem";
    const string type = props.GetProperty("type", "dram");
    const bool is_vmem = (type.substr(type.size() - 4).compare("vmem") == 0);

    ycsbc::DB *db_list[num_threads];
    ycsbc::CoreWorkload wl_list[num_threads];

    for(int t=0; t<num_threads; t+=1) {
        if(!is_vmem) props.SetProperty("dbpath", dbpath + to_string(t) + db_file_extension);
        db_list[t] = ycsbc::DBFactory::CreateDB(props);
        wl_list[t].Init(props);

        if (!db_list[t]) {
            cout << "Unknown database name " << props["dbname"] << endl;
            exit(0);
        }
    }

    // Loads data
    vector <future<int>> actual_ops;
    int total_ops = stoi(props[ycsbc::CoreWorkload::RECORD_COUNT_PROPERTY]);
    for (int i = 0; i < num_threads; ++i) {
        actual_ops.emplace_back(async(launch::async, NonNumaParallelDelegateClient, db_list[i], &wl_list[i], total_ops, true, i));
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
    for(int t=0; t<num_threads; t+=1) {
        wl_list[t].PrepareOfflineDataV1(total_ops);
    }
    utils::Timer<double> timer;
    timer.Start();
    for (int i = 0; i < num_threads; ++i) {
        actual_ops.emplace_back(async(launch::async, NonNumaParallelDelegateClient, db_list[i], &wl_list[i], total_ops, false, i));
    }
    assert((int) actual_ops.size() == num_threads);

    sum = 0;
    for (auto &n : actual_ops) {
        assert(n.valid());
        sum += n.get();
    }
    double duration = timer.End();
    cout << "# Transaction records:\t" << sum << endl;
    cout << "# Transaction throughput (KTPS)" << endl;
    cout << props["dbname"] << '\t' << file_name << '\t' << num_threads << '\t';
    cout << (total_ops * num_threads) / duration / 1000 << endl;

    for(int t = 0; t<num_threads; t+=1) {
        if (db_list[t]) db_list[t]->~DB();
    }
}

string NonNumaParallelParseCommandLine(int argc, const char *argv[], utils::Properties &props) {
    int argindex = 1;
    string filename;
    while (argindex < argc && NonNumaParallelStrStartWith(argv[argindex], "-")) {
        if (strcmp(argv[argindex], "-threads") == 0) {
            argindex++;
            if (argindex >= argc) {
                NonNumaParallelUsageMessage(argv[0]);
                exit(0);
            }
            props.SetProperty("threadcount", argv[argindex]);
            argindex++;
        } else if (strcmp(argv[argindex], "-db") == 0) {
            argindex++;
            if (argindex >= argc) {
                NonNumaParallelUsageMessage(argv[0]);
                exit(0);
            }
            props.SetProperty("dbname", argv[argindex]);
            argindex++;
        } else if (strcmp(argv[argindex], "-type") == 0) {
            // -type is only with storedsDB, it denotes the inner data structures we will use
            argindex++;
            if (argindex >= argc) {
                NonNumaParallelUsageMessage(argv[0]);
                exit(0);
            }
            props.SetProperty("type", argv[argindex]);
            argindex++;
        } else if (strcmp(argv[argindex], "-dbpath") == 0) {
            // -dbpath is only with storedsDB, it shows where the db file is
            argindex++;
            if (argindex >= argc) {
                NonNumaParallelUsageMessage(argv[0]);
                exit(0);
            }
            props.SetProperty("dbpath", argv[argindex]);
            argindex++;
        } else if (strcmp(argv[argindex], "-host") == 0) {
            argindex++;
            if (argindex >= argc) {
                NonNumaParallelUsageMessage(argv[0]);
                exit(0);
            }
            props.SetProperty("host", argv[argindex]);
            argindex++;
        } else if (strcmp(argv[argindex], "-port") == 0) {
            argindex++;
            if (argindex >= argc) {
                NonNumaParallelUsageMessage(argv[0]);
                exit(0);
            }
            props.SetProperty("port", argv[argindex]);
            argindex++;
        } else if (strcmp(argv[argindex], "-slaves") == 0) {
            argindex++;
            if (argindex >= argc) {
                NonNumaParallelUsageMessage(argv[0]);
                exit(0);
            }
            props.SetProperty("slaves", argv[argindex]);
            argindex++;
        } else if (strcmp(argv[argindex], "-P") == 0) {
            argindex++;
            if (argindex >= argc) {
                NonNumaParallelUsageMessage(argv[0]);
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
        NonNumaParallelUsageMessage(argv[0]);
        exit(0);
    }

    return filename;
}

void NonNumaParallelUsageMessage(const char *command) {
    cout << "Usage: " << command << " [options]" << endl;
    cout << "Options:" << endl;
    cout << "  -threads n: execute using n threads (default: 1)" << endl;
    cout << "  -db dbname: specify the name of the DB to use (default: basic)" << endl;
    cout << "  -P propertyfile: load properties from the given file. Multiple files can" << endl;
    cout << "                   be specified, and will be processed in the order specified" << endl;
}

inline bool NonNumaParallelStrStartWith(const char *str, const char *pre) {
    return strncmp(str, pre, strlen(pre)) == 0;
}