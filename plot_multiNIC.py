import matplotlib.pyplot as plt
import os
import numpy as np
#if not  os.path.exists('graphs_bm_r5_p10_30x_SLO'):
#	os.makedirs('graphs_bm_r5_p10_30x_SLO')
from matplotlib.ticker import FuncFormatter

#load imbalance
figure, ((ax)) = plt.subplots(1, 1)


tput_JSC12 = [
988328
,1947067
,2861992
,3739734
,4557377
,5312218
,5438952
,5711857
,5980011
,6311170
,6473546
]

lat_JSC12 = [
6476.8
,6844.8
,7728
,8243.2
,9052.8
,10524.8
,10892.8
,12144
,14352
,27305.6
,2407014.4
]

tput_JSC12 = [num / 1E6 for num in tput_JSC12]
lat_JSC12 = [num / 1E3 for num in lat_JSC12]

tput_JSC6 = [
987845
,1948059
,2871779
,4558325
,4955070
,5311736
,5447021
,5603254
,5829922
,6036189
]

lat_JSC6 = [
6035.2
,6403.2
,7286.4
,9568
,10745.6
,12364.8
,17222.4
,22300.8
,24656
,1739020.8
]

tput_JSC6 = [num / 1E6 for num in tput_JSC6]
lat_JSC6 = [num / 1E3 for num in lat_JSC6]

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

tput_scq_10p = [num / 1E6 for num in tput_scq_10p]
lat_scq_10p = [num / 1E3 for num in lat_scq_10p]


ax.plot(tput_JSC12, lat_JSC12, '-', color ='orchid', linewidth = 2.0, label="JSC_12", marker='o')
ax.plot(tput_JSC6, lat_JSC6, '-', color= 'navy', linewidth = 2.0, label="2x JSC_6", marker='o')
ax.plot(tput_scq_10p, lat_scq_10p, '--', color ='gray', linewidth = 2.0, label="grp_ex", marker='o')

ax.tick_params(axis='both', labelsize=12)
#ax.set_xticks(np.arange(0, 8, 1))

ax.legend(["JSC_12", "2x JSC_6","grp_ex"], loc = "upper left", fontsize=12)#,bbox_to_anchor=(1,0.5),fontsize=14,framealpha=0.5, ncol=4)

#ax.legend(["group_ex 10%", "group_ex 100%"], loc = "center left",bbox_to_anchor=(1,0.5),fontsize=14,framealpha=0.5)
#ax.legend(["grp_ex", "grp_sh", "Sassy"], loc = "upper left",fontsize=16,framealpha=0.5)

#ax.set_title('100%', fontsize=20)

ax.set_ylabel('p99 latency (us)',fontsize=14)
ax.set_xlabel('Load (MRPS)',fontsize=14)
ax.set_ylim(0, 50)
ax.set_xlim(0, 7)

#ax.set_xscale('log', basex=2)
#ax.set_yscale('log', basey=2)
figure.set_size_inches(3.5, 2.0)

plt.savefig('ubench_multiNIC.pdf',bbox_inches='tight')

plt.clf()