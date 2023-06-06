#ifndef TX_TYPE_DB_H
#define TX_TYPE_DB_H

#include <cstdint>
#include <iostream>
#include <map>

class TxScriptDb {
private:
    std::map<std::string, uint64_t> m_map; //stores script script->occurences key->value map
public:
    TxScriptDb(/* args */);
    ~TxScriptDb();
    void insertOrUpdate(std::string script);
    void merge(TxScriptDb* foreignTxScriptDB);
    void printContents();
    void saveCompleteStatsOrdered(std::string filename);
    void printStats();
};

#endif // TX_TYPE_DB_H