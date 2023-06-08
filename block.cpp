// Released as open source by NCC Group Plc - https://www.nccgroup.com/
// https://github.com/nccgroup/FastBTCParser

#include "block.h"

#include "script_opcodes.h"
#include "file_type_db.h"

#include <iostream>
#include <string.h>
#include <string>
#include <cstdint>
#include <stdio.h>

#include <openssl/sha.h>

TX::TX(char *txData, unsigned int blockTime) {
    m_data = nullptr;
    m_length = 0;
    m_inputCount = 0;
    m_inputStartOffsets = nullptr;
    m_outputCount = 0;
    m_outputStartOffsets = nullptr;
    m_totalOffset = 0;
    m_hasWitness = false;
    m_segWitCount = 0;
    m_segWitOffsets = nullptr;
    m_blockTime = blockTime;
    initFromASCII(txData);
}

TX::TX() {
    m_data = nullptr;
    m_length = 0;
    m_inputCount = 0;
    m_inputStartOffsets = nullptr;
    m_outputCount = 0;
    m_outputStartOffsets = nullptr;
    m_totalOffset = 0;
    m_hasWitness = false;
    m_segWitCount = 0;
    m_segWitOffsets = nullptr;
}

TX::~TX() {
    if(nullptr != m_inputStartOffsets) {
        delete m_inputStartOffsets;
    }
    if(nullptr != m_outputStartOffsets) {
        delete m_outputStartOffsets;
    }
    if(nullptr != m_segWitOffsets) {
        delete m_segWitOffsets;
    }
}

void TX::initFromASCII(char *asciiString) {
    if(nullptr != m_inputStartOffsets) {
        delete m_inputStartOffsets;
        m_inputStartOffsets = nullptr;
    }
    if(nullptr != m_outputStartOffsets) {
        delete m_outputStartOffsets;
        m_outputStartOffsets = nullptr;
    }
    if(nullptr != m_segWitOffsets) {
        delete m_segWitOffsets;
        m_segWitOffsets = nullptr;
    }

    int byteLen = 0;
    m_totalOffset = 0;
    m_outputCount = 0;

    m_version = asciiString; //we keep a marker to reuse if we need to calculate the TXID

    m_inputCount = getVarInt(asciiString + 4, &byteLen); // we skip the 4 version bytes
    m_totalOffset = 4 + byteLen;

    unsigned int witStackItems = 0;


    if(m_inputCount > 0) {
        m_inputStartOffsets = new unsigned int[m_inputCount];

        for(unsigned int i = 0; i < m_inputCount; ++i) {
            m_inputStartOffsets[i] = m_totalOffset;
            m_totalOffset += 36;
            m_totalOffset += getVarInt(asciiString + m_totalOffset, &byteLen);
            m_totalOffset += byteLen + 4;
        } 

        m_outputCount = getVarInt(asciiString + m_totalOffset, &byteLen);
        m_totalOffset += byteLen;

        m_outputStartOffsets = new unsigned int[m_outputCount];

        for(unsigned int i = 0; i < m_outputCount; ++i) {
            m_outputStartOffsets[i] = m_totalOffset;
            m_totalOffset += 8;
            m_totalOffset += getVarInt(asciiString + m_totalOffset, &byteLen);
            m_totalOffset += byteLen;
        } 

        m_lockingTime = asciiString + m_totalOffset;

        m_totalOffset += 4;

        m_data = asciiString;
        
    } else { //zero means we hit a Segregated Witness, transaction processing is a little different then 
        m_hasWitness = true;
        m_totalOffset += 1; //we skip the flag byte (which should be non-zero)
        m_inputCount = getVarInt(asciiString + m_totalOffset, &byteLen); // let's read the real actual input number
        m_totalOffset += byteLen; 

        m_inputStartOffsets = new unsigned int[m_inputCount];

        for(unsigned int i = 0; i < m_inputCount; ++i) {
            m_inputStartOffsets[i] = m_totalOffset;
            m_totalOffset += 36; // txid + vout
            m_totalOffset += getVarInt(asciiString + m_totalOffset, &byteLen);
            m_totalOffset += byteLen + 4;
        } 

        m_outputCount = getVarInt(asciiString + m_totalOffset, &byteLen);
        m_totalOffset += byteLen;

        m_outputStartOffsets = new unsigned int[m_outputCount];

        for(unsigned int i = 0; i < m_outputCount; ++i) {
            m_outputStartOffsets[i] = m_totalOffset;
            m_totalOffset += 8; //value in satoshis
            m_totalOffset += getVarInt(asciiString + m_totalOffset, &byteLen);
            m_totalOffset += byteLen;
        } 

        //now for the segregated witnesses

        m_segWitCount = m_inputCount; //there are exactly as many segwit as inputs (but witnesses can be empty) 
        
        m_segWitOffsets = new unsigned int[m_segWitCount];

        for(unsigned int i = 0; i < m_segWitCount; ++i) {
            m_segWitOffsets[i] = m_totalOffset;
            witStackItems = getVarInt(asciiString + m_totalOffset, &byteLen);
            m_totalOffset += byteLen;
            for(unsigned int j = 0; j < witStackItems; ++j) {
                unsigned int segWitSize = getVarInt(asciiString + m_totalOffset, &byteLen); 
                m_totalOffset += segWitSize;
                m_totalOffset += byteLen;
            }
        } 

        m_lockingTime = asciiString + m_totalOffset;
        m_totalOffset += 4; // lock time

        m_data = asciiString;
    }
}

