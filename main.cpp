// Released as open source by NCC Group Plc - https://www.nccgroup.com/
// https://github.com/nccgroup/FastBTCParser

#include "block.h"
#include "tx_type_db.h"
#include "tx_type_over_time_db.h"
#include "file_type_db.h"

#include <iostream>
#include <cstring>
#include <cstdint>
#include <stdio.h>
#include <unistd.h>
#include <iomanip>
#include <time.h>
#include <pthread.h>
#include <getopt.h>
#include <ncurses.h>
#include <mutex>
#include <signal.h>

std::mutex g_mutex;

struct threadParam {
    TxScriptDb * m_scriptDB;
    TxOverTimeDb * m_scriptOverTimeDB;
    TxScriptDb * m_segWitDB;
    int m_threadNumber;
    pthread_t m_threadId;
    int m_maxFileNumber;
    int m_minFileNumber;
    int m_maxThread;
    char* m_filenameRoot;
    char* m_ordinalsRoot;
    bool* m_merkleRoots;
    WINDOW * m_win;
};

 threadParam * params;
 WINDOW** windows;

void initWin(WINDOW* win, int fileNum, int threadNum) {
    wmove(win, 0, 0);
    wprintw(win, "Thread %3d\n[Proccessing file %05d  ]\n[  0%%                    ]\n", threadNum, fileNum);
    wrefresh(win);
}

void resizeHandler(int /*sig*/) {
    int nw;
    int maxThread = params[0].m_maxThread;
    g_mutex.lock();

    endwin();
    refresh();
    wclear(curscr);
    wrefresh(curscr);
    wclear(stdscr);
    wrefresh(stdscr);
    nw = getmaxx(stdscr);
    int numWinPerLines = nw/27;

    for(int j = 0; j < maxThread; ++j) {
        mvwin(windows[j], 5 + ((j/numWinPerLines)*4), (j-((j/numWinPerLines)*numWinPerLines))*27 );
        wrefresh(windows[j]);
    }
    wresize(windows[maxThread], 5, nw-1);
    wmove(windows[maxThread], 0, 0);
    wrefresh(windows[maxThread]);
    
    g_mutex.unlock();
}

void updateWin(threadParam* tParam, int fileNum) {
    unsigned int processingPercent = (((fileNum - tParam->m_minFileNumber)/tParam->m_maxThread)*100) / ((tParam->m_maxFileNumber + 1 - tParam->m_minFileNumber)/tParam->m_maxThread);
    if(g_mutex.try_lock()) { //we don't care if this is not displayed we don't want to slow processing for that.
        wmove(tParam->m_win, 0, 0);
        wprintw(tParam->m_win, "Thread %3d\n[Proccessing file %05d  ]\n", tParam->m_threadNumber, fileNum);
        switch(processingPercent / 5) {
            case 0:
                wprintw(tParam->m_win, "[%3d%%>                   ]", processingPercent);
                break;
            case 1:
                wprintw(tParam->m_win, "[%3d%%=>                  ]", processingPercent);
                break;
            case 2:
                wprintw(tParam->m_win, "[%3d%%==>                 ]", processingPercent);
                break;
            case 3:
                wprintw(tParam->m_win, "[%3d%%===>                ]", processingPercent);
                break;
            case 4:
                wprintw(tParam->m_win, "[%3d%%====>               ]", processingPercent);
                break;
            case 5:
                wprintw(tParam->m_win, "[%3d%%=====>              ]", processingPercent);
                break;
            case 6:
                wprintw(tParam->m_win, "[%3d%%======>             ]", processingPercent);
                break;
            case 7:
                wprintw(tParam->m_win, "[%3d%%=======>            ]", processingPercent);
                break;
            case 8:
                wprintw(tParam->m_win, "[%3d%%========>           ]", processingPercent);
                break;
            case 9:
                wprintw(tParam->m_win, "[%3d%%=========>          ]", processingPercent);
                break;
            case 10:
                wprintw(tParam->m_win, "[%3d%%==========>         ]", processingPercent);
                break;
            case 11:
                wprintw(tParam->m_win, "[%3d%%===========>        ]", processingPercent);
                break;
            case 12:
                wprintw(tParam->m_win, "[%3d%%============>       ]", processingPercent);
                break;
            case 13:
                wprintw(tParam->m_win, "[%3d%%=============>      ]", processingPercent);
                break;
            case 14:
                wprintw(tParam->m_win, "[%3d%%==============>     ]", processingPercent);
                break;
            case 15:
                wprintw(tParam->m_win, "[%3d%%===============>    ]", processingPercent);
                break;
            case 16:
                wprintw(tParam->m_win, "[%3d%%================>   ]", processingPercent);
                break;
            case 17:
                wprintw(tParam->m_win, "[%3d%%=================>  ]", processingPercent);
                break;
            case 18:
                wprintw(tParam->m_win, "[%3d%%==================> ]", processingPercent);
                break;
            case 19:
                wprintw(tParam->m_win, "[%3d%%===================>]", processingPercent);
                break;
            case 20:
                wprintw(tParam->m_win, "[%3d%%====================]", processingPercent);
                break;
            default:
                wprintw(tParam->m_win, "[%3d%%>                   ]", processingPercent);
                break;
        }
        wrefresh(tParam->m_win);
        g_mutex.unlock();
    }
}

