// Released as open source by NCC Group Plc - https://www.nccgroup.com/
// https://github.com/nccgroup/FastBTCParser

#ifndef FILE_TYPE_DB_H
#define FILE_TYPE_DB_H

#include <cstdint>
#include <iostream>
#include <map>

#define FILE_TYPE_SIZE 35

void initFileTypeDB();
std::string getFileTypeExtension(std::string dataType);

#endif // FILE_TYPE_DB_H