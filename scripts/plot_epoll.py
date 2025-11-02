import matplotlib.pyplot as plt
import os
import numpy as np
#if not  os.path.exists('graphs_bm_r5_p10_30x_SLO'):
#	os.makedirs('graphs_bm_r5_p10_30x_SLO')
from matplotlib.ticker import FuncFormatter

#load imbalance
figure, ((ax)) = plt.subplots(1, 1)

#10% CAP

epoll_lowload_p99 = [
7212.8
,6992
,12732.8
,29513.6
,60867.2
,126297.6
,276883.2]

poll_lowload_p99 = [
7433.59
,7948.8
,10377.6
,18547.2
,51004.8
,123795.2
,255980.8        
]

connections = [
16
,32
,64
,128
,256
,512
,1024
]


#tput_scq_10p = [num / 1E6 for num in tput_scq_10p]
poll_lowload_p99 = [num / 1E3 for num in poll_lowload_p99]
epoll_lowload_p99 = [num / 1E3 for num in epoll_lowload_p99]

ax.plot(connections, poll_lowload_p99, '--', color ='gray', linewidth = 2.0, label="poll", marker='o')
ax.plot(connections, epoll_lowload_p99, '-', color ='navy', linewidth = 2.0, label="epoll", marker='o')

ax.tick_params(axis='both', labelsize=12)
#ax.set_xticks(np.arange(0, 8, 1))

ax.legend(["poll", "epoll"], loc = "lower right", fontsize=12)#,bbox_to_anchor=(1,0.5),fontsize=14,framealpha=0.5, ncol=4)


#ax.set_title('100%', fontsize=20)

ax.set_ylabel('p99 latency (us)',fontsize=14)
ax.set_xlabel('Connections',fontsize=14)
#ax.set_xlabel('Load (MRPS)',fontsize=14)
#ax.set_ylim(0, 50)
#ax.set_xlim(0, 7)

#ax.set_xscale('log', basex=2)
#ax.set_yscale('log', basey=2)
figure.set_size_inches(3.5, 2.0)

plt.savefig('epoll_10%_low_load.pdf',bbox_inches='tight')

plt.clf()

figure, ((ax)) = plt.subplots(1, 1)

#100% CAP

epoll_lowload_p99 = [
5667.2
,6256
,13910.4
,30912
,65724.8
,133878.4
,317657.6
]

poll_lowload_p99 = [
7728
,8096
,11040
,22668.8
,61971.2
,132038.4
,279900.8
]

connections = [
16
,32
,64
,128
,256
,512
,1024
]


#tput_scq_10p = [num / 1E6 for num in tput_scq_10p]
poll_lowload_p99 = [num / 1E3 for num in poll_lowload_p99]
epoll_lowload_p99 = [num / 1E3 for num in epoll_lowload_p99]

ax.plot(connections, poll_lowload_p99, '--', color ='gray', linewidth = 2.0, label="poll", marker='o')
ax.plot(connections, epoll_lowload_p99, '-', color ='navy', linewidth = 2.0, label="epoll", marker='o')

ax.tick_params(axis='both', labelsize=12)
#ax.set_xticks(np.arange(0, 8, 1))

ax.legend(["poll", "epoll"], loc = "lower right", fontsize=12)#,bbox_to_anchor=(1,0.5),fontsize=14,framealpha=0.5, ncol=4)


#ax.set_title('100%', fontsize=20)

ax.set_ylabel('p99 latency (us)',fontsize=14)
ax.set_xlabel('Connections',fontsize=14)
#ax.set_xlabel('Load (MRPS)',fontsize=14)
#ax.set_ylim(0, 50)
#ax.set_xlim(0, 7)

#ax.set_xscale('log', basex=2)
#ax.set_yscale('log', basey=2)
figure.set_size_inches(3.5, 2.0)

plt.savefig('epoll_100%_low_load.pdf',bbox_inches='tight')

plt.clf()


figure, ((ax)) = plt.subplots(1, 1)
#10% CAP load latency

epoll_load = [
299378
,596559
,892342
,1186381
,1477535
,1765835
]

poll_load = [
299010
,596295
,891493
,1184006
,1475227
,1761598
,2043889
,2326121
]

epoll_p99 = [
292412.8
,312211.2
,326342.4
,333702.4
,334438.4
,357254.4
]

poll_p99 = [
253552
,290499.2
,302422.4
,311254.4
,318982.4
,328108.8
,338339.2
,351955.2
]   


#tput_scq_10p = [num / 1E6 for num in tput_scq_10p]
poll_load = [num / 1E6 for num in poll_load]
epoll_load = [num / 1E6 for num in epoll_load]
epoll_p99 = [num / 1E3 for num in epoll_p99]
poll_p99 = [num / 1E3 for num in poll_p99]

ax.plot(poll_load, poll_p99, '--', color ='gray', linewidth = 2.0, label="poll", marker='o')
ax.plot(epoll_load, epoll_p99, '-', color ='navy', linewidth = 2.0, label="epoll", marker='o')

ax.tick_params(axis='both', labelsize=12)
#ax.set_xticks(np.arange(0, 8, 1))

ax.legend(["poll", "epoll"], loc = "lower right", fontsize=12)#,bbox_to_anchor=(1,0.5),fontsize=14,framealpha=0.5, ncol=4)


#ax.set_title('100%', fontsize=20)

ax.set_ylabel('p99 latency (us)',fontsize=14)
ax.set_xlabel('Load (MRPS)',fontsize=14)
#ax.set_ylim(0, 50)
#ax.set_xlim(0, 7)

#ax.set_xscale('log', basex=2)
#ax.set_yscale('log', basey=2)
figure.set_size_inches(3.5, 2.0)

plt.savefig('epoll_10%_load_latency.pdf',bbox_inches='tight')

plt.clf()

figure, ((ax)) = plt.subplots(1, 1)

#100% CAP load latency

epoll_load = [
299095
,596627
,892291
,1184618
,1476929
,1763222
]

poll_load = [
299431
,596608
,890604
,1182181
,1469118
,1750686
]

epoll_p99 = [
316259.2
,384707.2
,456099.2
,532201.6
,604771.2
,663356.8
]

poll_p99 = [
284464
,359904
,427689.6
,504086.4
,573049.6
,634284.8
]


#tput_scq_10p = [num / 1E6 for num in tput_scq_10p]
poll_load = [num / 1E6 for num in poll_load]
epoll_load = [num / 1E6 for num in epoll_load]
epoll_p99 = [num / 1E3 for num in epoll_p99]
poll_p99 = [num / 1E3 for num in poll_p99]

ax.plot(poll_load, poll_p99, '--', color ='gray', linewidth = 2.0, label="poll", marker='o')
ax.plot(epoll_load, epoll_p99, '-', color ='navy', linewidth = 2.0, label="epoll", marker='o')

ax.tick_params(axis='both', labelsize=12)
#ax.set_xticks(np.arange(0, 8, 1))

ax.legend(["poll", "epoll"], loc = "lower right", fontsize=12)#,bbox_to_anchor=(1,0.5),fontsize=14,framealpha=0.5, ncol=4)


#ax.set_title('100%', fontsize=20)

ax.set_ylabel('p99 latency (us)',fontsize=14)
ax.set_xlabel('Load (MRPS)',fontsize=14)
#ax.set_ylim(0, 50)
#ax.set_xlim(0, 7)

#ax.set_xscale('log', basex=2)
#ax.set_yscale('log', basey=2)
figure.set_size_inches(3.5, 2.0)

plt.savefig('epoll_100%_load_latency.pdf',bbox_inches='tight')

plt.clf()

