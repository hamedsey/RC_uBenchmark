import matplotlib.pyplot as plt
import os
import numpy as np
#if not  os.path.exists('graphs_bm_r5_p10_30x_SLO'):
#	os.makedirs('graphs_bm_r5_p10_30x_SLO')
from matplotlib.ticker import FuncFormatter

#load imbalance
figure, ((ax)) = plt.subplots(1, 1)


conn = [
64
,128
,256
,512
,1024]

tput_scq = [
980780
,1865596
,2441264
,3073697
,3703692
]
tput_scq_100p = [num / 1E6 for num in tput_scq]

tput_sassy = [
1718756
,2551014
,4608656
,5459101
,6188873]
tput_scq_10p = [num / 1E6 for num in tput_sassy]


ax.plot(conn, tput_scq, '-', color ='orchid', linewidth = 2.0, label="grp_ex", marker='o')
ax.plot(conn, tput_sassy, '-', color= 'navy', linewidth = 2.0, label="JSC", marker='o')

ax.tick_params(axis='both', labelsize=12)
#ax.set_xticks(np.arange(0, 8, 1))

ax.legend(["grp_ex", "JSC"], loc = "lower right", fontsize=12)#,bbox_to_anchor=(1,0.5),fontsize=14,framealpha=0.5, ncol=4)

#ax.legend(["group_ex 10%", "group_ex 100%"], loc = "center left",bbox_to_anchor=(1,0.5),fontsize=14,framealpha=0.5)
#ax.legend(["grp_ex", "grp_sh", "Sassy"], loc = "upper left",fontsize=16,framealpha=0.5)

#ax.set_title('100%', fontsize=20)

ax.set_ylabel('Load (RPS)',fontsize=14)
ax.set_xlabel('Connection Count',fontsize=14)
#ax.set_ylim(0, 80)
#ax.set_xlim(0, 3)
#ax.set_xscale('log', basex=2)
#ax.set_yscale('log', basey=2)
figure.set_size_inches(3.5, 2.0)

plt.savefig('ubench_conn_scaling.pdf',bbox_inches='tight')

plt.clf()

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

LUT = [
12.34292089
,20.18150024
,32.77225036
]

FF = [
8.029124457
,9.794380125
,13.64884829]


ax.plot(conn, BRAM, '-', color ='orchid', linewidth = 2.0, label="BRAM", marker='o')
ax.plot(conn, LUT, '-', color= 'purple', linewidth = 2.0, label="LUT", marker='o')
ax.plot(conn, FF, '-', color= 'navy', linewidth = 2.0, label="FF", marker='o')

ax.tick_params(axis='both', labelsize=12)
#ax.set_xticks(np.arange(0, 8, 1))

ax.legend(["BRAM", "LUT", "FF"], loc = "upper left", fontsize=12, framealpha=0.5)#,bbox_to_anchor=(1,0.5),fontsize=14,framealpha=0.5, ncol=4)

#ax.legend(["group_ex 10%", "group_ex 100%"], loc = "center left",bbox_to_anchor=(1,0.5),fontsize=14,framealpha=0.5)
#ax.legend(["grp_ex", "grp_sh", "Sassy"], loc = "upper left",fontsize=16,framealpha=0.5)

#ax.set_title('100%', fontsize=20)

ax.set_ylabel('Resource\nUtilization (%)',fontsize=14)
ax.set_xlabel('Connection Count',fontsize=14)
ax.set_ylim(0, 40)
#ax.set_xlim(0, 3)
#ax.set_xscale('log', basex=2)
#ax.set_yscale('log', basey=2)
figure.set_size_inches(3.5, 2.0)

plt.savefig('ubench_resource_scaling.pdf',bbox_inches='tight')


plt.clf()

figure, ((ax)) = plt.subplots(1, 1)

x = [
1
,2
,4
,8
,16
,32
,64
,128
]

util_1k_conn = [
0.013302217
,0.014984442
,0.020005186
,0.028380656
,0.043459095
,0.071930507
,0.127161934
,0.235861532
]

util_12_cores = [
0.036762609
,0.073291845
,0.14633087
,0.292389472
,0.584487229
,1.168663296
,2.336995981
,4.673641903
]

ax.plot(x, util_1k_conn, '-', color ='orchid', linewidth = 2.0, label="1k conn, X cores", marker='o')
#ax.plot(conn, LUT, '-', color= 'purple', linewidth = 2.0, label="LUT", marker='o')
ax.plot(x, util_12_cores, '-', color= 'navy', linewidth = 2.0, label="12 cores, X*1k conn", marker='o')

ax.plot(128, 29.95, color= 'black', linewidth = 2.0, label="128 cores, 128k conn", marker='x', linestyle='None')

ax.tick_params(axis='both', labelsize=12)
#ax.set_xticks(np.arange(0, 8, 1))

ax.legend(["1k conn, X cores", "12 cores, X*1k conn","128 cores, 128k conn"], loc = "upper left", fontsize=11, framealpha=0.5)#,bbox_to_anchor=(1,0.5),fontsize=14,framealpha=0.5, ncol=4)

#ax.legend(["group_ex 10%", "group_ex 100%"], loc = "center left",bbox_to_anchor=(1,0.5),fontsize=14,framealpha=0.5)
#ax.legend(["grp_ex", "grp_sh", "Sassy"], loc = "upper left",fontsize=16,framealpha=0.5)

#ax.set_title('FPGA Resource Utilization%', fontsize=20)

ax.set_ylabel('Resource\nUtilization (%)',fontsize=14)
ax.set_xlabel('Cores/1k Connections',fontsize=14)
ax.set_ylim(0, 33)
#ax.set_xlim(0, 3)
#ax.set_xscale('log', basex=2)
#ax.set_yscale('log', basey=2)
figure.set_size_inches(3.5, 2.0)

plt.savefig('FPGA_resource_utilization_percent.pdf',bbox_inches='tight')