def count_value_changes(lst, x):
    if lst.size == 0:
        return 0

    change_count = 0
    previous_value = lst[0]
    i = 1
    for current_value in lst[1:]:
        if current_value != previous_value:
            change_count += 1
            print(str(i) + "  " + str(x[i]))
            previous_value = current_value
        i = i + 1

    return change_count


import numpy as np
import matplotlib.pyplot as plt


from statistics import mean
#import argparse
#import csv
import os, glob
import sys

figure, ((ax)) = plt.subplots(2, 1)

file_list_time0 = glob.glob("*B.time") 
file_list_time0 = sorted(file_list_time0,key = os.path.getmtime)

file_list_time1 = glob.glob("*1.time") 
file_list_time1 = sorted(file_list_time1,key = os.path.getmtime)

file_list_core0 = glob.glob("*B.core") 
file_list_core0 = sorted(file_list_core0,key = os.path.getmtime)

file_list_core1 = glob.glob("*1.core") 
file_list_core1 = sorted(file_list_core1,key = os.path.getmtime)

'''
p90_list  = []
p95_list  = []
p99_list  = []
p999_list = []
'''

file_list_latencies = glob.glob("*.result") 
file_list_latencies = sorted(file_list_latencies, key = os.path.getmtime)
latencies = np.loadtxt(file_list_latencies[0])
print("min latency = "+str(4.6*16*min(latencies)))
print("p25 latency = "+str(4.6*16*np.percentile(latencies,25)))
print("p50 latency = "+str(4.6*16*np.percentile(latencies,50)))
print("p75 latency = "+str(4.6*16*np.percentile(latencies,75)))
print("p95 latency = "+str(4.6*16*np.percentile(latencies,95)))
print("mean latency= "+str(4.6*16*mean(latencies)))

x0 = np.loadtxt(file_list_time0[0])
x1 = np.loadtxt(file_list_time1[0])

y0 = np.loadtxt(file_list_core0[0])
y1 = np.loadtxt(file_list_core1[0])

changes = count_value_changes(y0, x0)
print(changes)
print(" . . . . .")
changes = count_value_changes(y1, x1)
print(changes)

ax[0].plot(x0, y0, '-' , marker='', label='0', color='blue')
ax[1].plot(x1, y1, '-' , marker='', label='1', color='red')


#ax[0].set_xlim([0, 0.2e8])#1e2])
#ax[1].set_xlim([0, 0.2e8])#1e2])
ax[0].set_xlim([0, 1e10])#1e2])
ax[1].set_xlim([0, 1e10])#1e2])

ax[0].set_ylim([-0.01, 1.01])
ax[1].set_ylim([-0.01, 1.01])#

#data = np.loadtxt(filename)
#data_mean = mean(data)
#maximum = max(data)
#p90 = np.percentile(data, 90)
#p95 = np.percentile(data, 95)
#p99 = np.percentile(data, 99)
#p999 = np.percentile(data, 99.9)

#print(filename)
#print(p99*4.6*16)
#result = [p90, p95, p99, p999]
#print(result)
#results.append(result)

#plt.plot(x0, y0, label='0', color='blue')  # Plot with a label
#plt.plot(x1, y1, label='1', color='red')  # Plot with a label

plt.show()

plt.savefig('conn2core_map2.png', bbox_inches='tight', dpi=300)

'''
with open('result/result.csv', 'w') as f:
    # create the csv writer
    writer = csv.writer(f)

    # write a row to the csv file
    writer.writerow(["90%", "95%", "99%", "99.9%"])
    writer.writerows(results)
'''
