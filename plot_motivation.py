import matplotlib.pyplot as plt
import os
import numpy as np
#if not  os.path.exists('graphs_bm_r5_p10_30x_SLO'):
#	os.makedirs('graphs_bm_r5_p10_30x_SLO')

#load imbalance
figure, ((ax)) = plt.subplots(1, 1)

tput_scq_10p = [299128
,596837
,892234
,1186258
,1477861
,1766845
,2051020
,2336819
,2614209
,2887392
,3159353
,3425766
,3697400
,4199762]

lat_scq_10p  = [5667.2
,5740.8
,5961.6
,6182.4
,6550.4
,6844.8
,7286.4
,8022.4
,8316.8
,9052.8
,9641.6
,11040
,18841.6
,1573273.6
]




tput_scq_100p = [299282, 596885, 891914, 1186046, 1476512, 1766311, 2050650, 2335575, 2611812, 2887147, 3156093, 3431832, 3692664, 3938439, 4202176, 4454398, 4700459, 4931150, 5168875, 5410371, 5635136, 5848692, 6039088, 6235058, 6409619, 6570329, 6769175, 7015330, 7219501, 7246550]
lat_scq_100p  = [7139.2, 7212.8, 7360, 7507.2, 7728, 7948.8, 8169.6, 8464, 8758.4,8979.2,9420.8,9641.6,10156.8,10524.8,10966.4,11555.2,12217.6,12880,13689.6,14867.2,16118.4,17590.4
,19209.6
,21564.8
,24582.4
,29219.2
,38051.2
,81916.8
,902704
,8441478.4]



tput_sscq_10p = [299136
,597046
,892624
,1186299
,1477754
,1765450
,2049722
,2336127
,2614192
,2890102
,3160400
,3434703
,3693252
,3946673
,4203911
,4433421]

lat_sscq_10p = [6329.6
,6403.2
,6624
,6771.2
,6918.4
,7065.6
,7360
,7728
,8684.8
,11113.6
,14352
,17075.2
,18547.2
,20387.2
,27011.2
,1622364.8]


tput_scq_10p = [num / 1E6 for num in tput_scq_10p]
tput_scq_100p = [num / 1E6 for num in tput_scq_100p]
lat_scq_10p = [num / 1E3 for num in lat_scq_10p]
lat_scq_100p = [num / 1E3 for num in lat_scq_100p]

tput_sscq_10p = [num / 1E6 for num in tput_sscq_10p]
lat_sscq_10p = [num / 1E3 for num in lat_sscq_10p]

ax.plot(tput_scq_10p, lat_scq_10p, '--', color ='red', linewidth = 2.0, label="grp_ex 10%", marker='o')
ax.plot(tput_scq_100p, lat_scq_100p, '--', color= 'blue', linewidth = 2.0, label="grp_ex 100%", marker='o')
ax.plot(tput_sscq_10p, lat_sscq_10p, '--', color= 'gray', linewidth = 2.0, label="grp_sh 10%", marker='o')

ax.tick_params(axis='both', labelsize=16)
ax.set_xticks(np.arange(0, 8, 1))

#ax.legend(["group_ex 10%", "group_ex 100%"], loc = "center left",bbox_to_anchor=(1,0.5),fontsize=14,framealpha=0.5)
ax.legend(["grp_ex 10%", "grp_ex 100%","grp_sh 10%"], loc = "upper left",fontsize=16,framealpha=0.5)

ax.set_ylabel('p99 latency (us)',fontsize=20)
ax.set_xlabel('Load (MRPS)',fontsize=20)
ax.set_ylim(0, 50)
#ax.set_xscale('log', basex=2)
#ax.set_yscale('log', basey=2)


#figure.tight_layout(pad=1.0)
figure.set_size_inches(4.5, 2.5)

plt.savefig('motivation_LB.pdf',bbox_inches='tight')

plt.clf()
#plt.show()


#Synchronization Overhead

figure, ((ax)) = plt.subplots(1, 1)


tput_sscq_100p = [299186
,596873
,891838
,1186066
,1478412
,1766509
,2049689
,2335729
,2612368
,2885527
,3151053
,3423193
,3681504
,3930572
,4155049
,4397379
,4670959
,4909320
,5138262
,5393478
,5611473
,5826963
,6053776]

