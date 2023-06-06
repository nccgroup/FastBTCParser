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
        
        scriptNum = 0
        for scriptCount in scriptsInstanceCount:
            if scriptCount > 200000:
                fig, ax = plt.subplots()
                ax.plot(date, script[scriptNum])
                ax.set_ylabel('occurrences')
                ax.set_title(scriptNames[scriptNum])
                plt.xticks([0, 365, 730, 1095, 1461, 1826, 2191, 2556, 2922, 3287, 3652, 4017, 4383, 4748, 5113, 5478], ['2009', '2010', '2011','2012', '2013', '2014', '2015', '2016', '2017', '2018', '2019', '2020', '2021', '2022', '2023', '2024'], rotation=45)
                plt.margins(x=0)
                fig.savefig("script" + str(scriptNum) + ".png", dpi=300)
                #plt.show()
            scriptNum += 1

        
