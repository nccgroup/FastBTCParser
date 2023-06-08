// Released as open source by NCC Group Plc - https://www.nccgroup.com/
// https://github.com/nccgroup/FastBTCParser

#include "tx_type_db.h"
#include <iterator>
#include <iostream>
#include <fstream>
#include <iomanip>

TxScriptDb::TxScriptDb(/* args */) {
}

TxScriptDb::~TxScriptDb() {
}

void TxScriptDb::insertOrUpdate(std::string script) {
    if(m_map.count(script) > 0) {
        m_map[script] += 1;
    } else {
        m_map[script] = 1;
    }
}

void TxScriptDb::merge(TxScriptDb *foreignTxScriptDB){
    std::map<std::string, uint64_t>::iterator itr;
    for (itr = foreignTxScriptDB->m_map.begin(); itr != foreignTxScriptDB->m_map.end(); ++itr) {
        if(m_map.count(itr->first) > 0) {
            m_map[itr->first] += itr->second;
        } else {
            m_map[itr->first] = itr->second;
        }
    }
}

void TxScriptDb::printContents(){
    uint64_t totalOutputs = 0;
    std::map<std::string, uint64_t>::iterator itr;
    std::cout << "  #OCCURENCES ITEM" << std::endl;
    for (itr = m_map.begin(); itr != m_map.end(); ++itr) {
        std::cout << std::setw(13) << itr->second 
                  << " " << itr->first << std::endl;
        totalOutputs += itr->second;
    }
    std::cout << "found " << m_map.size() << " types of item in " << totalOutputs << " tx outputs." << std::endl;
}

void TxScriptDb::saveCompleteStatsOrdered(std::string filename) {
    uint64_t previousMin = 0;
    uint64_t currentMin = UINT64_MAX;
    uint64_t totalOutputs = 0;
    std::map<std::string, uint64_t>::iterator itr;

    std::ofstream outdata;
    outdata.open(filename, std::fstream::out | std::fstream::trunc);
    if( !outdata ) { // file couldn't be opened
        std::cerr << "Error: file could not be opened" << std::endl;
        exit(1);
    }

    outdata << "  #OCCURENCES ITEM" << std::endl;

    do {
        for (itr = m_map.begin(); itr != m_map.end(); ++itr) {
            if((itr->second < currentMin)&&(itr->second > previousMin)) {
                currentMin = itr->second;
            }
        }
        for (itr = m_map.begin(); itr != m_map.end(); ++itr) {
            if(itr->second == currentMin) {
                outdata << std::setw(13) << itr->second 
                    << " " << itr->first << std::endl;
                    totalOutputs += itr->second;
            }
        }
        previousMin = currentMin;
        currentMin = UINT64_MAX;
    } while (previousMin != UINT64_MAX);

    outdata << "found " << m_map.size() << " types of item in " << totalOutputs << " tx outputs." << std::endl;
    outdata.close();
    std::cout << "Results written to " << filename << std::endl; 
}

void TxScriptDb::printStats() {
    uint64_t totalOutputs = 0;
    std::map<std::string, uint64_t>::iterator itr;
    for (itr = m_map.begin(); itr != m_map.end(); ++itr) {
        totalOutputs += itr->second;
    }
    std::cout << "found " << m_map.size() << " types of item in " << totalOutputs << " tx outputs." << std::endl;
}
