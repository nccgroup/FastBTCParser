// Released as open source by NCC Group Plc - https://www.nccgroup.com/
// https://github.com/nccgroup/FastBTCParser

#include "tx_type_over_time_db.h"
#include <iterator>
#include <iostream>
#include <fstream>
#include <iomanip>
#include <time.h>

//number of days in years 2009 to 2023 accounting for leap years.
#define TIME_ARRAY_SIZE (365*12+366*3) 

void indexToDate(int daySince2009, int& year, int& month, int& day) {
    year = 0;
    month = 0;
    day = 0;
    int tempDay = daySince2009;

    //find year
    do {
        if((tempDay >=0) && (tempDay <=364)) { //2009 2013 2017 2021
            year = 2009 + year * 4;
        } else if((tempDay >= 365) && (tempDay <= 729)) { //2010 2014 2018 2022
            tempDay -= 365;
            year = 2010 + year * 4;
        } else if((tempDay >= 730) && (tempDay <= 1094)) { //2011 2015 2019 2023
            tempDay -= 730;
            year = 2011 + year * 4;
        } else if((tempDay >= 1095) && (tempDay <= 1460)) { //leap year 2012 2016 2020
            tempDay -= 1095;
            year = 2012 + year * 4;
        } else {
            tempDay -= 1461;
            year++;
        }
    } while (year <= 2008);


    if((tempDay >=0) && (tempDay <=30)) { //jan
        month = 1;
        day = tempDay + 1;
        tempDay += 1000;
    } else {
        tempDay -= 31;
    }
    
    if((tempDay >= 0) && (tempDay <= 28) && (year % 4 == 0)) { //feb leap year
        month = 2;
        day = tempDay + 1;
        tempDay += 1000;
    } else if((tempDay >=0) && (tempDay <= 27) && (year % 4 != 0)) { //feb
        month = 2;
        day = tempDay + 1;
        tempDay += 1000;
    } else {
        if (year % 4 != 0) {
            tempDay -= 28;
        } else {
            tempDay -= 29;
        }
    }

    if((tempDay >=0) && (tempDay <=30)) { //mar
        month = 3;
        day = tempDay + 1;
        tempDay += 1000;
    } else {
        tempDay -= 31;
    }

    if((tempDay >=0) && (tempDay <=29)) { //apr
        month = 4;
        day = tempDay + 1;
        tempDay += 1000;
    } else {
        tempDay -= 30;
    }

    if((tempDay >=0) && (tempDay <=30)) { //may
        month = 5;
        day = tempDay + 1;
        tempDay += 1000;
    } else {
        tempDay -= 31;
    }

    if((tempDay >=0) && (tempDay <=29)) { //jun
        month = 6;
        day = tempDay + 1;
        tempDay += 1000;
    } else {
        tempDay -= 30;
    }

    if((tempDay >=0) && (tempDay <=30)) { //jul
        month = 7;
        day = tempDay + 1;
        tempDay += 1000;
    } else {
        tempDay -= 31;
    }

    if((tempDay >=0) && (tempDay <=30)) { //aug
        month = 8;
        day = tempDay + 1;
        tempDay += 1000;
    } else {
        tempDay -= 31;
    }

    if((tempDay >=0) && (tempDay <=29)) { //sep
        month = 9;
        day = tempDay + 1;
        tempDay += 1000;
    } else {
        tempDay -= 30;
    }

    if((tempDay >=0) && (tempDay <=30)) { //oct
        month = 10;
        day = tempDay + 1;
        tempDay += 1000;
    } else {
        tempDay -= 31;
    }

    if((tempDay >=0) && (tempDay <=29)) { //nov
        month = 11;
        day = tempDay + 1;
        tempDay += 1000;
    } else {
        tempDay -= 30;
    }

    if((tempDay >=0) && (tempDay <=30)) { //dec
        month = 12;
        day = tempDay + 1;
        tempDay += 1000;
    } else {
        tempDay -= 31;
    }  
}

TxOverTimeDb::TxOverTimeDb() {
}

TxOverTimeDb::~TxOverTimeDb() {
    std::map<std::string, uint64_t*>::iterator itr;
    for (itr = m_map.begin(); itr != m_map.end(); ++itr) {
        delete itr->second;
    }
}

void TxOverTimeDb::insertOrUpdate(std::string script, const time_t atTime) {
    tm * scriptTM = gmtime(&atTime);
    int year = scriptTM->tm_year+1900-2009;
    int yearIndex = year * 365; 
    yearIndex += (year / 4); //(number of leap years since 2009 (not counting current year)) 
    int index = yearIndex+scriptTM->tm_yday;

    if((index < 0) || (index >= TIME_ARRAY_SIZE)) {
        index = TIME_ARRAY_SIZE - 1;
    }

    if(m_map.count(script) > 0) {
        m_map[script][index] += 1;
    } else {
        m_map[script] = new uint64_t[TIME_ARRAY_SIZE];
        for (int i = 0; i < TIME_ARRAY_SIZE; ++i) {
            m_map[script][i] = 0;
        }
        m_map[script][index] = 1;
    }
}

