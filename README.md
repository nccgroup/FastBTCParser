# Disclaimer

USE AT YOUR OWN RISK!! The author can not be held liable for any of the 
contents of the bitcoin blockchain. Files uncovered using this tool could be 
harmful, caintain viruses, contain malicious code, and could be illegal in the 
user country.  

This tool is a Bitcoin blockchain parser, the author does NOT endorse the use 
of crypto-currencies, NFTs or any sort of blockchain technology. 

The tool is provided as a research curiosity.

# FastBTCParser

The intent of the tool is to extract somewhat interesting data and provide 
some graphical data as a final result.

The tool can currently:
- Check Merkle roots of all blocks (but does not check the chain continuity, 
all blocks Merkle roots are verified independently and thus this can not be 
used to confirm the entire "validity" of the blockchain it parses).
- Fingerprint locking scripts (`ScriptPubKey`) and extract daily occurrences 
stats about these in CSV format.
- Somewhat process `SegWit` with the intent of extracting most (but not all) 
ordinals files.

## Prerequisites

You will need a full copy of the entire blockchain as provided by Bitcoin 
Core (<https://bitcoin.org/en/bitcoin-core/>), or at least a few consecutive 
files from that. Blockchain files are expected to have filenames of the format

```
blkXXXXX.dat
```

Where XXXXX is a 5 digits number.

## Compiling the tool

Compilation of the program requires `ncurses-devel`, and `libssl-devel` 
(sometimes called `openssl-devel`) packages.

From the cloned repository folder:

```
mkdir build
cd build
cmake ../
make
```

## Tool Usage

from the `build` folder:

```
./fastbtcparser --help
```
should show:

```
Usage: ./fastbtcparser --folder=FOLDER [OPTIONS] ... 
Parse the bitcoin blockchain files in FOLDER to extract some stats about it.
The program accepts the following list of arguments
  -f, --folder=FOLDER        Parses blockchain files found in FOLDER.
  -s, --startblock=NUM       Start parsing from block file number NUM.
  -e, --endblock=NUM         Stops Parsing after block file number NUM.
  -l, --lockingscripts=FILE  Fingerprint and count locking scripts, 
                             then write results to FILENAME.
  -m, --merkleroots          Verify all transactions in blocks add up
                             to the correct Merkle root of that block.
  -o, --ordinals=FOLDER      Extract all ordinals and output all files to FOLDER.
  -t, --threads=NUM          Multithread processing to NUM threads. (recommend
                             setting to number of logical processors for maximum
                             perfomance, defaults to 1).
  -h, --help                 Print this message and exit.
```

More interesting set of parameters:

```
./fastbtcparser --folder=BLOCKSFOLDER --startblock=0 --endblock=3750 --lockingscripts=scripts --threads=16
```

This will create a set of CSV files containing stats about the locking scripts

Using one of the python scripts afterwards with the CSV files will generate 
some graphics:

```
../createScriptGraphsMonthly.py scriptsMonthly.csv
```

```
./fastbtcparser --folder=BLOCKSFOLDER --startblock=0 --endblock=3750 --merkleroots --threads=16
```

The above will verify all the merkle roots of the blocks contained in files 0 
to 3750 (depending on the machine power and number of threads passed as 
parameter this could take a while).

```
mkdir ordinalsfiles
./fastbtcparser --folder=BLOCKSFOLDER --startblock=0 --endblock=3750 --ordinals=ordinalsfiles --threads=16
```

This will extract all ordinals files to the specified folder. Only try this if 
you have read and understood the risks involved with extracting files from the 
blockchain described higer on this page. DO NOT ATTEMPT UNLESS WILLING TO BREAK 
YOUR MACHINE. USE AT YOUR OWN RISK!!

