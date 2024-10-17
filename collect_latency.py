import numpy as np
from statistics import mean
import argparse
import csv
import os, glob
import sys

file_list = glob.glob("*.result") 
file_list = sorted(file_list,key = os.path.getmtime)
#results = [[]]
'''
p90_list  = []
p95_list  = []
p99_list  = []
p999_list = []
'''
for filename in file_list:
    data = np.loadtxt(filename)
    #data_mean = mean(data)
    #maximum = max(data)
    #p90 = np.percentile(data, 90)
    #p95 = np.percentile(data, 95)
    p99 = np.percentile(data, 99)
    #p999 = np.percentile(data, 99.9)

    print(filename)
    print(p99*4.6*16)
    #result = [p90, p95, p99, p999]
    #print(result)
    #results.append(result)

'''
with open('result/result.csv', 'w') as f:
    # create the csv writer
    writer = csv.writer(f)

    # write a row to the csv file
    writer.writerow(["90%", "95%", "99%", "99.9%"])
    writer.writerows(results)
'''