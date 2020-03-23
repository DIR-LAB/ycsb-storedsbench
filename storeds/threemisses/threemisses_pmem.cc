//
// Created by Islam, Abdullah Al Raqibul on 10/15/19.
//

#include <iostream>
#include <sstream>
#include <random>
#include <time.h>

#include "../../Headers/UMVMCommon.h"
using namespace UMVM;
using namespace std;

namespace ycsbc {
    class ThreeMissesPmem : public StoredsBase {
    public:
        ThreeMissesPmem(const char *path) {
            ThreeMissesPmem::init(path);
        }

        int init(const char *path);

        int read(const uint64_t key, void *&result);

        int scan(const uint64_t key, int len, std::vector <std::vector<DB::Kuint64VstrPair>> &result);

        int update(const uint64_t key, void *value);

        int insert(const uint64_t key, void *value);

        void destroy();

        ~ThreeMissesPmem();

    private:
        /* Private Data */
        ThreeMisses *TestMap;
    };

    /*
     * init -- hash-list initializer
     */
    int ThreeMissesPmem::init(const char *path) {
        string IndexFileName = "/mnt/PM0/idx.pmem";
        string DataFileName = "/mnt/PM0/data.pmem";
        const int64_t count = 10000000 * 2;

        remove(IndexFileName.c_str());
        remove(DataFileName.c_str());

        UserModeVirtualMemoryFile::ThreeMissesFileOptions Options(IndexFileName, 30*count);
        TestMap = new ThreeMisses(Options, DataFileName);

        return 1;
    }

    int ThreeMissesPmem::scan(const uint64_t key, int len, std::vector <std::vector<DB::Kuint64VstrPair>> &result) {
        throw "Scan: function not implemented!";
    }

    /**
     * read -- read 'value' of 'key' and place it into '&result'
     */
    int ThreeMissesPmem::read(const uint64_t key, void *&result) {
	uint64_t keytemp = key;
        AccessVectorChar KeyAV(reinterpret_cast<char*>(&keytemp));
        AccessVectorChar StoredValueAV = TestMap->GetValue(KeyAV);
        result = StoredValueAV.data();
        return 1;
    }

    /**
     * update -- update 'value' of 'key' into the hash-list, will insert the 'value' if 'key' not exists
     */
    int ThreeMissesPmem::update(const uint64_t key, void *value) {
	uint64_t keytemp = key;
        AccessVectorChar KeyAV(reinterpret_cast<char*>(&keytemp));

        char* valueptr = reinterpret_cast<char*>(value);
        AccessVectorChar DataAV(valueptr,strlen(valueptr));

        AccessVectorChar StoredValueAV = TestMap->GetValue(KeyAV);

        TestMap->UpdateRecordField(StoredValueAV, 0, DataAV);
        return 1;
    }

    /**
     * insert -- inserts 'value' into the hash-list, will update the 'value' if 'key' already exists
     */
    int ThreeMissesPmem::insert(const uint64_t key, void *value) {
        //std::cout << key << " " << value << std::endl;
	uint64_t keytemp = key;
        AccessVectorChar KeyAV(reinterpret_cast<char*>(&keytemp));
        
        static string DataBuffer((char *) value);
        AccessVectorChar DataAV(DataBuffer);
        TestMap->AppendRecord(KeyAV, DataAV);
        return 1;
    }

    /**
     * free -- destructor
     */
    void ThreeMissesPmem::destroy() {
    }
}   //ycsbc