void TxOverTimeDb::merge(TxOverTimeDb *foreignTxOverTimeDB) {
    std::map<std::string, uint64_t*>::iterator itr;
    for (itr = foreignTxOverTimeDB->m_map.begin(); itr != foreignTxOverTimeDB->m_map.end(); ++itr) {
        if(m_map.count(itr->first) > 0) {
            for (int i = 0; i < TIME_ARRAY_SIZE; ++i) {
                m_map[itr->first][i] += itr->second[i];
            }
        } else {
            m_map[itr->first] = new uint64_t[TIME_ARRAY_SIZE];
            for (int i = 0; i < TIME_ARRAY_SIZE; ++i) {
                m_map[itr->first][i] = itr->second[i];
            }
        }
    }
}

void TxOverTimeDb::saveContents(std::string filename) {
    std::ofstream outdata;
    outdata.open(filename, std::fstream::out | std::fstream::trunc);
    if( !outdata ) { // file couldn't be opened
        std::cerr << "Error: file could not be opened" << std::endl;
        exit(1);
    }

    std::map<std::string, uint64_t*>::iterator itr;

    outdata << "date";
    int year, month, day;
    for (int i = 0; i < TIME_ARRAY_SIZE; ++i) {
        indexToDate(i, year, month, day);
        outdata <<    ";" << month << "/" << day << "/" << year;
    }
    outdata << std::endl;

    for (itr = m_map.begin(); itr != m_map.end(); ++itr) {
        outdata << itr->first;
        for (int i = 0; i < TIME_ARRAY_SIZE; ++i) {
            outdata <<    ";" << itr->second[i];
        }
        outdata << std::endl;
    }
    outdata.close();

    std::cout << "Results written to " << filename << std::endl; 
}

void TxOverTimeDb::saveContentsInverted(std::string filename) {
    std::ofstream outdata;
    int year, month, day;

    outdata.open(filename, std::fstream::out | std::fstream::trunc);
    if( !outdata ) { // file couldn't be opened
        std::cerr << "Error: file could not be opened" << std::endl;
        exit(1);
    }

    std::map<std::string, uint64_t*>::iterator itr;
    outdata << "ITEM = OCCURENCES PER DAY" << std::endl;
    
    for (itr = m_map.begin(); itr != m_map.end(); ++itr) {
        outdata << itr->first << ";";
    }
    outdata << std::endl;


    for (int i = 0; i < TIME_ARRAY_SIZE; ++i) {
        indexToDate(i, year, month, day);
        outdata << month << "/" << day << "/" << year << ";";
        for (itr = m_map.begin(); itr != m_map.end(); ++itr) {
            outdata << itr->second[i]<< ";";
        }
        outdata << std::endl;
    }
    std::cout << "Results written to " << filename << std::endl;
    outdata.close();
}

void TxOverTimeDb::saveContentsMonthly(std::string filename) {
    std::ofstream outdata;
    outdata.open(filename, std::fstream::out | std::fstream::trunc);
    if( !outdata ) { // file couldn't be opened
        std::cerr << "Error: file could not be opened" << std::endl;
        exit(1);
    }

    std::map<std::string, uint64_t*>::iterator itr;

    outdata << "date";
    int year, month, day;
    int prevMonth = 0;
    for (int i = 0; i < TIME_ARRAY_SIZE; ++i) {
        indexToDate(i, year, month, day);
        if(prevMonth != month) {
            prevMonth = month;
            switch(month) {
                case 1:
                    outdata << ";Jan-" << year;
                    break;
                case 2:
                    outdata << ";Feb-" << year;
                    break;
                case 3:
                    outdata << ";Mar-" << year;
                    break;
                case 4:
                    outdata << ";Apr-" << year;
                    break;
                case 5:
                    outdata << ";May-" << year;
                    break;
                case 6:
                    outdata << ";Jun-" << year;
                    break;
                case 7:
                    outdata << ";Jul-" << year;
                    break;
                case 8:
                    outdata << ";Aug-" << year;
                    break;
                case 9:
                    outdata << ";Sep-" << year;
                    break;
                case 10:
                    outdata << ";Oct-" << year;
                    break;
                case 11:
                    outdata << ";Nov-" << year;
                    break;
                case 12:
                    outdata << ";Dec-" << year;
                    break;
                default:
                    outdata << ";failed!!!-" << year;
                    break;
            }
        }
    }
    outdata << std::endl;

    prevMonth = 1;
    uint64_t monthlyCumulative;

    for (itr = m_map.begin(); itr != m_map.end(); ++itr) {
        outdata << itr->first;
        prevMonth = 1;
        monthlyCumulative = 0;
        for (int i = 0; i < TIME_ARRAY_SIZE; ++i) {
            indexToDate(i, year, month, day);
            if(prevMonth != month) {
                prevMonth = month;
                outdata << ";" << monthlyCumulative;
                monthlyCumulative = itr->second[i];
            } else {
                monthlyCumulative += itr->second[i];
            }
        }
        outdata << ";" << monthlyCumulative << std::endl;
    }
    outdata.close();

    std::cout << "Results written to " << filename << std::endl; 
}

void TxOverTimeDb::printStats() {
    uint64_t totalOutputs = 0;
    std::map<std::string, uint64_t*>::iterator itr;
    for (itr = m_map.begin(); itr != m_map.end(); ++itr) {
        for (int i = 0; i < TIME_ARRAY_SIZE; ++i) {
            totalOutputs += itr->second[i];
        }
    }
    std::cout << "found " << m_map.size() << " types of scripts in " << totalOutputs << " tx outputs." << std::endl;
}
