// Released as open source by NCC Group Plc - https://www.nccgroup.com/
// https://github.com/nccgroup/FastBTCParser

#ifndef BLOCK_H
#define BLOCK_H

#include <cstdint>
#include <string>
#include "tx_type_db.h"
#include "tx_type_over_time_db.h"

unsigned int swapIntEndianness2b(const char * smallEndianInt);
unsigned int swapIntEndianness3b(const char * smallEndianInt);
unsigned int swapIntEndianness4b(const char * smallEndianInt);
uint64_t swapIntEndianness5b(const char * smallEndianInt);
uint64_t swapIntEndianness6b(const char * smallEndianInt);
uint64_t swapIntEndianness7b(const char * smallEndianInt);
uint64_t swapIntEndianness8b(const char * smallEndianInt);
uint64_t getVarInt(const char* buffer, int* byteLen); //byteLen a return parameter for the length of the varint so that we know how much to skip ahead to continue reading
std::string rawToHex(const unsigned char * data, uint64_t size);
char decToHex(int dec);
void hashEndianSwap(unsigned char * hash);

class TX {
public:
    TX(char * txData, unsigned int blockTime);
    TX();
    ~TX();
    void initFromASCII(char* asciiString);
    unsigned int getNextTxOffset();
    void processScriptPubKeys(TxScriptDb* scriptDB, TxOverTimeDb* scriptOverTimeDB);
    void processSegregatedWitnesses(TxScriptDb* segWitDB, char * ordinalsFolder);
    void scriptPrintFull(int offset, int size);
    void scriptPrintShort(int offset, int size);
   
    void getTXID(unsigned char * txid);
    void getWTXID(unsigned char * wtxid);
    bool isSegWitTX();
private:
    void processScript(int offset, int size, TxScriptDb* scriptDB, TxOverTimeDb* scriptOverTimeDB);
    std::string scriptString(int offset, int size);
    std::string extractString(uint64_t offset, uint64_t size);
    bool isOrdinal(int offset, int size);
    void saveOrdinal(std::string dataType, uint64_t offset, uint64_t size, char * ordinalsFolder);
    char* m_data;
    unsigned int m_length;
    unsigned int m_inputCount;
    unsigned int* m_inputStartOffsets;
    unsigned int m_outputCount;
    unsigned int* m_outputStartOffsets;
    unsigned int m_totalOffset;
    bool m_hasWitness;
    unsigned int m_segWitCount;
    unsigned int* m_segWitOffsets;
    char * m_version;
    char * m_lockingTime; 
    unsigned int m_blockTime;
};

class Block {
public:
    Block(char * blockData);
    Block();
    ~Block();
    void processTx(TxScriptDb* scriptDB, TxOverTimeDb* scriptOverTimeDB, TxScriptDb* segWitDB, char * ordinalsFolder);
    bool verifyMerkleRoot();
    void initFromASCII(char* asciiString);
    int getNextBlockOffset();
    int getTxCount();
    unsigned int getVersion();
    unsigned int getTime();
    void getBlockHash(unsigned char * hash);
private:
    char* m_data;
    int m_length;
    int m_txCount;
    unsigned int m_version;
    unsigned int m_time;
};

#endif // BLOCK_H