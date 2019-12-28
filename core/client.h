//
//  client.h
//  YCSB-C
//
//  Created by Jinglei Ren on 12/10/14.
//  Copyright (c) 2014 Jinglei Ren <jinglei@ren.systems>.
//

#ifndef YCSB_C_CLIENT_H_
#define YCSB_C_CLIENT_H_

#include <string>
#include "db.h"
#include "core_workload.h"
#include "utils.h"

namespace ycsbc {

    class Client {
    public:
        Client(DB &db, CoreWorkload &wl) : db_(db), workload_(wl) {}

        virtual bool DoInsert();

        virtual bool DoTransaction();

        virtual bool DoTransactionOffline();

        virtual bool DoTransactionOfflineV1(int idx);

        virtual ~Client() {}

    protected:

        virtual int TransactionRead();

        virtual int TransactionReadOffline();

        virtual int TransactionReadOfflineV1(const uint64_t &key);

        virtual int TransactionReadModifyWrite();

        virtual int TransactionScan();

        virtual int TransactionUpdate();

        virtual int TransactionUpdateOffline();

        virtual int TransactionUpdateOfflineV1(const uint64_t &key);

        virtual int TransactionInsert();

        virtual int TransactionInsertOffline();

        virtual int TransactionInsertOfflineV1(const uint64_t &key);

        DB &db_;
        CoreWorkload &workload_;
    };

    inline bool Client::DoInsert() {
        uint64_t key = workload_.NextSequenceKey();
        std::vector <DB::KVPair> pairs;
        workload_.BuildValues(pairs);
        return (db_.Insert(workload_.NextTable(), key, pairs) == DB::kOK);
    }

    inline bool Client::DoTransaction() {
        int status = -1;
        switch (workload_.NextOperation()) {
            case READ:
                status = TransactionRead();
                break;
            case UPDATE:
                status = TransactionUpdate();
                break;
            case INSERT:
                status = TransactionInsert();
                break;
            case SCAN:
                status = TransactionScan();
                break;
            case READMODIFYWRITE:
                status = TransactionReadModifyWrite();
                break;
            default:
                throw utils::Exception("Operation request is not recognized!");
        }
        assert(status >= 0);
        return (status == DB::kOK);
    }

    inline bool Client::DoTransactionOffline() {
        int status = -1;
        switch (workload_.NextOperation()) {
            case READ:
                status = TransactionReadOffline();
                break;
            case UPDATE:
                status = TransactionUpdateOffline();
                break;
            case INSERT:
                status = TransactionInsertOffline();
                break;
            default:
                throw utils::Exception("Operation request is not recognized!");
        }
        assert(status >= 0);
        return (status == DB::kOK);
    }

    inline bool Client::DoTransactionOfflineV1(int idx) {
        int status = -1;
        struct offline_ops current_op = workload_.GetOperationByIndex(idx);
        switch (current_op.operation) {
            case READ:
                status = TransactionReadOfflineV1(current_op.key);
                break;
            case UPDATE:
                status = TransactionUpdateOfflineV1(current_op.key);
                break;
            case INSERT:
                status = TransactionInsertOfflineV1(current_op.key);
                break;
            default:
                throw utils::Exception("Operation request is not recognized!");
        }
        assert(status >= 0);
        return (status == DB::kOK);
    }

    inline int Client::TransactionRead() {
        const std::string &table = workload_.NextTable();
        const uint64_t &key = workload_.NextTransactionKey();
        std::vector <DB::Kuint64VstrPair> result;
        if (!workload_.read_all_fields()) {
            std::vector <std::string> fields;
            fields.push_back("field" + workload_.NextFieldName());
            return db_.Read(table, key, &fields, result);
        } else {
            return db_.Read(table, key, NULL, result);
        }
    }

    inline int Client::TransactionReadOffline() {
        const std::string &table = workload_.NextTable();
        const uint64_t &key = workload_.NextTransactionKeyOffline();
        std::vector <DB::Kuint64VstrPair> result;
        return db_.Read(table, key, NULL, result);
    }