unsigned int TX::getNextTxOffset() {
    return m_totalOffset;
}

void TX::processScriptPubKeys(TxScriptDb* scriptDB, TxOverTimeDb* scriptOverTimeDB) {
    int byteLen;
    int scriptPubKeySize;
    int offset = 0;
    for(unsigned int i = 0; i < m_outputCount; ++i) {
        scriptPubKeySize = getVarInt(m_data+m_outputStartOffsets[i]+8,&byteLen);
        offset = m_outputStartOffsets[i]+8+byteLen;
        processScript(offset, scriptPubKeySize, scriptDB, scriptOverTimeDB);
    }
}

void TX::processSegregatedWitnesses(TxScriptDb *segWitDB, char * ordinalsFolder)
{
    int byteLen;
    int offset = 0;
    unsigned int witStackItems = 0;
    for(unsigned int i = 0; i < m_segWitCount; ++i) {
        witStackItems = getVarInt(m_data + m_segWitOffsets[i], &byteLen);
        offset = m_segWitOffsets[i] + byteLen;
        for(unsigned int j = 0; j < witStackItems; ++j) {
            unsigned int segWitSize = getVarInt(m_data + offset, &byteLen); 
            offset += byteLen;
            if (isOrdinal(offset, segWitSize)) {
                uint64_t offset2 = 0;
                uint64_t size2 = 0;
                bool success = true;
                uint64_t dataTypeSize = 0;
                std::string dataType = "";
                //there are different types of ordinals, extracting the type is first order of business
                //need to check size to prevent issues where we read out of the segwit
                if((((opCodes)(m_data[(int64_t)offset+0x28])) == OP_DATA_1) && (((opCodes)(m_data[(int64_t)offset+0x29])) == OP_DATA_1)) {
                    dataTypeSize = (uint8_t)m_data[(int64_t)offset+0x2a];
                    dataType = extractString(offset+0x2b, dataTypeSize);
                    offset2 = offset + 0x2b + dataTypeSize;
                } else if (((opCodes)(m_data[(int64_t)offset+0x28])) == OP_1) {
                    if((((opCodes)(m_data[(int64_t)offset+0x29])) >= OP_DATA_1) && (((opCodes)(m_data[(int64_t)offset+0x29])) <= OP_DATA_75)) {
                        dataTypeSize = (uint8_t)m_data[(int64_t)offset+0x29];
                        dataType = extractString(offset+0x2a, dataTypeSize);
                        offset2 = offset + 0x2a + dataTypeSize;
                    } else if (((opCodes)(m_data[(int64_t)offset+0x29])) == OP_PUSHDATA1) {
                        dataTypeSize = (uint8_t)m_data[(int64_t)offset+0x2a];
                        dataType = extractString(offset+0x2a + 1, dataTypeSize);
                        offset2 = offset + 0x2a + 1 + dataTypeSize;
                    } else if (((opCodes)(m_data[(int64_t)offset+0x29])) == OP_PUSHDATA2) {
                        dataTypeSize = swapIntEndianness2b(m_data+offset+0x2a);
                        dataType = extractString(offset+0x2a + 2, dataTypeSize);
                        offset2 = offset + 0x2a + 2 + dataTypeSize;
                    } else if (((opCodes)(m_data[(int64_t)offset+0x29])) == OP_PUSHDATA4) {
                        dataTypeSize = swapIntEndianness4b(m_data+offset+0x2a);
                        dataType = extractString(offset+0x2a + 4, dataTypeSize);
                        offset2 = offset + 0x2a + 4 + dataTypeSize;
                    } else {
                        success = false;
                    }
                } else if ((m_data[(int64_t)offset+0x28]) == OP_DATA_6) {
                    if ((m_data[(int64_t)offset+0x2f]) == OP_DATA_6) {
                        if((((opCodes)(m_data[(int64_t)offset+0x36])) == OP_DATA_1) && (((opCodes)(m_data[(int64_t)offset+0x37])) == OP_DATA_1)) {
                            dataTypeSize = (uint8_t)m_data[(int64_t)offset+0x38];
                            dataType = extractString(offset+0x39, dataTypeSize);
                            offset2 = offset + 0x39 + dataTypeSize;
                        } else {
                            success = false;
                        }
                    } else {
                        success = false;
                    }
                }
                else {
                    success = false;
                }

                if (success == true) {
                    size2 = segWitSize - (offset2 - offset);
                    segWitDB->insertOrUpdate(dataType);
                    saveOrdinal(dataType, offset2, size2, ordinalsFolder);
                }
            }

            offset += segWitSize;
        }
    } 
}

