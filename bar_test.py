import matplotlib.pyplot as plt
import os
import numpy as np
#if not  os.path.exists('graphs_bm_r5_p10_30x_SLO'):
#	os.makedirs('graphs_bm_r5_p10_30x_SLO')
from matplotlib.ticker import FuncFormatter

#load imbalance
figure, ((ax)) = plt.subplots(1, 1)

conn = [
1024
,2048
,4096
]

BRAM = [
4.814814815
,4.953703704
,4.953703704]

BRAM = [(18432*num*2160/100)/61704000 for num in BRAM]

LUT = [
12.34292089
,20.18150024
,32.77225036
]

LUT = [(64*num*331680/100)/61704000 for num in LUT]

FF = [
8.029124457
,9.794380125
,13.64884829]

FF = [(1*num*663360/100)/61704000 for num in FF]


'''
ax.plot(conn, BRAM, '-', color ='orchid', linewidth = 2.0, label="BRAM", marker='o')
ax.plot(conn, LUT, '-', color= 'purple', linewidth = 2.0, label="LUT", marker='o')
ax.plot(conn, FF, '-', color= 'navy', linewidth = 2.0, label="FF", marker='o')
'''

#conn = ['Category A', 'Category B', 'Category C']
#BRAM = np.array([10, 20, 15])
#LUT = np.array([5, 12, 8])
#FF = np.array([8, 7, 10])

conn = ['1024', '2048', '4096']

BRAM = [3.1, 3.2, 3.2]
LUT = [4.2, 6.9, 11.3]
FF = [0.1, 0.1, 0.1]

print(BRAM)
print(LUT)
print(FF)

#BRAM = [round(num, 3) for num in BRAM]
#LUT = [round(num, 3) for num in LUT]
#FF = [round(num, 3) for num in FF]

print(BRAM)
print(LUT)
print(FF)

result_array = [7.3, 10.1, 14.5]
#result_array = [num/100 for num in result_array]

ax.bar(conn, BRAM, label='BRAM', color ='orchid')
ax.bar(conn, LUT, bottom=BRAM, label='LUT', color= 'purple')
ax.bar(conn, FF, bottom= result_array, label='FF', color= 'navy')


ax.tick_params(axis='both', labelsize=12)
#ax.set_xticks(np.arange(0, 8, 1))

ax.legend(["BRAM", "LUT", "FF"], loc = "upper left", fontsize=12, framealpha=0.5)#,bbox_to_anchor=(1,0.5),fontsize=14,framealpha=0.5, ncol=4)

#ax.legend(["group_ex 10%", "group_ex 100%"], loc = "center left",bbox_to_anchor=(1,0.5),fontsize=14,framealpha=0.5)
#ax.legend(["grp_ex", "grp_sh", "Sassy"], loc = "upper left",fontsize=16,framealpha=0.5)

#ax.set_title('100%', fontsize=20)

ax.set_ylabel('Resource\nUtilization (%)',fontsize=14)
ax.set_xlabel('Connection Count',fontsize=14)
#ax.set_ylim(0, 40)
#ax.set_xlim(0, 3)
#ax.set_xscale('log', basex=2)
#ax.set_yscale('log', basey=2)
figure.set_size_inches(3.5, 2.0)

plt.savefig('bar_test.pdf',bbox_inches='tight')