lat_sscq_100p  = [7875.2
,8022.4
,8169.6
,8316.8
,8684.8
,8832
,9494.4
,11187.2
,15235.2
,19136
,21638.4
,22227.2
,22742.4
,22816
,23404.8
,24140.8
,25686.4
,27084.8
,28924.8
,33193.6
,38787.2
,49017.6
,1016048]

tput_sscq_100p = [num / 1E6 for num in tput_sscq_100p]
lat_sscq_100p = [num / 1E3 for num in lat_sscq_100p]

ax.plot(tput_sscq_100p, lat_sscq_100p, '--', color ='black', linewidth = 2.0, label="grp_sh 100%", marker='o')
ax.plot(tput_scq_100p, lat_scq_100p, '--', color= 'blue', linewidth = 2.0, label="grp_ex 100%", marker='o')
ax.tick_params(axis='both', labelsize=16)
ax.set_xticks(np.arange(0, 8, 1))


#ax.legend(["group_ex 10%", "group_ex 100%"], loc = "center left",bbox_to_anchor=(1,0.5),fontsize=14,framealpha=0.5)
ax.legend(["grp_sh 100%", "grp_ex 100%"], loc = "upper left",fontsize=16,framealpha=0.5)

ax.set_ylabel('p99 latency (us)',fontsize=20)
ax.set_xlabel('Load (MRPS)',fontsize=20)
ax.set_ylim(0, 50)
#ax.set_xscale('log', basex=2)
#ax.set_yscale('log', basey=2)


#figure.tight_layout(pad=1.0)
figure.set_size_inches(4.5, 2.5)

plt.savefig('motivation_SYNC.pdf',bbox_inches='tight')


plt.clf()
figure, ((ax)) = plt.subplots(1, 1)

tput_sscq_100p = [299073
,596906
,891787
,1185150
,1476908
,1764539
,2046975
,2328912
,2608090
,2716052]

avg_kernel_ST_sscq_100p = [3102
,3176
,3252
,3288
,3273
,3316
,3369
,3419
,3408
,3367]


tput_scq_100p = [299165
,596745
,891951
,1185188
,1477039
,1764550
,2050047
,2330897
,2606985
,2888378
,3155709
,3430790
,3678836
,3933974
]

avg_kernel_ST_scq_100p = [2148
,2149
,2142
,2145
,2166
,2159
,2156
,2179
,2176
,2187
,2195
,2196
,2197
,2193
]


tput_sscq_100p = [num / 1E6 for num in tput_sscq_100p]
tput_scq_100p = [num / 1E6 for num in tput_scq_100p]

ax.plot(tput_sscq_100p, avg_kernel_ST_sscq_100p, '--', color ='black', linewidth = 2.0, label="grp_sh 100%", marker='o')
ax.plot(tput_scq_100p, avg_kernel_ST_scq_100p, '--', color= 'green', linewidth = 2.0, label="grp_ex 100%", marker='o')

ax.tick_params(axis='both', labelsize=16)

#ax.legend(["group_ex 10%", "group_ex 100%"], loc = "center left",bbox_to_anchor=(1,0.5),fontsize=14,framealpha=0.5)
ax.legend(["grp_sh 100%", "grp_ex 100%"], loc = "lower right",fontsize=16,framealpha=0.5)

ax.set_ylabel('Avg Kernel\nService Time (ns)',fontsize=20)
ax.set_xlabel('Load (MRPS)',fontsize=20)
ax.set_ylim(0, 4000)
#ax.set_xscale('log', basex=2)
#ax.set_yscale('log', basey=2)


#figure.tight_layout(pad=1.0)
figure.set_size_inches(4.5, 2.5)

plt.savefig('motivation_AFFINITY.pdf',bbox_inches='tight')


plt.clf()


##########################
#idle polling
figure, ((ax)) = plt.subplots(1, 1)

tput_no_grp_64Q_1T = [9620
,94882
,189482
,280136
,368502
,454128
]
lat_no_grp_64Q_1T = [
8684.8
,13027.2
,18547.2
,28409.6
,56156.8
,664248.096
]


tput_no_grp_128Q_1T = [
9244
,93735
,189099
,282788
,324397
]
lat_no_grp_128Q_1T = [
9309
,20902.4
,30396.8
,277603
,109222.4
]


tput_no_grp_256Q_1T = [
9970
,99817
,199711
]
lat_no_grp_256Q_1T = [
22153.6
,952972.8
,1043059.2
]


