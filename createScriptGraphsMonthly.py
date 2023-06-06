#!/usr/bin/python3
import matplotlib.pyplot as plt
import matplotlib.ticker as tck
import csv
import sys

if __name__ == "__main__":
    filename = sys.argv[1]

    with open(filename) as csv_file:
        csv_reader = csv.reader(csv_file, delimiter=';')
        line_count = 0
        col_index = 0
        date = []
        xAxis = []
        script = [[]]
        scriptNames = []
        scriptsInstanceCount = []
        instanceCount = 0
        
        for row in csv_reader:
            if line_count == 0:
                for item in row:
                    if col_index != 0:
                        date.append(item)
                    col_index += 1
            else:
                script.append([]);
                for item in row:
                    if col_index == 0:
                        scriptNames.append(item)
                    else:
                        script[line_count - 1].append(int(item))
                        instanceCount += int(item)
                    col_index += 1
                scriptsInstanceCount.append(instanceCount)
            line_count += 1
            col_index = 0
            instanceCount = 0

        print(f'Processed {line_count} lines.')

        p2pk64 = []
        p2pk32 = []
        p2pkh = []
        opreturn = []
        opdata20 = []
        multisig = [[]]
    
        scriptNum = 0
        for scriptCount in scriptsInstanceCount:
            if scriptNames[scriptNum] == "OP_1 OP_DATA_33 <data> OP_DATA_65 <data> OP_2 OP_CHECKMULTISIG ":
                multisig[0] = script[scriptNum]
            if scriptNames[scriptNum] == "OP_1 OP_DATA_65 <data> OP_DATA_65 <data> OP_DATA_33 <data> OP_3 OP_CHECKMULTISIG ":
                multisig.append(script[scriptNum])
            if scriptNames[scriptNum] == "OP_3 OP_DATA_33 <data> OP_DATA_33 <data> OP_DATA_33 <data> OP_3 OP_CHECKMULTISIG ":
                multisig.append(script[scriptNum])
            if scriptNames[scriptNum] == "OP_2 OP_DATA_65 <data> OP_DATA_65 <data> OP_2 OP_CHECKMULTISIG ":
                multisig.append(script[scriptNum])
            if scriptNames[scriptNum] == "OP_2 OP_DATA_65 <data> OP_DATA_65 <data> OP_DATA_65 <data> OP_3 OP_CHECKMULTISIG ":
                multisig.append(script[scriptNum])
            if scriptNames[scriptNum] == "OP_1 OP_DATA_65 <data> OP_DATA_65 <data> OP_2 OP_CHECKMULTISIG ":
                multisig.append(script[scriptNum])
            if scriptNames[scriptNum] == "OP_1 OP_DATA_65 <data> OP_1 OP_CHECKMULTISIG ":
                multisig.append(script[scriptNum])
            if scriptNames[scriptNum] == "OP_1 OP_DATA_33 <data> OP_1 OP_CHECKMULTISIG ":
                multisig.append(script[scriptNum])
            if scriptNames[scriptNum] == "OP_2 OP_DATA_33 <data> OP_DATA_33 <data> OP_DATA_33 <data> OP_3 OP_CHECKMULTISIG ":
                multisig.append(script[scriptNum])
            if scriptNames[scriptNum] == "OP_2 OP_DATA_33 <data> OP_DATA_33 <data> OP_2 OP_CHECKMULTISIG ":
                multisig.append(script[scriptNum])
            if scriptNames[scriptNum] == "OP_1 OP_DATA_65 <data> OP_DATA_33 <data> OP_DATA_33 <data> OP_3 OP_CHECKMULTISIG ":
                multisig.append(script[scriptNum])
            if scriptNames[scriptNum] == "OP_1 OP_DATA_65 <data> OP_DATA_65 <data> OP_DATA_65 <data> OP_3 OP_CHECKMULTISIG ":
                multisig.append(script[scriptNum])
            if scriptNames[scriptNum] == "OP_1 OP_DATA_65 <data> OP_DATA_33 <data> OP_2 OP_CHECKMULTISIG ":
                multisig.append(script[scriptNum])
            if scriptNames[scriptNum] == "OP_1 OP_DATA_33 <data> OP_DATA_33 <data> OP_DATA_65 <data> OP_3 OP_CHECKMULTISIG ":
                multisig.append(script[scriptNum])
            if scriptNames[scriptNum] == "OP_1 OP_DATA_33 <data> OP_DATA_33 <data> OP_2 OP_CHECKMULTISIG ":
                multisig.append(script[scriptNum])
            if scriptNames[scriptNum] == "OP_1 OP_DATA_33 <data> OP_DATA_33 <data> OP_DATA_33 <data> OP_3 OP_CHECKMULTISIG ":
                multisig.append(script[scriptNum])

            if scriptNames[scriptNum] == "OP_DUP OP_HASH160 OP_DATA_20 <data> OP_EQUALVERIFY OP_CHECKSIG ":
                p2pkh = script[scriptNum]
                fig, ax = plt.subplots()
                ax.plot(date, script[scriptNum])
                ax.set_ylabel('Number of occurrences per month')
                ax.set_title("P2PKH locking script per Month")
                #ax.ticklabel_format(useOffset=False)
                plt.ticklabel_format(axis='y',style='plain')
                plt.xticks([0, 12, 24, 36, 48, 60, 72, 84, 96, 108, 120, 132, 144, 156, 168, 180], ['2009', '2010', '2011','2012', '2013', '2014', '2015', '2016', '2017', '2018', '2019', '2020', '2021', '2022', '2023', '2024'], rotation=45)
                plt.margins(x=0)
                plt.subplots_adjust(left=0.16)
                fig.savefig("scriptP2PKH.png", dpi=300)
                #plt.show()
            if scriptNames[scriptNum] == "OP_HASH160 OP_DATA_20 <data> OP_EQUAL ":
                fig, ax = plt.subplots()
                ax.plot(date, script[scriptNum])
                ax.set_ylabel('Number of occurrences per month')
                ax.set_title("P2SH locking script per Month")
                plt.xticks([0, 12, 24, 36, 48, 60, 72, 84, 96, 108, 120, 132, 144, 156, 168, 180], ['2009', '2010', '2011','2012', '2013', '2014', '2015', '2016', '2017', '2018', '2019', '2020', '2021', '2022', '2023', '2024'], rotation=45)
                plt.margins(x=0)
                plt.ticklabel_format(axis='y',style='plain')
                plt.subplots_adjust(left=0.16)
                fig.savefig("scriptP2SH.png", dpi=300)
            if scriptNames[scriptNum] == "OP_DATA_33 <data> OP_CHECKSIG ":
                p2pk32 = script[scriptNum]
                fig, ax = plt.subplots()
                ax.plot(date, script[scriptNum])
                ax.set_ylabel('Number of occurrences per month')
                ax.set_title("P2PK (32 bytes pubkey)")
                plt.xticks([0, 12, 24, 36, 48, 60, 72, 84, 96, 108, 120, 132, 144, 156, 168, 180], ['2009', '2010', '2011','2012', '2013', '2014', '2015', '2016', '2017', '2018', '2019', '2020', '2021', '2022', '2023', '2024'], rotation=45)
                plt.margins(x=0)
                fig.savefig("scriptP2PK32.png", dpi=300)
            if scriptNames[scriptNum] == "OP_DATA_65 <data> OP_CHECKSIG ":
                p2pk64 = script[scriptNum]
                fig, ax = plt.subplots()
                ax.plot(date, script[scriptNum])
                ax.set_ylabel('occurrences')
                ax.set_title("P2PK (64 bytes pubkey)")
                plt.xticks([0, 12, 24, 36, 48, 60, 72, 84, 96, 108, 120, 132, 144, 156, 168, 180], ['2009', '2010', '2011','2012', '2013', '2014', '2015', '2016', '2017', '2018', '2019', '2020', '2021', '2022', '2023', '2024'], rotation=45)
                plt.margins(x=0)
                fig.savefig("scriptP2PK64.png", dpi=300)
            if scriptNames[scriptNum] == " ":
                fig, ax = plt.subplots()
                ax.plot(date, script[scriptNum])
                ax.set_ylabel('occurrences')
                ax.set_title("Empty Script")
                plt.xticks([0, 12, 24, 36, 48, 60, 72, 84, 96, 108, 120, 132, 144, 156, 168, 180], ['2009', '2010', '2011','2012', '2013', '2014', '2015', '2016', '2017', '2018', '2019', '2020', '2021', '2022', '2023', '2024'], rotation=45)
                plt.margins(x=0)
                fig.savefig("scriptEmpty.png", dpi=300)
                #plt.show()
            if scriptNames[scriptNum] == "OP_RETURN ":
                fig, ax = plt.subplots()
                ax.plot(date, script[scriptNum])
                ax.set_ylabel('occurrences')
                ax.set_title("OP_RETURN")
                plt.xticks([0, 12, 24, 36, 48, 60, 72, 84, 96, 108, 120, 132, 144, 156, 168, 180], ['2009', '2010', '2011','2012', '2013', '2014', '2015', '2016', '2017', '2018', '2019', '2020', '2021', '2022', '2023', '2024'], rotation=45)
                plt.margins(x=0)
                fig.savefig("scriptOP_RETURN.png", dpi=300)
            if scriptNames[scriptNum] == "OP_1 OP_DATA_32 <data> ":
                fig, ax = plt.subplots()
                ax.plot(date, script[scriptNum])
                ax.set_ylabel('occurrences')
                ax.set_title("32 bytes data storage script")
                plt.xticks([0, 12, 24, 36, 48, 60, 72, 84, 96, 108, 120, 132, 144, 156, 168, 180], ['2009', '2010', '2011','2012', '2013', '2014', '2015', '2016', '2017', '2018', '2019', '2020', '2021', '2022', '2023', '2024'], rotation=45)
                plt.margins(x=0)
                fig.savefig("script1and32.png", dpi=300)
            if scriptNames[scriptNum] == "OP_0 OP_DATA_32 <data> ":
                fig, ax = plt.subplots()
                ax.plot(date, script[scriptNum])
                ax.set_ylabel('occurrences')
                ax.set_title("32 bytes data storage script")
                plt.xticks([0, 12, 24, 36, 48, 60, 72, 84, 96, 108, 120, 132, 144, 156, 168, 180], ['2009', '2010', '2011','2012', '2013', '2014', '2015', '2016', '2017', '2018', '2019', '2020', '2021', '2022', '2023', '2024'], rotation=45)
                plt.margins(x=0)
                fig.savefig("script0and32.png", dpi=300)
            if scriptNames[scriptNum] == "OP_RETURN <data> ":
                fig, ax = plt.subplots()
                ax.plot(date, script[scriptNum])
                ax.set_ylabel('occurrences')
                ax.set_title("Data Storage Script")
                plt.xticks([0, 12, 24, 36, 48, 60, 72, 84, 96, 108, 120, 132, 144, 156, 168, 180], ['2009', '2010', '2011','2012', '2013', '2014', '2015', '2016', '2017', '2018', '2019', '2020', '2021', '2022', '2023', '2024'], rotation=45)
                plt.margins(x=0)
                fig.savefig("scriptOP_RETURN_DATA.png", dpi=300)
                #plt.show()
            if scriptNames[scriptNum] == "OP_0 OP_DATA_20 <data> ":
                fig, ax = plt.subplots()
                ax.plot(date, script[scriptNum])
                ax.set_ylabel('occurrences')
                ax.set_title("20 bytes data storage script")
                plt.xticks([0, 12, 24, 36, 48, 60, 72, 84, 96, 108, 120, 132, 144, 156, 168, 180], ['2009', '2010', '2011','2012', '2013', '2014', '2015', '2016', '2017', '2018', '2019', '2020', '2021', '2022', '2023', '2024'], rotation=45)
                plt.margins(x=0)
                fig.savefig("script0and20.png", dpi=300)
            if scriptCount > 800000:
                fig, ax = plt.subplots()
                ax.plot(date, script[scriptNum])
                ax.set_ylabel('occurrences')
                ax.set_title(scriptNames[scriptNum])
                plt.xticks([0, 12, 24, 36, 48, 60, 72, 84, 96, 108, 120, 132, 144, 156, 168, 180], ['2009', '2010', '2011','2012', '2013', '2014', '2015', '2016', '2017', '2018', '2019', '2020', '2021', '2022', '2023', '2024'], rotation=45)
                plt.margins(x=0)
                fig.savefig("script" + str(scriptNum) + ".png", dpi=300)
                #plt.show()
            scriptNum += 1
        
        fig, ax = plt.subplots()
        ax.stackplot(date, p2pk32, p2pk64, p2pkh, labels = ['P2PK 32 bytes address','P2PK 64 bytes address', 'P2PKH'])
        ax.set_ylabel('occurrences')
        #ax.set_title(scriptNames[scriptNum])
        ax.legend(loc='upper left')
        ax.set_title('P2PK and P2PKH cumulative')
        ax.set_xlabel('Year')
        plt.xticks([0, 12, 24, 36, 48, 60, 72, 84, 96, 108, 120, 132, 144, 156, 168, 180], ['2009', '2010', '2011','2012', '2013', '2014', '2015', '2016', '2017', '2018', '2019', '2020', '2021', '2022', '2023', '2024'], rotation=45)
        #plt.margins(x=0)
        fig.savefig("script_P2PK_P2PKH.png", dpi=300)
        #plt.show()

        fig, ax = plt.subplots()
        ax.stackplot(date, p2pk32, p2pk64, labels = ['P2PK 32 bytes address','P2PK 64 bytes address'])
        ax.set_ylabel('Number of occurrences per month')
        #ax.set_title(scriptNames[scriptNum])
        ax.legend(loc='upper left')
        ax.set_title('P2PK Stacked per Month')
        ax.set_xlabel('')
        plt.xticks([0, 12, 24, 36, 48, 60, 72, 84, 96, 108, 120, 132, 144, 156, 168, 180], ['2009', '2010', '2011','2012', '2013', '2014', '2015', '2016', '2017', '2018', '2019', '2020', '2021', '2022', '2023', '2024'], rotation=45)
        plt.margins(x=0)
        fig.savefig("script_P2PK_stacked.png", dpi=300)

        fig, ax = plt.subplots()
        ax.stackplot(date, multisig[0], multisig[1], multisig[2], multisig[3], multisig[4], multisig[5], multisig[6], multisig[7], multisig[8], multisig[9], multisig[10], multisig[11], multisig[12], multisig[13], multisig[14], multisig[15])
        ax.set_ylabel('Number of occurrences per month')
        #ax.set_title(scriptNames[scriptNum])
        #ax.legend(loc='upper left')
        ax.set_title('P2MS Stacked per Month')
        ax.set_xlabel('')
        plt.xticks([0, 12, 24, 36, 48, 60, 72, 84, 96, 108, 120, 132, 144, 156, 168, 180], ['2009', '2010', '2011','2012', '2013', '2014', '2015', '2016', '2017', '2018', '2019', '2020', '2021', '2022', '2023', '2024'], rotation=45)
        plt.margins(x=0)
        fig.savefig("script_P2MS_stacked.png", dpi=300)
        
