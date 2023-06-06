#ifndef FILE_TYPE_DB_H
#define FILE_TYPE_DB_H

#include <cstdint>
#include <iostream>
#include <map>

#define FILE_TYPE_SIZE 35

void initFileTypeDB();
std::string getFileTypeExtension(std::string dataType);

#endif // FILE_TYPE_DB_H