void updateFinishedWin(WINDOW * win, int threadNum) {
    g_mutex.lock();
    wmove(win, 0, 0);
    wprintw(win, "Thread %3d\n[ FINISHED !             ]\n[100%%====================]", threadNum);
    wrefresh(win);
    delwin(win);
    g_mutex.unlock();
}

void * threadFunc(void *arg)
{
    struct threadParam *tParam = (threadParam *) arg;

    int i = 0;
    char * buffer;
    char * filename;
    size_t len_read = 0;
    FILE *fp;
    bool fileexist = true;
    Block* tmpBlock;
    size_t offset = 0;
    int block_count = 0;

    size_t neededlen = strlen(tParam->m_filenameRoot) + 15;
    filename = new char[neededlen];

    if (tParam->m_merkleRoots != nullptr) {
        tParam->m_merkleRoots[0] = true;
    }

    i += tParam->m_minFileNumber + tParam->m_threadNumber; 

    do {
        sprintf(filename, "%s/blk%05d.dat", tParam->m_filenameRoot, i);
        fp = fopen(filename,"r");
        if( fp == NULL ) {
            std::cout << "could not open file " << filename << std::endl;
            fileexist = false;
        } else {
            updateWin(tParam, i);
            fseek(fp , 0 , SEEK_END);
            int length = ftell(fp);
            rewind(fp);
            buffer = new char[length+1];
            len_read = fread(buffer, 1, length, fp);
            fclose(fp);
            offset = 0;

            while(offset+88 <= len_read) {
                tmpBlock = new Block(buffer+offset);
                if((tParam->m_scriptDB != nullptr) || (tParam->m_segWitDB != nullptr)) {
                    tmpBlock->processTx(tParam->m_scriptDB, tParam->m_scriptOverTimeDB, tParam->m_segWitDB, tParam->m_ordinalsRoot);
                }
                if (tParam->m_merkleRoots != nullptr) {
                    if (false == tmpBlock->verifyMerkleRoot()) {
                        tParam->m_merkleRoots[0] = false;
                        time_t blockTime = tmpBlock->getTime();
                        tm * blockTM = gmtime(&blockTime);
                        std::cout << "in file " << filename << " block " << block_count << " shows merkle root miscalculation "
                            << "at date : " << blockTM->tm_mon + 1 << "/" << blockTM->tm_mday << "/" 
                            << blockTM->tm_year + 1900 << " " << blockTM->tm_hour << ":" 
                            << blockTM->tm_min << ":" << blockTM->tm_sec 
                            << " (time as given by the block, which could be entirely wrong)"<< std::endl;
                        std::cout << "merkle root was supposed to be : " << rawToHex((unsigned char *)buffer+offset+44, 32) << std::endl;
                    }
                } 
                offset += tmpBlock->getNextBlockOffset();
                ++block_count;
                delete tmpBlock;
            }
            delete[] buffer;
            i += tParam->m_maxThread;
        }
        
    }while(fileexist && (i <= tParam->m_maxFileNumber));
    updateFinishedWin(tParam->m_win, tParam->m_threadNumber);

    return nullptr;
}