void TX::scriptPrintFull(int offset, int size) {
    std::cout << scriptString(offset, size) << std::endl;
}

void TX::scriptPrintShort(int offset, int size) {
    std::cout << scriptString(offset, size) << std::endl;
}

void TX::getTXID(unsigned char *txid) {
    if(m_hasWitness == true) {
        unsigned char* tmpData;
        tmpData = new unsigned char[m_segWitOffsets[0] + 2];//txdata - 1 byte marker - 1 byte flag + 4 bytes locking time
        memcpy(tmpData, m_version, 4);
        memcpy(tmpData + 4, m_data + 6, m_segWitOffsets[0] - 6); //we skip version, marker and flag
        memcpy(tmpData + m_segWitOffsets[0] - 2, m_lockingTime, 4); 
        SHA256(tmpData,m_segWitOffsets[0] + 2, txid);// 
        SHA256(txid, SHA256_DIGEST_LENGTH, txid);
        delete[] tmpData;
    } else {
        SHA256((unsigned char * )(m_data), m_totalOffset, txid);
        SHA256(txid, SHA256_DIGEST_LENGTH, txid);
    }
}

void TX::getWTXID(unsigned char *wtxid) {
    SHA256((unsigned char * )(m_data), m_totalOffset, wtxid);
    SHA256((unsigned char * )(wtxid), SHA256_DIGEST_LENGTH, wtxid);
}

bool TX::isSegWitTX() {
    return m_hasWitness;
}

void TX::processScript(int offset, int size, TxScriptDb *scriptDB, TxOverTimeDb* scriptOverTimeDB) {
    std::string script = scriptString(offset, size);
    scriptDB->insertOrUpdate(script);
    scriptOverTimeDB->insertOrUpdate(script, m_blockTime);
}

