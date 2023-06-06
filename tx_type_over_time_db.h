#ifndef TX_TYPE_OVER_TIME_DB_H
#define TX_TYPE_OVER_TIME_DB_H

#include <cstdint>
#include <iostream>
#include <map>

class TxOverTimeDb {
private:
    std::map<std::string, uint64_t*> m_map; //stores script script->occurences key->value map
public:
    TxOverTimeDb(/* args */);
    ~TxOverTimeDb();
    void insertOrUpdate(std::string script, const time_t atTime);
    void merge(TxOverTimeDb* foreignTxOverTimeDB);
    void saveContents(std::string filename);
    void saveContentsInverted(std::string filename);
    void saveContentsMonthly(std::string filename);
    void printStats();
};

#endif // TX_TYPE_OVER_TIME_DB_H