void help(char * argv) {
    fprintf(stderr, "Usage: %s --folder=FOLDER [OPTIONS] ... \n", argv);
    fprintf(stderr, "Parse the bitcoin blockchain files in FOLDER to extract some stats about it.\n");
    fprintf(stderr, "The program accepts the following list of arguments\n");
    fprintf(stderr, "  -f, --folder=FOLDER        Parses blockchain files found in FOLDER.\n");
    fprintf(stderr, "  -s, --startblock=NUM       Start parsing from block file number NUM.\n");
    fprintf(stderr, "  -e, --endblock=NUM         Stops Parsing after block file number NUM.\n");
    fprintf(stderr, "  -l, --lockingscripts=FILE  Fingerprint and count locking scripts, \n");
    fprintf(stderr, "                             then write results to FILENAME.\n");
    fprintf(stderr, "  -m, --merkleroots          Verify all transactions in blocks add up\n");
    fprintf(stderr, "                             to the correct Merkle root of that block.\n");
    fprintf(stderr, "  -o, --ordinals=FOLDER      Extract all ordinals and output all files to FOLDER.\n");
    fprintf(stderr, "  -t, --threads=NUM          Multithread processing to NUM threads. (recommend\n");
    fprintf(stderr, "                             setting to number of logical processors for maximum\n");
    fprintf(stderr, "                             perfomance, defaults to 1).\n");
    fprintf(stderr, "  -h, --help                 Print this message and exit.\n");
}