    inline int Client::TransactionReadOfflineV1(const uint64_t &key) {
        const std::string &table = workload_.NextTable();
        //const uint64_t &key = workload_.NextTransactionKeyOfflineV1(idx);
        std::vector <DB::Kuint64VstrPair> result;
        return db_.Read(table, key, NULL, result);
    }

    inline int Client::TransactionReadModifyWrite() {
        const std::string &table = workload_.NextTable();
        const uint64_t &key = workload_.NextTransactionKey();
        std::vector <DB::Kuint64VstrPair> result;

        if (!workload_.read_all_fields()) {
            std::vector <std::string> fields;
            fields.push_back("field" + workload_.NextFieldName());
            db_.Read(table, key, &fields, result);
        } else {
            db_.Read(table, key, NULL, result);
        }

        std::vector <DB::KVPair> values;
        if (workload_.write_all_fields()) {
            workload_.BuildValues(values);
        } else {
            workload_.BuildUpdate(values);
        }
        return db_.Update(table, key, values);
    }

    inline int Client::TransactionScan() {
        const std::string &table = workload_.NextTable();
        const uint64_t &key = workload_.NextTransactionKey();
        int len = workload_.NextScanLength();
        std::vector <std::vector<DB::Kuint64VstrPair>> result;
        if (!workload_.read_all_fields()) {
            std::vector <std::string> fields;
            fields.push_back("field" + workload_.NextFieldName());
            return db_.Scan(table, key, len, &fields, result);
        } else {
            return db_.Scan(table, key, len, NULL, result);
        }
    }

    inline int Client::TransactionUpdate() {
        const std::string &table = workload_.NextTable();
        const uint64_t &key = workload_.NextTransactionKey();
        std::vector <DB::KVPair> values;
        if (workload_.write_all_fields()) {
            workload_.BuildValues(values);
        } else {
            workload_.BuildUpdate(values);
        }
        return db_.Update(table, key, values);
    }

    inline int Client::TransactionUpdateOffline() {
        const std::string &table = workload_.NextTable();
        const uint64_t &key = workload_.NextTransactionKeyOffline();
        std::vector <DB::KVPair> values;
        if (workload_.write_all_fields()) {
            workload_.BuildValuesOffline(values);
        } else {
            workload_.BuildUpdateOffline(values);
        }
        return db_.Update(table, key, values);
    }

    inline int Client::TransactionUpdateOfflineV1(const uint64_t &key) {
        const std::string &table = workload_.NextTable();
        //const uint64_t &key = workload_.NextTransactionKeyOfflineV1(idx);
        std::vector <DB::KVPair> values;
        if (workload_.write_all_fields()) {
            workload_.BuildValuesOffline(values);
        } else {
            workload_.BuildUpdateOffline(values);
        }
        return db_.Update(table, key, values);
    }

    inline int Client::TransactionInsert() {
        const std::string &table = workload_.NextTable();
        const uint64_t &key = workload_.NextSequenceKey();
        std::vector <DB::KVPair> values;
        workload_.BuildValues(values);
        return db_.Insert(table, key, values);
    }

    inline int Client::TransactionInsertOffline() {
        const std::string &table = workload_.NextTable();
        const uint64_t &key = workload_.NextSequenceKeyOffline();
        std::vector <DB::KVPair> values;
        workload_.BuildValuesOffline(values);
        return db_.Insert(table, key, values);
    }

    inline int Client::TransactionInsertOfflineV1(const uint64_t &key) {
        const std::string &table = workload_.NextTable();
        //const uint64_t &key = workload_.NextSequenceKeyOfflineV1(idx);
        std::vector <DB::KVPair> values;
        workload_.BuildValuesOffline(values);
        return db_.Insert(table, key, values);
    }

} // ycsbc

#endif // YCSB_C_CLIENT_H_