std::string TX::scriptString(int offset, int size) {
    std::string scriptString = "";
    unsigned int tempDataSize = 0;
    int ifcount = 0;
    for(int64_t i = 0; i < size; ++i) {
        switch ((opCodes)(m_data[(int64_t)offset+i])) {
            case OP_DATA_1:
            case OP_DATA_2:
            case OP_DATA_3:
            case OP_DATA_4:
            case OP_DATA_5:
            case OP_DATA_6:
            case OP_DATA_7:
            case OP_DATA_8:
            case OP_DATA_9:
            case OP_DATA_10:
            case OP_DATA_11:
            case OP_DATA_12:
            case OP_DATA_13:
            case OP_DATA_14:
            case OP_DATA_15:
            case OP_DATA_16:
            case OP_DATA_17:
            case OP_DATA_18:
            case OP_DATA_19:
            case OP_DATA_20:
            case OP_DATA_21:
            case OP_DATA_22:
            case OP_DATA_23:
            case OP_DATA_24:
            case OP_DATA_25:
            case OP_DATA_26:
            case OP_DATA_27:
            case OP_DATA_28:
            case OP_DATA_29:
            case OP_DATA_30:
            case OP_DATA_31:
            case OP_DATA_32:
            case OP_DATA_33:
            case OP_DATA_34:
            case OP_DATA_35:
            case OP_DATA_36:
            case OP_DATA_37:
            case OP_DATA_38:
            case OP_DATA_39:
            case OP_DATA_40:
            case OP_DATA_41:
            case OP_DATA_42:
            case OP_DATA_43:
            case OP_DATA_44:
            case OP_DATA_45:
            case OP_DATA_46:
            case OP_DATA_47:
            case OP_DATA_48:
            case OP_DATA_49:
            case OP_DATA_50:
            case OP_DATA_51:
            case OP_DATA_52:
            case OP_DATA_53:
            case OP_DATA_54:
            case OP_DATA_55:
            case OP_DATA_56:
            case OP_DATA_57:
            case OP_DATA_58:
            case OP_DATA_59:
            case OP_DATA_60:
            case OP_DATA_61:
            case OP_DATA_62:
            case OP_DATA_63:
            case OP_DATA_64:
            case OP_DATA_65:
            case OP_DATA_66:
            case OP_DATA_67:
            case OP_DATA_68:
            case OP_DATA_69:
            case OP_DATA_70:
            case OP_DATA_71:
            case OP_DATA_72:
            case OP_DATA_73:
            case OP_DATA_74:
            case OP_DATA_75:
                scriptString += std::string(opCodesNames[(uint8_t)m_data[offset+i]]) + " <data> ";
                i+=(unsigned int)((unsigned char)m_data[offset+i]);
                break;

            case OP_PUSHDATA1: //next byte holds size of data
                scriptString += std::string(opCodesNames[(uint8_t)m_data[offset+i]]) + " <data> ";
                tempDataSize = (unsigned int)((unsigned char)m_data[offset+i+1]);
                i += tempDataSize + 1;
                break;
            case OP_PUSHDATA2: //next 2 bytes hold size of data
                scriptString += std::string(opCodesNames[(uint8_t)m_data[offset+i]]) + " <data> ";
                tempDataSize = swapIntEndianness2b(m_data+offset+i+1);
                i += tempDataSize + 2;
                break;
            case OP_PUSHDATA4: //next 4 bytes hold size of data
                scriptString += std::string(opCodesNames[(uint8_t)m_data[offset+i]]) + " <data> ";
                tempDataSize = swapIntEndianness4b(m_data+offset+i+1);
                i += tempDataSize + 4;
                break;

            case OP_RETURN:
                scriptString += std::string(opCodesNames[(uint8_t)m_data[offset+i]]) + " ";
                if (ifcount == 0) {
                    if (i < (size - 1)) {
                        scriptString += "<data> "; //what follows is only data
                    }
                    i = size; //stop processing here
                }
                break;
            case OP_IF:
                scriptString += std::string(opCodesNames[(uint8_t)m_data[offset+i]]) + " ";
                ifcount += 1;
                break;
            case OP_ENDIF:
                scriptString += std::string(opCodesNames[(uint8_t)m_data[offset+i]]) + " ";
                ifcount -= 1;
                break;
            case OP_PUBKEYHASH:
            case OP_PUBKEY:
            case OP_INVALIDOPCODE:
                scriptString += std::string(opCodesNames[(uint8_t)m_data[offset+i]]) + " "; //those 3 are also invalid when used in script
                i = size; //stop processing here
                break;
            
            default:
                scriptString += std::string(opCodesNames[(uint8_t)m_data[offset+i]]) + " ";
                break;
        }
    }

    return scriptString;
}

std::string TX::extractString(uint64_t offset, uint64_t size) {
    std::string value = "";
    for(uint64_t i = 0; i < size; ++ i) {
        value += m_data[offset + i];
    }
    value += "";
    return value;
}