int main(int argc, char **argv)
{
    std::cout << "starting: " << std::endl;
    int i = 0;
    int maxThread = 8;
    int maxFileNumber = 99999; //(the block file format is silly like that... 
                               //it has 5 digits in the block number. If we 
                               //reach that limit the block chain is over 12 TB 
                               //which is probably a bad sign for the planet)

    char * blockFolder = nullptr;
    char * ordinalsFolder = nullptr;
    char * lockingScriptsFile = nullptr;

    int c;
    bool ordinals = false;
    bool lockingScripts = false;
    bool merkleRoots = false;

    if (argc < 2) {
        help(argv[0]);
        exit(EXIT_FAILURE);
    }

    while (1) {
        int option_index = 0;
        static struct option long_options[] = {
            {"folder",         required_argument, 0, 'f' },
            {"startblock",     required_argument, 0, 's' },
            {"endblock",       required_argument, 0, 'e' },
            {"lockingscripts", required_argument, 0, 'l' },
            {"merkleroots",    no_argument,       0, 'm' },
            {"ordinals",       required_argument, 0, 'o' },
            {"threads",        required_argument, 0, 't' },
            {"help",           no_argument,       0, 'h' },
            {0,                0,                 0, 0   }
        };

        c = getopt_long(argc, argv, "f:s:e:l:mo:t:h", long_options, &option_index);
        if (c == -1){
            break;
        }

        size_t folderNameSize = 0;

        switch (c) {

        case 'f':
            folderNameSize = strlen(optarg); 
            if ((folderNameSize > 0) && (folderNameSize < 2048)) { //arbitrary max folder name length of 2048 characters;
                blockFolder = new char[folderNameSize+1];
                strncpy(blockFolder, optarg, folderNameSize+1); //this is fine, we've checked the size just before the if statement
            } else {
                fprintf(stderr, "folder was too long or missing\n");
                exit(EXIT_FAILURE);
            }
            break;

        case 's':
            i = std::stoi(std::string(optarg));
            break;

        case 'e':
            maxFileNumber = std::stoi(std::string(optarg));
            break;

        case 'l':
            lockingScripts = true;
            folderNameSize = strlen(optarg); 
            if ((folderNameSize > 0) && (folderNameSize < 2048)) { //arbitrary max folder name length of 2048 characters;
                lockingScriptsFile = new char[folderNameSize+1];
                strncpy(lockingScriptsFile, optarg, folderNameSize+1); //this is fine, we've checked the size just before the if statement
            } else {
                fprintf(stderr, "file name was too long or missing\n");
                exit(EXIT_FAILURE);
            }
            break;
            break;

        case 'm':
            merkleRoots = true;
            break;

        case 'o':
            ordinals = true;
            folderNameSize = strlen(optarg); 
            if ((folderNameSize > 0) && (folderNameSize < 2048)) { //arbitrary max folder name length of 2048 characters;
                ordinalsFolder = new char[folderNameSize+1];
                strncpy(ordinalsFolder, optarg, folderNameSize+1); //this is fine, we've checked the size just before the if statement
            } else {
                fprintf(stderr, "folder was too long or missing\n");
                exit(EXIT_FAILURE);
            }
            break;

        case 't':
            maxThread = std::stoi(std::string(optarg));
            break;
        
        case 'h':
            help(argv[0]);
            exit(EXIT_SUCCESS);
            break;

        default:
            printf("?? getopt returned character code 0%o ??\n", c);
            help(argv[0]);
            exit(EXIT_FAILURE);
        }
    }

    if (optind < argc) {
        printf("non-option ARGV-elements: ");
        while (optind < argc)
            printf("%s ", argv[optind++]);
        printf("\n");
        help(argv[0]);
        exit(EXIT_FAILURE);
    }

    initFileTypeDB();
    
    TxScriptDb** scriptDB;
    TxOverTimeDb** scriptOverTimeDB;
    scriptDB = new TxScriptDb*[maxThread];
    scriptOverTimeDB = new TxOverTimeDb*[maxThread];
    for(int j = 0; j < maxThread; ++j) {
        scriptDB[j] = new TxScriptDb;
        scriptOverTimeDB[j] = new TxOverTimeDb;
    }

    TxScriptDb** segWitDB;    
    segWitDB = new TxScriptDb*[maxThread];
    for(int j = 0; j < maxThread; ++j) {
        segWitDB[j] = new TxScriptDb;
    }

    bool * merkleRootResults;
    if (merkleRoots == true) {
        merkleRootResults = new bool[maxThread];
    }

    initscr(); //ncurses init
    noecho(); //don't show typed characters
    int w;
    w = getmaxx(stdscr);
    wrefresh(stdscr); //draw the screen
    windows = new WINDOW*[maxThread+1];
    int numWinPerLines = w/27;

    for(int j = 0; j < maxThread; ++j) {
        windows[j] = newwin(5, 27, 5 + ((j/numWinPerLines)*4), (j-((j/numWinPerLines)*numWinPerLines))*27 );
        scrollok(windows[j], FALSE);
    }
    windows[maxThread] = newwin(5, w-1, 0, 0);

    curs_set(0); //sets cursor invisible
    wmove(windows[maxThread], 0, 0);
    wprintw(windows[maxThread], "Launching BTC blockchain processing with %d Thread(s)\n", maxThread);
    wprintw(windows[maxThread], "Merkle Root %s. Locking script stats %s. Ordinals %s.\n", merkleRoots? "Enabled":"Disabled", lockingScripts? "Enabled":"Disabled", ordinals? "Enabled":"Disabled");
    wprintw(windows[maxThread], "Start file #%05d. End file #%05d.\n", i, maxFileNumber);
    wprintw(windows[maxThread], "Now processing...\n");
    wrefresh(windows[maxThread]);

    for(int j = 0; j < maxThread; ++j) {
        initWin(windows[j], 0, j);
    }

    int retValue;
    params = new threadParam[maxThread];
    
    for(int j = 0; j < maxThread; ++j)
    {
        if (lockingScripts == true) {
            params[j].m_scriptDB = scriptDB[j];
            params[j].m_scriptOverTimeDB = scriptOverTimeDB[j];
        } else {
            params[j].m_scriptDB = nullptr;
            params[j].m_scriptOverTimeDB = nullptr;
        }
        if (ordinals == true) {
            params[j].m_segWitDB = segWitDB[j];
        } else {
            params[j].m_segWitDB = nullptr;
        }        
        params[j].m_threadNumber = j;
        params[j].m_maxFileNumber = maxFileNumber;
        params[j].m_minFileNumber = i;
        params[j].m_maxThread = maxThread;
        params[j].m_filenameRoot = blockFolder;
        params[j].m_ordinalsRoot = ordinalsFolder;
        if (merkleRoots == true) {
            params[j].m_merkleRoots = &(merkleRootResults[j]);
        } else {
            params[j].m_merkleRoots = nullptr;
        }
        params[j].m_win = windows[j];
        

        retValue = pthread_create(&params[j].m_threadId, NULL, &threadFunc, &params[j]);
        if (retValue != 0)
        {
            std::cout << "pthread_create error" << std::endl;
            return retValue;
        }
    }

    signal(SIGWINCH, resizeHandler);

    for(int j = 0; j < maxThread; ++j)
    {
        retValue = pthread_join(params[j].m_threadId, nullptr);
        if (retValue != 0)
        {
            std::cout << "pthread_join error" << std::endl;
            return retValue;
        }
    }


    endwin(); //ncurses we've joined all threads, we can now stop using ncurses.

    //we've joined all threads, let's collate all DBs
    std::cout << "collating/merging results..." << std::endl;

    for(int j = 1; j < maxThread; ++j)
    {
        if (lockingScripts == true) {
            scriptDB[0]->merge(scriptDB[j]);
            scriptOverTimeDB[0]->merge(scriptOverTimeDB[j]);
            delete scriptDB[j];
            delete scriptOverTimeDB[j];
        }

        if (ordinals == true) {
            segWitDB[0]->merge(segWitDB[j]);
            delete segWitDB[j];
        }

        if (merkleRoots == true) {
            merkleRootResults[0] &= merkleRootResults[j];
        }
    }

    if (lockingScripts == true) {
        std::cout << "Locking scripts:" << std::endl;
        scriptDB[0]->saveCompleteStatsOrdered(std::string(lockingScriptsFile)+std::string("Stats.csv"));
        scriptOverTimeDB[0]->saveContents(std::string(lockingScriptsFile)+std::string(".csv"));
        scriptOverTimeDB[0]->saveContentsInverted(std::string(lockingScriptsFile)+std::string("Inverted.csv"));
        scriptOverTimeDB[0]->saveContentsMonthly(std::string(lockingScriptsFile)+std::string("Monthly.csv"));
        scriptOverTimeDB[0]->printStats();
        delete scriptDB[0];
        delete scriptOverTimeDB[0];
        delete[] scriptDB;
        delete[] scriptOverTimeDB;
    }

    if (ordinals == true) {
        std::cout << "Ordinals file types:" << std::endl;
        segWitDB[0]->printContents();
        delete segWitDB[0];
        delete segWitDB;
    }

    if (merkleRoots == true) {
        if (merkleRootResults[0] == true) {
            std::cout << "Merkle roots result: Success (All checked blocks' Merkle roots were consistent with transactions from that block)" << std::endl;
        } else {
            std::cout << "Merkle roots result: Failed (one or more transaction is inconsistent with its block's Merkle root, check above)" << std::endl;
        }
        delete merkleRootResults;
    }

    delete params;
    exit(EXIT_SUCCESS);
    return 0;
}