tput_no_grp_64Q_12T = [
285347
,548032
,795668
,868272
,1083893
,1186395
,1608430
,1629051
,2359499
,3075973
]
lat_no_grp_64Q_12T = [
10156.8
,11628.8
,13321.6
,14499.2
,16265.6
,16265.6
,17737.6
,26864
,95385.6
,104585.6
]


tput_no_grp_128Q_12T = [
9309
,282726
,561387
,808396
,923217
,1191984
,1884344
,2376149
,2479199
,3113355
]
lat_no_grp_128Q_12T = [
14499.2
,20828.8
,24214.4
,27526.4
,30249.6
,32236.8
,33561.6
,37536
,37536
,188710.4
]


tput_no_grp_256Q_12T = [
10024
,298357
,472866
,878765
,1157858
,1667094
]
lat_no_grp_256Q_12T = [
25833.6
,41584
,47398.4
,54832
,64105.6
,319497.6
]


tput_no_grp_64Q_1T = [num / 1E6 for num in tput_no_grp_64Q_1T]
tput_no_grp_128Q_1T = [num / 1E6 for num in tput_no_grp_128Q_1T]
tput_no_grp_256Q_1T = [num / 1E6 for num in tput_no_grp_256Q_1T]
tput_no_grp_64Q_12T = [num / 1E6 for num in tput_no_grp_64Q_12T]
tput_no_grp_128Q_12T = [num / 1E6 for num in tput_no_grp_128Q_12T]
tput_no_grp_256Q_12T = [num / 1E6 for num in tput_no_grp_256Q_12T]

lat_no_grp_64Q_1T = [num / 1E3 for num in lat_no_grp_64Q_1T]
lat_no_grp_128Q_1T = [num / 1E3 for num in lat_no_grp_128Q_1T]
lat_no_grp_256Q_1T = [num / 1E3 for num in lat_no_grp_256Q_1T]
lat_no_grp_64Q_12T = [num / 1E3 for num in lat_no_grp_64Q_12T]
lat_no_grp_128Q_12T = [num / 1E3 for num in lat_no_grp_128Q_12T]
lat_no_grp_256Q_12T = [num / 1E3 for num in lat_no_grp_256Q_12T]


ax.plot(tput_no_grp_64Q_1T,  lat_no_grp_64Q_1T,  '--', color ='red', linewidth = 2.0, label="64Q_1C")#, marker='o')
ax.plot(tput_no_grp_128Q_1T, lat_no_grp_128Q_1T, '--', color= 'blue', linewidth = 2.0, label="128Q_1C")#, marker='o')
ax.plot(tput_no_grp_256Q_1T, lat_no_grp_256Q_1T, '--', color= 'black', linewidth = 2.0, label="256Q_1C")#, marker='o')

ax.plot(tput_no_grp_64Q_12T,  lat_no_grp_64Q_12T,  '-', color ='red', linewidth = 2.0, label="64Q_12C")#, marker='o')
ax.plot(tput_no_grp_128Q_12T, lat_no_grp_128Q_12T, '-', color= 'blue', linewidth = 2.0, label="128Q_12C")#, marker='o')
ax.plot(tput_no_grp_256Q_12T, lat_no_grp_256Q_12T, '-', color= 'black', linewidth = 2.0, label="256Q_12C")#, marker='o')

ax.tick_params(axis='both', labelsize=16)

#ax.legend(["group_ex 10%", "group_ex 100%"], loc = "center left",bbox_to_anchor=(1,0.5),fontsize=14,framealpha=0.5)
ax.legend(["64Q_1C", "128Q_1C", "256Q_1C", "64Q_12C", "128Q_12C", "256Q_12C"], loc = "lower right",fontsize=12,framealpha=0.5)

ax.set_ylabel('p99 latency (us)',fontsize=20)
ax.set_xlabel('Load (MRPS)',fontsize=20)
ax.set_ylim(0, 70)
ax.set_title('no_grp, 10% active connections', fontsize=20)

#ax.set_xscale('log', basex=2)
#ax.set_yscale('log', basey=2)


#figure.tight_layout(pad=1.0)
figure.set_size_inches(4.5, 2.5)

plt.savefig('motivation_IDLEPOLLING.pdf',bbox_inches='tight')


plt.clf()