bool TX::isOrdinal(int offset, int size) {
    bool ordinal = false;
    if(size < 0x30) {
        return ordinal;
    }
    if(((opCodes)(m_data[(int64_t)offset])) == OP_DATA_32) {
        if(((opCodes)(m_data[(int64_t)offset+0x21])) == OP_CHECKSIG) {
            if(((opCodes)(m_data[(int64_t)offset+0x22])) == OP_0) {
                if(((opCodes)(m_data[(int64_t)offset+0x23])) == OP_IF) {
                    if(((opCodes)(m_data[(int64_t)offset+0x24])) == OP_DATA_3) {
                        if(((m_data[(int64_t)offset+0x25])) == 'o') {
                            if(((m_data[(int64_t)offset+0x26])) == 'r') {
                                if(((m_data[(int64_t)offset+0x27])) == 'd') {
                                    ordinal = true;
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    return ordinal;
}

void TX::saveOrdinal(std::string dataType, uint64_t offset, uint64_t size, char * ordinalsFolder) {

    uint64_t witOffset = 0;
    bool success = true;
    if(((opCodes)(m_data[(int64_t)offset+witOffset])) != OP_0) {
        std::cout << "failed saving ordinal." << std::endl;
        return;
    }

    witOffset++;

    unsigned char shaResult[SHA_DIGEST_LENGTH];
    SHA1((unsigned char * )(m_data+offset), size, shaResult);
    std::string filename = std::string(ordinalsFolder) + std::string("/") + rawToHex(shaResult, SHA_DIGEST_LENGTH) + "." + getFileTypeExtension(dataType); 

    FILE *fp;
    fp = fopen(filename.c_str(),"wb");
    if( fp == NULL ) {
        std::cout << "could not open file " << filename << std::endl;
    } else {
        while((((opCodes)(m_data[(int64_t)offset+witOffset])) != OP_ENDIF) && (witOffset < size) && success) {
            uint64_t tempSize = 0;
            if((((opCodes)(m_data[(int64_t)offset+witOffset])) >= OP_DATA_1) && (((opCodes)(m_data[(int64_t)offset+witOffset])) <= OP_DATA_75)) {
                tempSize = (uint8_t)m_data[(int64_t)offset+witOffset];
                witOffset++;
            } else if (((opCodes)(m_data[(int64_t)offset+witOffset])) == OP_PUSHDATA1) {
                witOffset++;
                tempSize = (uint8_t)m_data[(int64_t)offset+witOffset];
                witOffset++;
            } else if (((opCodes)(m_data[(int64_t)offset+witOffset])) == OP_PUSHDATA2) {
                witOffset++;
                tempSize = swapIntEndianness2b(m_data+offset+witOffset);
                witOffset += 2;
            } else if (((opCodes)(m_data[(int64_t)offset+witOffset])) == OP_PUSHDATA4) {
                witOffset++;
                tempSize = swapIntEndianness4b(m_data+offset+witOffset);
                witOffset += 4;
            } else {
                success = false;
                witOffset++;
            }

            if (success) {
                fwrite(m_data+offset+witOffset, 1, tempSize, fp);
                witOffset += tempSize;
            } else {
                std::cout << "write failed: " << filename << std::endl;
            }
        }

        fclose(fp);
    }
}

Block::Block() {
    m_data = nullptr;
    m_length = 0;
    m_txCount = 0;
    m_version = 0;
    m_time = 0;
}

Block::Block(char * blockData) {
    m_data = nullptr;
    m_length = swapIntEndianness4b(blockData+4);
    m_txCount = 0;
    m_version = 0;
    m_time = 0;

    initFromASCII(blockData);
}

Block::~Block() {
    //we don't delete stuff anymore as we are only keeping a reference to the 
    //place where the block start from the data that gets passed to the block 
    //class (but memory managed out of it)... this is for speed optimization.
}

void Block::processTx(TxScriptDb* scriptDB, TxOverTimeDb* scriptOverTimeDB, TxScriptDb* segWitDB, char * ordinalsFolder) {
    int byteLen = 0;
    getVarInt(m_data+88, &byteLen); //we re-read txcount totknow on how many bytes it is stored
    int offset = byteLen +88; //skipping header, tx data starts at offset 88 + lenght to store txCount
    TX* tmpTX = nullptr;
    int i = 0;

    for(i = 0; (i < m_txCount) && (offset < m_length); ++i) {
        tmpTX = new TX(m_data+offset, m_time);

        offset += tmpTX->getNextTxOffset();
        if (scriptDB != nullptr) {
            tmpTX->processScriptPubKeys(scriptDB, scriptOverTimeDB);
        }
        if (segWitDB != nullptr) {
            tmpTX->processSegregatedWitnesses(segWitDB, ordinalsFolder);
        }

        delete tmpTX;
    }
}

bool Block::verifyMerkleRoot()
{
    bool isValid = false;
    int byteLen = 0;
    getVarInt(m_data+88, &byteLen); //we re-read txcount totknow on how many bytes it is stored
    int offset = byteLen +88; //skipping header, tx data starts at offset 88 + lenght to store txCount
    TX* tmpTX = nullptr;
    int i = 0;
    int extendedArraySize = m_txCount+1; //we need one for easier calculations incase of odd txCount
    unsigned char * txids = new unsigned char [extendedArraySize*SHA256_DIGEST_LENGTH];

    for(i = 0; (i < m_txCount) && (offset < m_length); ++i) {
        tmpTX = new TX(m_data+offset, m_time);
        tmpTX->getTXID(txids+(i*SHA256_DIGEST_LENGTH));
        offset += tmpTX->getNextTxOffset();
        delete tmpTX;
    }

    int remainingTXIDs = m_txCount;
    memcpy(txids+((extendedArraySize - 1)*SHA256_DIGEST_LENGTH), txids+((extendedArraySize - 2)*SHA256_DIGEST_LENGTH), SHA256_DIGEST_LENGTH);

    while(remainingTXIDs > 1) {
        for (i = 0; i < remainingTXIDs; i += 2) {
            SHA256(txids+(i*SHA256_DIGEST_LENGTH), SHA256_DIGEST_LENGTH*2, txids+((i/2)*SHA256_DIGEST_LENGTH));
            SHA256(txids+((i/2)*SHA256_DIGEST_LENGTH), SHA256_DIGEST_LENGTH, txids+((i/2)*SHA256_DIGEST_LENGTH));
        }
        remainingTXIDs = (remainingTXIDs + 1) / 2;
        memcpy(txids+(remainingTXIDs*SHA256_DIGEST_LENGTH), txids+((remainingTXIDs - 1)*SHA256_DIGEST_LENGTH), SHA256_DIGEST_LENGTH);
    }

    if(0 == memcmp(m_data+44, txids, SHA256_DIGEST_LENGTH)) { //merkle root is at offset 44 in the block
        isValid = true;
    }

    delete[] txids;
    return isValid;
}

void Block::initFromASCII(char *asciiString) {

    if(nullptr != m_data) {
        delete m_data;
        m_data = nullptr;
    }
    m_data = asciiString;

    m_version = swapIntEndianness4b(m_data+8);
    m_time = swapIntEndianness4b(m_data+76);;

    int byteLen = 0;
    m_txCount = getVarInt(m_data+88, &byteLen);
}

int Block::getNextBlockOffset() {
    return m_length+8; //+8 to account for magic number and block length.
}

int Block::getTxCount() {
    return m_txCount;
}

unsigned int Block::getVersion() {
    return m_version;
}

unsigned int Block::getTime() {
    return m_time;
}

void Block::getBlockHash(unsigned char *hash) {
    SHA256((unsigned char *)m_data+8, 80, hash);
    SHA256(hash, SHA256_DIGEST_LENGTH, hash);
}

uint64_t getVarInt(const char *buffer, int *byteLen) {
    switch(buffer[0]) {
    case (char)0xfd:
        *byteLen = 3;
        return swapIntEndianness2b(buffer + 1);
        break;
    case (char)0xfe:
        *byteLen = 5;
        return swapIntEndianness4b(buffer + 1);
        break;
    case (char)0xff:
        *byteLen = 9;
        return swapIntEndianness8b(buffer + 1);
        break;
    default:
        *byteLen = 1;
        return (uint64_t)(((unsigned char)buffer[0]));
        break;
    }
    return 0;
}

unsigned int swapIntEndianness2b(const char *smallEndianInt) {
    return ((unsigned int)((unsigned char)smallEndianInt[1]) << 8) + ((unsigned int)((unsigned char)smallEndianInt[0]));
}

unsigned int swapIntEndianness3b(const char *smallEndianInt) {
    return ((unsigned int)((unsigned char)smallEndianInt[2]) << 16) + 
            ((unsigned int)((unsigned char)smallEndianInt[1]) << 8) + ((unsigned int)((unsigned char)smallEndianInt[0]));
}

unsigned int swapIntEndianness4b(const char *smallEndianInt) {
    return ((unsigned int)((unsigned char)smallEndianInt[3]) << 24 ) + ((unsigned int)((unsigned char)smallEndianInt[2]) << 16) + 
            ((unsigned int)((unsigned char)smallEndianInt[1]) << 8) + ((unsigned int)((unsigned char)smallEndianInt[0]));
}

uint64_t swapIntEndianness5b(const char *smallEndianInt) {
    return ((uint64_t)((unsigned char)smallEndianInt[4]) << 32) + 
            ((uint64_t)((unsigned char)smallEndianInt[3]) << 24 ) + ((uint64_t)((unsigned char)smallEndianInt[2]) << 16) + 
            ((uint64_t)((unsigned char)smallEndianInt[1]) << 8) + ((uint64_t)((unsigned char)smallEndianInt[0]));
}

uint64_t swapIntEndianness6b(const char *smallEndianInt) {
    return ((uint64_t)((unsigned char)smallEndianInt[5]) << 40) + ((uint64_t)((unsigned char)smallEndianInt[4]) << 32) + 
            ((uint64_t)((unsigned char)smallEndianInt[3]) << 24 ) + ((uint64_t)((unsigned char)smallEndianInt[2]) << 16) + 
            ((uint64_t)((unsigned char)smallEndianInt[1]) << 8) + ((uint64_t)((unsigned char)smallEndianInt[0]));
}

uint64_t swapIntEndianness7b(const char *smallEndianInt) {
    return ((uint64_t)((unsigned char)smallEndianInt[6]) << 48) + 
            ((uint64_t)((unsigned char)smallEndianInt[5]) << 40) + ((uint64_t)((unsigned char)smallEndianInt[4]) << 32) + 
            ((uint64_t)((unsigned char)smallEndianInt[3]) << 24 ) + ((uint64_t)((unsigned char)smallEndianInt[2]) << 16) + 
            ((uint64_t)((unsigned char)smallEndianInt[1]) << 8) + ((uint64_t)((unsigned char)smallEndianInt[0]));
}

uint64_t swapIntEndianness8b(const char *smallEndianInt) {
    return ((uint64_t)((unsigned char)smallEndianInt[7]) << 56 ) + ((uint64_t)((unsigned char)smallEndianInt[6]) << 48) + 
            ((uint64_t)((unsigned char)smallEndianInt[5]) << 40) + ((uint64_t)((unsigned char)smallEndianInt[4]) << 32) + 
            ((uint64_t)((unsigned char)smallEndianInt[3]) << 24 ) + ((uint64_t)((unsigned char)smallEndianInt[2]) << 16) + 
            ((uint64_t)((unsigned char)smallEndianInt[1]) << 8) + ((uint64_t)((unsigned char)smallEndianInt[0]));
}

std::string rawToHex(const unsigned char * data, uint64_t size) {
    std::string result = "";
    for(uint64_t i = 0; i < size; ++i) {
        result += decToHex((data[i] >> 4) & 0b00001111);
        result += decToHex(data[i] & 0b00001111);
    }
    return result;
}

char decToHex(int dec) {
    char value = 0;
    if((0 <= dec) && (9 >= dec)) {
        value = dec + '0';
    } else if((10 <= dec) && (15 >= dec)) {
        value = dec - 10 + 'a';
    }
    return value;
}

void hashEndianSwap(unsigned char *hash) {
    unsigned char temp[SHA256_DIGEST_LENGTH];
    memcpy(temp, hash, SHA256_DIGEST_LENGTH);
    for(int i = 0; i < SHA256_DIGEST_LENGTH; i++) {
        hash[i] = temp[SHA256_DIGEST_LENGTH-i-1];
    }
}
