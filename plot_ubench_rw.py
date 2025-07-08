import matplotlib.pyplot as plt
import os
import numpy as np
#if not  os.path.exists('graphs_bm_r5_p10_30x_SLO'):
#	os.makedirs('graphs_bm_r5_p10_30x_SLO')
from matplotlib.ticker import FuncFormatter

#load imbalance
figure, ((ax)) = plt.subplots(1, 1)

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
,3933974]
lat_scq_100p  = [9862.4
,10598.4
,11408
,12217.6
,13174.4
,14572.8
,15971.2
,17664
,20534.4
,24214.4
,30691.2
,41436.8
,63222.4
,1314496]
tput_scq_100p = [num / 1E6 for num in tput_scq_100p]
lat_scq_100p = [num / 1E3 for num in lat_scq_100p]


tput_scq_20p = [299036
,596571
,1185860
,1478508
,1766812
,2051949
,2336982
,2616280
,2892205
,3160593
,3442563]
lat_scq_20p  = [7360
,7728
,8905.6
,9568
,10377.6
,11187.2
,12585.6
,14278.4
,19945.6
,56892.8
,110988.8]
tput_scq_20p = [num / 1E6 for num in tput_scq_20p]
lat_scq_20p = [num / 1E3 for num in lat_scq_20p]

tput_scq_10p = [298919
,596413
,892193
,337778
,1477675
,1765635
,2050782
,2337134
,2615299
,2887842]
lat_scq_10p  = [6771.2
,7286.4
,7948.8
,7580.8
,9641.6
,10745.6
,12806.4
,16265.6
,22963.2
,2313984]
tput_scq_10p = [num / 1E6 for num in tput_scq_10p]
lat_scq_10p = [num / 1E3 for num in lat_scq_10p]
    
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
lat_sscq_100p  = [10672
,11040
,11334.4
,11776
,12438.4
,14646.4
,45116.8
,73158.4
,177228.8
,24025984]
tput_sscq_100p = [num / 1E6 for num in tput_sscq_100p]
lat_sscq_100p = [num / 1E3 for num in lat_sscq_100p]

tput_sscq_10p = [298970
,596674
,892042
,1186064
,1478184
,1766447
,2049078
,2139243
,2211662]
lat_sscq_10p = [9200
,9715.2
,10304
,11113.6
,13248
,18841.6
,3054400
,4806153.6
,4462147.2
]
tput_sscq_10p = [num / 1E6 for num in tput_sscq_10p]
lat_sscq_10p = [num / 1E3 for num in lat_sscq_10p]

tput_sscq_20p = [299230
,597198
,892149
,1186321
,1478011
,1766910
,2052098
,2257641
,2337309]
lat_sscq_20p  = [9788.8
,10083.2
,10524.8
,11113.6
,12512
,16339.2
,67344
,7472608
,7824931.2]
tput_sscq_20p = [num / 1E6 for num in tput_sscq_20p]
lat_sscq_20p = [num / 1E3 for num in lat_sscq_20p]

tput_sassy_100p = [299162
,892000
,1186132
,1476983
,1764754
,2047480
,2329832
,2606788
,2882906]
lat_sassy_100p  = [11849.6
,13027.2
,13689.6
,14499.2
,15676.8
,17664
,22889.6
,41731.2
,160448]
tput_sassy_100p = [num / 1E6 for num in tput_sassy_100p]
lat_sassy_100p = [num / 1E3 for num in lat_sassy_100p]

tput_sassy_10p = [299117
,892402
,1185862
,1477376
,1765698
,2050713
,2337251
,2613184
,2890733
,3159521
,3699183
,3951243]
lat_sassy_10p  = [9862.4
,11702.4
,12070.4
,12806.4
,13836.8
,15088
,17296
,21491.2
,28630.4
,39376
,72496
,1679625.6]
tput_sassy_10p = [num / 1E6 for num in tput_sassy_10p]
lat_sassy_10p = [num / 1E3 for num in lat_sassy_10p]

tput_sassy_20p = [299281
,892463
,1185769
,1478598
,1766993
,2052159
,2336270
,2616869
,2892428
,3162231
,3439988
,3697559
,3958443]
lat_sassy_20p  = [10083.2
,11996.8
,12438.4
,13542.4
,14867.2
,17001.6
,22006.4
,34444.8
,61161.6
,99580.8
,150732.8
,213145.6
,300508.8]
tput_sassy_20p = [num / 1E6 for num in tput_sassy_20p]
lat_sassy_20p = [num / 1E3 for num in lat_sassy_20p]

tput_sassy_JAC1_100p = [299151
,1185995
,1477128
,1764684
,2049981
,2334142
,2609844
,2747576
,2921011]
lat_sassy_JAC1_100p  = [10745.6
,12953.6
,13616
,14425.6
,15897.6
,18400
,26716.8
,95280940.8
,23984326.4]
tput_sassy_JAC1_100p = [num / 1E6 for num in tput_sassy_JAC1_100p]
lat_sassy_JAC1_100p = [num / 1E3 for num in lat_sassy_JAC1_100p]

tput_sassy_JAC2_100p = [597101
,892307
,1185780
,1476816
,1766387
,2050516
,2335468
,2613002
,2887652
,2905428]
lat_sassy_JAC2_100p  = [11996.8
,12217.6
,12585.6
,13395.2
,14499.2
,15824
,17516.8
,20240
,30176
,24753225.6]
tput_sassy_JAC2_100p = [num / 1E6 for num in tput_sassy_JAC2_100p]
lat_sassy_JAC2_100p = [num / 1E3 for num in lat_sassy_JAC2_100p]

tput_sassy_JAC4_100p = [299208
,891793
,1185197
,1476517
,1765035
,2049838
,2333397
,2611068
,2887402
,2893746]
lat_sassy_JAC4_100p  = [12732.8
,14499.2
,15676.8
,16560
,17222.4
,18473.6
,20313.6
,23036.8
,27894.4
,24170976]
tput_sassy_JAC4_100p = [num / 1E6 for num in tput_sassy_JAC4_100p]
lat_sassy_JAC4_100p = [num / 1E3 for num in lat_sassy_JAC4_100p]

tput_sassy_JAC1_20p = [299208
,596938
,892161
,1186429
,1478046
,1765793
,2051883
,2335952
,2616153
,2892814
,3162247
,3439230
,3699055
,3952450]
lat_sassy_JAC1_20p  = [8684.8
,9788.8
,11260.8
,11776
,12217.6
,12732.8
,13689.6
,15382.4
,18473.6
,25833.6
,44748.8
,78310.4
,143446.4
,225068.8]
tput_sassy_JAC1_20p = [num / 1E6 for num in tput_sassy_JAC1_20p]
lat_sassy_JAC1_20p = [num / 1E3 for num in lat_sassy_JAC1_20p]

tput_sassy_JAC2_20p = [299054
,596610
,892553
,1186186
,1478557
,1766960
,2050799
,2336346
,2615113
,2891481
,3161679
,3438755
,3957021
,4217999
,4473427]
lat_sassy_JAC2_20p  = [8243.2
,8832
,9347.2
,9641.6
,10156.8
,10892.8
,11702.4
,12291.2
,13616
,14646.4
,16339.2
,19651.2
,65283.2
,184956.8
,435638.4]
tput_sassy_JAC2_20p = [num / 1E6 for num in tput_sassy_JAC2_20p]
lat_sassy_JAC2_20p = [num / 1E3 for num in lat_sassy_JAC2_20p]

tput_sassy_JAC4_20p = [299148
,892076
,1186216
,1478892
,1767006
,2051833
,2337212
,2615759
,2892374
,3163436
,3437548
,3702326
,3959101
,4217300
,4472423
,4715641]
lat_sassy_JAC4_20p  = [8684.8
,9788.8
,10377.6
,10892.8
,11555.2
,11996.8
,12512
,13248
,14204.8
,15382.4
,16928
,19136
,22300.8
,28556.8
,2689270.4
,2824768]
tput_sassy_JAC4_20p = [num / 1E6 for num in tput_sassy_JAC4_20p]
lat_sassy_JAC4_20p = [num / 1E3 for num in lat_sassy_JAC4_20p]

tput_sassy_JAC1_10p = [298976
,596872
,892243
,1185612
,1478029
,1766944
,2051005
,2336154
,2613371
,2889358
,3160220
,3436982
,3952534
,4213016]
lat_sassy_JAC1_10p  = [8096
,9126.4
,10672
,11260.8
,11702.4
,12070.4
,12806.4
,13910.4
,15676.8
,18768
,24656
,36064
,88172.8
,1598739.2]
tput_sassy_JAC1_10p = [num / 1E6 for num in tput_sassy_JAC1_10p]
lat_sassy_JAC1_10p = [num / 1E3 for num in lat_sassy_JAC1_10p]

tput_sassy_JAC2_10p = [299190
,596964
,892410
,1186469
,1477925
,1766615
,2051715
,2337669
,2614903
,2890719
,3159726
,3438605
,3697901
,3954989
,4212944]
lat_sassy_JAC2_10p  = [7580.8
,8243.2
,8905.6
,9126.4
,9494.4
,10083.2
,11040
,11702.4
,12585.6
,13689.6
,15014.4
,17296
,24361.6
,48355.2
,1477593.6]
tput_sassy_JAC2_10p = [num / 1E6 for num in tput_sassy_JAC2_10p]
lat_sassy_JAC2_10p = [num / 1E3 for num in lat_sassy_JAC2_10p]

tput_sassy_JAC4_10p = [298930
,892001
,1185905
,1478190
,1767013
,2051514
,2336580
,2613707
,2890900
,3158568
,3434346
,3696732
,3957976
,4213999]
lat_sassy_JAC4_10p  = [8022.4
,9273.6
,9862.4
,10672
,11260.8
,11702.4
,12364.8
,13174.4
,13984
,15014.4
,16486.4
,18400
,24656
,1609558.4]
tput_sassy_JAC4_10p = [num / 1E6 for num in tput_sassy_JAC4_10p]
lat_sassy_JAC4_10p = [num / 1E3 for num in lat_sassy_JAC4_10p]


ax.plot(tput_scq_100p, lat_scq_100p, '--', color ='orchid', linewidth = 2.0, label="grp_ex", marker='o')
ax.plot(tput_sscq_100p, lat_sscq_100p, '--', color= 'purple', linewidth = 2.0, label="grp_sh", marker='o')
ax.plot(tput_sassy_100p, lat_sassy_100p, '-', color= 'navy', linewidth = 2.0, label="Sassy_JSC", marker='o')
ax.plot(tput_sassy_JAC1_100p, lat_sassy_JAC1_100p, '-', color= 'blue', linewidth = 2.0, label="Sassy_JAC", marker='o')
ax.plot(tput_sassy_JAC2_100p, lat_sassy_JAC2_100p, '-', color= 'cornflowerblue', linewidth = 2.0, label="Sassy_JAC_2", marker='o')
ax.plot(tput_sassy_JAC4_100p, lat_sassy_JAC4_100p, '-', color= 'lightsteelblue', linewidth = 2.0, label="Sassy_JAC_4", marker='o')

ax.tick_params(axis='both', labelsize=16)
#ax.set_xticks(np.arange(0, 8, 1))

#ax.legend(["group_ex 10%", "group_ex 100%"], loc = "center left",bbox_to_anchor=(1,0.5),fontsize=14,framealpha=0.5)
#ax.legend(["grp_ex", "grp_sh", "Sassy"], loc = "upper left",fontsize=16,framealpha=0.5)

#ax.set_title('100%', fontsize=20)

#ax.set_ylabel('p99 latency (us)',fontsize=20)
ax.set_xlabel('Load (MRPS)',fontsize=20)
ax.set_ylim(0, 50)
ax.set_xlim(0, 4)

#ax.set_xscale('log', basex=2)
#ax.set_yscale('log', basey=2)


#figure.tight_layout(pad=1.0)
figure.set_size_inches(3.5, 2.0)

plt.savefig('ubench_rw_100.pdf',bbox_inches='tight')

plt.clf()
#plt.show()

figure, ((ax)) = plt.subplots(1, 1)

ax.plot(tput_scq_20p, lat_scq_20p, '--', color ='orchid', linewidth = 2.0, label="grp_ex", marker='o')
ax.plot(tput_sscq_20p, lat_sscq_20p, '--', color= 'purple', linewidth = 2.0, label="grp_sh", marker='o')
ax.plot(tput_sassy_20p, lat_sassy_20p, '-', color= 'navy', linewidth = 2.0, label="JSC", marker='o')
ax.plot(tput_sassy_JAC1_20p, lat_sassy_JAC1_20p, '-', color= 'blue', linewidth = 2.0, label="JAC_1", marker='o')
ax.plot(tput_sassy_JAC2_20p, lat_sassy_JAC2_20p, '-', color= 'cornflowerblue', linewidth = 2.0, label="JAC_2", marker='o')
ax.plot(tput_sassy_JAC4_20p, lat_sassy_JAC4_20p, '-', color= 'lightsteelblue', linewidth = 2.0, label="JAC_4", marker='o')

ax.tick_params(axis='both', labelsize=16)
#ax.set_xticks(np.arange(0, 8, 1))

#ax.legend(["grp_ex", "grp_sh", "JSC", "JAC_1", "JAC_2", "JAC_4"], loc = "center left",bbox_to_anchor=(1,0.5),fontsize=14,framealpha=0.5, ncol=6)
#ax.legend(["grp_ex", "grp_sh", "JSC", "JAC_1", "JAC_2", "JAC_4"], loc = "upper left",fontsize=15,framealpha=0.5)

#ax.set_title('20%', fontsize=20)

#ax.set_ylabel('p99 latency (us)',fontsize=20)
ax.set_xlabel('Load (MRPS)',fontsize=20)
ax.set_ylim(0, 50)
ax.set_xlim(0, 5)

#ax.set_xscale('log', basex=2)
#ax.set_yscale('log', basey=2)


#figure.tight_layout(pad=1.0)
figure.set_size_inches(3.5, 2.0)

plt.savefig('ubench_rw_20.pdf',bbox_inches='tight')

plt.clf()


figure, ((ax)) = plt.subplots(1, 1)
ax.plot(tput_scq_10p, lat_scq_10p, '--', color ='orchid', linewidth = 2.0, label="grp_ex", marker='o')
ax.plot(tput_sscq_10p, lat_sscq_10p, '--', color= 'purple', linewidth = 2.0, label="grp_sh", marker='o')
ax.plot(tput_sassy_10p, lat_sassy_10p, '-', color= 'navy', linewidth = 2.0, label="Sassy_JSC", marker='o')
ax.plot(tput_sassy_JAC1_10p, lat_sassy_JAC1_10p, '-', color= 'blue', linewidth = 2.0, label="Sassy_JAC", marker='o')
ax.plot(tput_sassy_JAC2_10p, lat_sassy_JAC2_10p, '-', color= 'cornflowerblue', linewidth = 2.0, label="Sassy_JAC_2", marker='o')
ax.plot(tput_sassy_JAC4_10p, lat_sassy_JAC4_10p, '-', color= 'lightsteelblue', linewidth = 2.0, label="Sassy_JAC_4", marker='o')

ax.tick_params(axis='both', labelsize=16)
#ax.set_xticks(np.arange(0, 8, 1))

#ax.legend(["group_ex 10%", "group_ex 100%"], loc = "center left",bbox_to_anchor=(1,0.5),fontsize=14,framealpha=0.5)
#ax.legend(["grp_ex", "grp_sh", "Sassy"], loc = "upper left",fontsize=16,framealpha=0.5)

#ax.set_title('10%', fontsize=20)

#ax.set_ylabel('p99 latency (us)',fontsize=20)
ax.set_xlabel('Load (MRPS)',fontsize=20)
ax.set_ylim(0, 50)
ax.set_xlim(0, 5)

#ax.set_xscale('log', basex=2)
#ax.set_yscale('log', basey=2)


#figure.tight_layout(pad=1.0)
figure.set_size_inches(3.5, 2.0)

plt.savefig('ubench_rw_10.pdf',bbox_inches='tight')

plt.clf()




###REBALANCES

rebal_scq_100p  = [0
,0
,0
,0
,0
,0
,0
,0
,0
,0
,0
,0
,0
,0]

rebal_scq_20p  = [0
,0
,0
,0
,0
,0
,0
,0
,0
,0
,0]
rebal_scq_10p  = [0
,0
,0
,0
,0
,0
,0
,0
,0
,0]

rebal_sscq_100p  = [270471
,543568
,814159
,1083320
,1351224
,1615560
,1875929
,2140427
,2398936
,2500262]
rebal_sscq_20p  = [270806
,544565
,815799
,1086567
,1355925
,1623858
,1894091
,2109643
,2193923]
rebal_sscq_10p  = [270795
,544836
,817276
,1089270
,1360753
,1631379
,1917439
,2030809
,2113334]

rebal_sassy_100p  = [231415
,791228
,1061807
,1320908
,1573485
,1819651
,2060579
,2272689
,2292876]
rebal_sassy_20p  = [220875
,763934
,1020982
,1263569
,1488589
,1698183
,1885568
,2003447
,1952509
,1785362
,1575519
,1362067
,1130453]
rebal_sassy_10p  = [215414
,733355
,972678
,1191983
,1388324
,1562809
,1712943
,1816619
,1837038
,1743502
,1348456
,1150819]

rebal_sassy_JAC1_100p  = [93805
,745822
,935520
,1095043
,1275714
,1465517
,1625504
,2299347
,158]
rebal_sassy_JAC1_20p  = [79583
,234744
,436630
,674902
,898216
,1070518
,1215918
,1372367
,1526712
,1642130
,1669126
,1579721
,1345197
,1125373]
rebal_sassy_JAC1_10p  = [71578
,213405
,395189
,608739
,830016
,1012212
,1149091
,1268733
,1386987
,1488286
,1532039
,1475790
,1118070
,913754]

rebal_sassy_JAC2_100p  = [50092
,111837
,201090
,319461
,439187
,549336
,647689
,740547
,822998
,49]
rebal_sassy_JAC2_20p  = [8806
,37222
,82305
,145415
,234366
,342125
,462288
,578573
,684202
,778022
,865945
,947532
,1011643
,835830
,590123]
rebal_sassy_JAC2_10p  = [7677
,33000
,73177
,127269
,199429
,290863
,398488
,516325
,620437
,711328
,792950
,869896
,916945
,923882
,816528]

rebal_sassy_JAC4_100p  = [143
,4599
,12878
,28609
,54564
,94696
,149012
,212102
,282868
,127]
rebal_sassy_JAC4_20p  = [139
,2235
,5752
,11960
,22730
,39623
,64982
,99906
,144680
,196913
,256972
,320842
,386454
,450605
,504972
,348995]
rebal_sassy_JAC4_10p  = [323
,2774
,5419
,11565
,18677
,31764
,50818
,81838
,116506
,163273
,217156
,278003
,340568
,396617]

print(str(len(tput_scq_100p))+" "+str(len(rebal_scq_100p)))
rebal_scq_100p = [num / 1E6 for num in rebal_scq_100p]
rebal_scq_100p = [req / reb if reb != 0 else float('inf') for req, reb in zip(tput_scq_100p, rebal_scq_100p)]
#print(rebal_scq_100p)

print(str(len(tput_sscq_100p))+" "+str(len(rebal_sscq_100p)))
rebal_sscq_100p = [num / 1E6 for num in rebal_sscq_100p]
rebal_sscq_100p  = [req / reb for req, reb in zip(tput_sscq_100p, rebal_sscq_100p)]
#print(rebal_sscq_100p)

print(str(len(tput_sassy_100p))+" "+str(len(rebal_sassy_100p)))
rebal_sassy_100p = [num / 1E6 for num in rebal_sassy_100p]
rebal_sassy_100p  = [req / reb for req, reb in zip(tput_sassy_100p, rebal_sassy_100p)]
#print(rebal_sassy_100p)

print(str(len(tput_sassy_JAC1_100p))+" "+str(len(rebal_sassy_JAC1_100p)))
rebal_sassy_JAC1_100p = [num / 1E6 for num in rebal_sassy_JAC1_100p]
rebal_sassy_JAC1_100p  = [req / reb for req, reb in zip(tput_sassy_JAC1_100p, rebal_sassy_JAC1_100p)]
#print(rebal_sassy_JAC1_100p)

print(str(len(tput_sassy_JAC2_100p))+" "+str(len(rebal_sassy_JAC2_100p)))
rebal_sassy_JAC2_100p = [num / 1E6 for num in rebal_sassy_JAC2_100p]
rebal_sassy_JAC2_100p  = [req / reb for req, reb in zip(tput_sassy_JAC2_100p, rebal_sassy_JAC2_100p)]
#print(rebal_sassy_JAC2_100p)

print(str(len(tput_sassy_JAC4_100p))+" "+str(len(rebal_sassy_JAC4_100p)))
rebal_sassy_JAC4_100p = [num / 1E6 for num in rebal_sassy_JAC4_100p]
rebal_sassy_JAC4_100p  = [req / reb for req, reb in zip(tput_sassy_JAC4_100p, rebal_sassy_JAC4_100p)]
#print(rebal_sassy_JAC4_100p)

print(str(len(tput_scq_20p))+" "+str(len(rebal_scq_20p)))
rebal_scq_20p = [num / 1E6 for num in rebal_scq_20p]
rebal_scq_20p = [req / reb if reb != 0 else float('inf') for req, reb in zip(tput_scq_20p, rebal_scq_20p)]
#print(rebal_scq_20p)

print(str(len(tput_sscq_20p))+" "+str(len(rebal_sscq_20p)))
rebal_sscq_20p = [num / 1E6 for num in rebal_sscq_20p]
rebal_sscq_20p  = [req / reb for req, reb in zip(tput_sscq_20p, rebal_sscq_20p)]
#print(rebal_sscq_20p)

print(str(len(tput_sassy_20p))+" "+str(len(rebal_sassy_20p)))
rebal_sassy_20p = [num / 1E6 for num in rebal_sassy_20p]
rebal_sassy_20p  = [req / reb for req, reb in zip(tput_sassy_20p, rebal_sassy_20p)]
#print(rebal_sassy_20p)

print(str(len(tput_sassy_JAC1_20p))+" "+str(len(rebal_sassy_JAC1_20p)))
rebal_sassy_JAC1_20p = [num / 1E6 for num in rebal_sassy_JAC1_20p]
'''
print("tput")
print(tput_sassy_JAC1_20p)
print("rebal")
print(rebal_sassy_JAC1_20p)
'''
rebal_sassy_JAC1_20p  = [req / reb for req, reb in zip(tput_sassy_JAC1_20p, rebal_sassy_JAC1_20p)]

print(str(len(tput_sassy_JAC2_20p))+" "+str(len(rebal_sassy_JAC2_20p)))
rebal_sassy_JAC2_20p = [num / 1E6 for num in rebal_sassy_JAC2_20p]
'''
print("tput")
print(tput_sassy_JAC2_20p)
print("rebal")
print(rebal_sassy_JAC2_20p)
'''
rebal_sassy_JAC2_20p  = [req / reb for req, reb in zip(tput_sassy_JAC2_20p, rebal_sassy_JAC2_20p)]


print(str(len(tput_sassy_JAC4_20p))+" "+str(len(rebal_sassy_JAC4_20p)))
rebal_sassy_JAC4_20p = [num / 1E6 for num in rebal_sassy_JAC4_20p]
rebal_sassy_JAC4_20p  = [req / reb for req, reb in zip(tput_sassy_JAC4_20p, rebal_sassy_JAC4_20p)]
#print(rebal_sassy_JAC4_20p)

print(str(len(tput_scq_10p))+" "+str(len(rebal_scq_10p)))
rebal_scq_10p = [num / 1E6 for num in rebal_scq_10p]
rebal_scq_10p = [req / reb if reb != 0 else float('inf') for req, reb in zip(tput_scq_10p, rebal_scq_10p)]
#print(rebal_scq_10p)

print(str(len(tput_sscq_10p))+" "+str(len(rebal_sscq_10p)))
rebal_sscq_10p = [num / 1E6 for num in rebal_sscq_10p]
rebal_sscq_10p  = [req / reb for req, reb in zip(tput_sscq_10p, rebal_sscq_10p)]
#print(rebal_sscq_10p)

print(str(len(tput_sassy_10p))+" "+str(len(rebal_sassy_10p)))
rebal_sassy_10p = [num / 1E6 for num in rebal_sassy_10p]
rebal_sassy_10p  = [req / reb for req, reb in zip(tput_sassy_10p, rebal_sassy_10p)]
#print(rebal_sassy_10p)

print(str(len(tput_sassy_JAC1_10p))+" "+str(len(rebal_sassy_JAC1_10p)))
rebal_sassy_JAC1_10p = [num / 1E6 for num in rebal_sassy_JAC1_10p]
rebal_sassy_JAC1_10p  = [req / reb for req, reb in zip(tput_sassy_JAC1_10p, rebal_sassy_JAC1_10p)]
#print(rebal_sassy_JAC1_10p)

print(str(len(tput_sassy_JAC2_10p))+" "+str(len(rebal_sassy_JAC2_10p)))
rebal_sassy_JAC2_10p = [num / 1E6 for num in rebal_sassy_JAC2_10p]
rebal_sassy_JAC2_10p  = [req / reb for req, reb in zip(tput_sassy_JAC2_10p, rebal_sassy_JAC2_10p)]
#print(rebal_sassy_JAC2_10p)

print(str(len(tput_sassy_JAC4_10p))+" "+str(len(rebal_sassy_JAC4_10p)))
rebal_sassy_JAC4_10p = [num / 1E6 for num in rebal_sassy_JAC4_10p]
rebal_sassy_JAC4_10p  = [req / reb for req, reb in zip(tput_sassy_JAC4_10p, rebal_sassy_JAC4_10p)]
#print(rebal_sassy_JAC4_10p)

figure, ((ax)) = plt.subplots(1, 1)
ax.plot(tput_scq_10p, rebal_scq_10p, '--', color ='orchid', linewidth = 2.0, label="grp_ex", marker='o')
ax.plot(tput_sscq_10p, rebal_sscq_10p, '--', color= 'purple', linewidth = 2.0, label="grp_sh", marker='o')
ax.plot(tput_sassy_10p[:-2], rebal_sassy_10p[:-2], '-', color= 'navy', linewidth = 2.0, label="Sassy_JSC", marker='o')
ax.plot(tput_sassy_JAC1_10p[:-2], rebal_sassy_JAC1_10p[:-2], '-', color= 'blue', linewidth = 2.0, label="Sassy_JAC", marker='o')
ax.plot(tput_sassy_JAC2_10p[:-2], rebal_sassy_JAC2_10p[:-2], '-', color= 'cornflowerblue', linewidth = 2.0, label="Sassy_JAC_2", marker='o')
ax.plot(tput_sassy_JAC4_10p[:-2], rebal_sassy_JAC4_10p[:-2], '-', color= 'lightsteelblue', linewidth = 2.0, label="Sassy_JAC_4", marker='o')
ax.tick_params(axis='both', labelsize=16)
#ax.set_xticks(np.arange(0, 8, 1))

#ax.legend(["group_ex 10%", "group_ex 100%"], loc = "center left",bbox_to_anchor=(1,0.5),fontsize=14,framealpha=0.5)
#ax.legend(["grp_ex", "grp_sh", "Sassy"], loc = "upper left",fontsize=16,framealpha=0.5)

#ax.set_title('10%', fontsize=20)
plt.yscale('log')
#ax.set_ylabel('Requests/rebalanes',fontsize=20)
ax.set_xlabel('Load (MRPS)',fontsize=20)
ax.set_xlim(0, 5)

ax.set_ylim(1, 1000)
#ax.set_xscale('log', basex=2)
#ax.set_yscale('log', basey=2)


#figure.tight_layout(pad=1.0)
figure.set_size_inches(3.5, 2.0)

plt.savefig('ubench_rw_rebal_10.pdf',bbox_inches='tight')

plt.clf()

figure, ((ax)) = plt.subplots(1, 1)
ax.plot(tput_scq_20p, rebal_scq_20p, '--', color ='orchid', linewidth = 2.0, label="grp_ex", marker='o')
ax.plot(tput_sscq_20p, rebal_sscq_20p, '--', color= 'purple', linewidth = 2.0, label="grp_sh", marker='o')
ax.plot(tput_sassy_20p[:-3], rebal_sassy_20p[:-3], '-', color= 'navy', linewidth = 2.0, label="Sassy_JSC", marker='o')
ax.plot(tput_sassy_JAC1_20p[:-3], rebal_sassy_JAC1_20p[:-3], '-', color= 'blue', linewidth = 2.0, label="Sassy_JAC", marker='o')
ax.plot(tput_sassy_JAC2_20p[:-3], rebal_sassy_JAC2_20p[:-3], '-', color= 'cornflowerblue', linewidth = 2.0, label="Sassy_JAC_2", marker='o')
ax.plot(tput_sassy_JAC4_20p[:-3], rebal_sassy_JAC4_20p[:-3], '-', color= 'lightsteelblue', linewidth = 2.0, label="Sassy_JAC_4", marker='o')
ax.tick_params(axis='both', labelsize=16)
#ax.set_xticks(np.arange(0, 8, 1))

#ax.legend(["group_ex 10%", "group_ex 100%"], loc = "center left",bbox_to_anchor=(1,0.5),fontsize=14,framealpha=0.5)
#ax.legend(["grp_ex", "grp_sh", "Sassy"], loc = "upper left",fontsize=16,framealpha=0.5)

#ax.set_title('10%', fontsize=20)

#ax.set_ylabel('Requests/rebalanes',fontsize=20)
ax.set_xlabel('Load (MRPS)',fontsize=20)
plt.yscale('log')
ax.set_xlim(0, 5)

ax.set_ylim(1, 1000)
#ax.set_xscale('log', basex=2)
#ax.set_yscale('log', basey=2)


#figure.tight_layout(pad=1.0)
figure.set_size_inches(3.5, 2.0)

plt.savefig('ubench_rw_rebal_20.pdf',bbox_inches='tight')

plt.clf()

figure, ((ax)) = plt.subplots(1, 1)
ax.plot(tput_scq_100p, rebal_scq_100p, '--', color ='orchid', linewidth = 2.0, label="grp_ex", marker='o')
ax.plot(tput_sscq_100p, rebal_sscq_100p, '--', color= 'purple', linewidth = 2.0, label="grp_sh", marker='o')
ax.plot(tput_sassy_100p, rebal_sassy_100p, '-', color= 'navy', linewidth = 2.0, label="Sassy_JSC", marker='o')
ax.plot(tput_sassy_JAC1_100p[:-1], rebal_sassy_JAC1_100p[:-1], '-', color= 'blue', linewidth = 2.0, label="Sassy_JAC", marker='o')
ax.plot(tput_sassy_JAC2_100p[:-1], rebal_sassy_JAC2_100p[:-1], '-', color= 'cornflowerblue', linewidth = 2.0, label="Sassy_JAC_2", marker='o')
ax.plot(tput_sassy_JAC4_100p[:-1], rebal_sassy_JAC4_100p[:-1], '-', color= 'lightsteelblue', linewidth = 2.0, label="Sassy_JAC_4", marker='o')
ax.tick_params(axis='both', labelsize=16)
#ax.set_xticks(np.arange(0, 8, 1))

#ax.legend(["group_ex 10%", "group_ex 100%"], loc = "center left",bbox_to_anchor=(1,0.5),fontsize=14,framealpha=0.5)
#ax.legend(["grp_ex", "grp_sh", "Sassy"], loc = "upper left",fontsize=16,framealpha=0.5)

#ax.set_title('10%', fontsize=20)
plt.yscale('log')
#ax.set_ylabel('Requests/rebalanes',fontsize=20)
ax.set_xlabel('Load (MRPS)',fontsize=20)
ax.set_xlim(0, 4)

ax.set_ylim(1, 1000)
#ax.set_xscale('log', basex=2)
#ax.set_yscale('log', basey=2)


#figure.tight_layout(pad=1.0)
figure.set_size_inches(3.5, 2.0)

plt.savefig('ubench_rw_rebal_100.pdf',bbox_inches='tight')

plt.clf()

ast_scq_100p = [2148
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
,2193]
ast_scq_100p = [num / 1E3 for num in ast_scq_100p]

ast_sscq_100p = [3102
,3176
,3252
,3288
,3273
,3316
,3369
,3419
,3408
,3367]
ast_sscq_100p = [num / 1E3 for num in ast_sscq_100p]

ast_sassy_JSC_100p = [3224
,3268
,3290
,3306
,3302
,3114
,2890
,2904
,2892]
ast_sassy_JSC_100p = [num / 1E3 for num in ast_sassy_JSC_100p]

ast_sassy_JAC1_100p  = [2963
,2981
,3000
,3018
,3025
,3067
,3128
,2888
,2891]
ast_sassy_JAC1_100p = [num / 1E3 for num in ast_sassy_JAC1_100p]

ast_sassy_JAC2_100p  = [2360
,2425
,2502
,2571
,2609
,2669
,2691
,2694
,2722
,2905]
ast_sassy_JAC2_100p = [num / 1E3 for num in ast_sassy_JAC2_100p]

ast_sassy_JAC4_100p  = [2344
,2375
,2408
,2446
,2472
,2511
,2524
,2918
,2904
,2904]
ast_sassy_JAC4_100p = [num / 1E3 for num in ast_sassy_JAC4_100p]

ast_scq_20p = [1780
,1772
,1781
,1784
,1775
,1778
,1778
,1778
,1777
,1779
,1780]
ast_scq_20p = [num / 1E3 for num in ast_scq_20p]

ast_sscq_20p = [3490
,3581
,3657
,3735
,3808
,3878
,4093
,4311
,4213]
ast_sscq_20p = [num / 1E3 for num in ast_sscq_20p]

ast_sassy_JSC_20p = [3293
,3432
,3496
,3525
,3551
,3560
,3503
,3300
,3029
,2777
,2573
,2390
,2236]
ast_sassy_JSC_20p = [num / 1E3 for num in ast_sassy_JSC_20p]

ast_sassy_JAC1_20p  = [2004
,2297
,2578
,2812
,2941
,2968
,2971
,2994
,3019
,3012
,2938
,2761
,2563
,2393]
ast_sassy_JAC1_20p = [num / 1E3 for num in ast_sassy_JAC1_20p]

ast_sassy_JAC2_20p  = [1806
,1864
,1926
,1994
,2079
,2156
,2227
,2262
,2312
,2336
,2356
,2364
,2333
,2215
,2088]
ast_sassy_JAC2_20p = [num / 1E3 for num in ast_sassy_JAC2_20p]

ast_sassy_JAC4_20p  = [1796
,1809
,1818
,1828
,1841
,1858
,1880
,1904
,1930
,1957
,1981
,2007
,2016
,2042
,1975
,1860]
ast_sassy_JAC4_20p = [num / 1E3 for num in ast_sassy_JAC4_20p]


ast_scq_10p = [1765
,1763
,1762
,1764
,1762
,1763
,1766
,1763
,1762
,1763]
ast_scq_10p = [num / 1E3 for num in ast_scq_10p]

ast_sscq_10p = [3516
,3635
,3745
,3826
,3931
,4059
,4624
,4628
,4495]
ast_sscq_10p = [num / 1E3 for num in ast_sscq_10p]

ast_sassy_JSC_10p = [3418
,3387
,3373
,3354
,3324
,3277
,3158
,2970
,2990
,2540
,2212
,2004]
ast_sassy_JSC_10p = [num / 1E3 for num in ast_sassy_JSC_10p]

ast_sassy_JAC1_10p  = [1949
,2202
,2450
,2658
,2809
,2863
,2862
,2854
,2854
,2846
,2797
,2683
,2362
,2210]
ast_sassy_JAC1_10p = [num / 1E3 for num in ast_sassy_JAC1_10p]

ast_sassy_JAC2_10p  = [1766
,1808
,1870
,1927
,1993
,2064
,2132
,2192
,2227
,2251
,2270
,2281
,2279
,2254
,2177]
ast_sassy_JAC2_10p = [num / 1E3 for num in ast_sassy_JAC2_10p]

ast_sassy_JAC4_10p  = [1772
,1784
,1791
,1803
,1816
,1837
,1858
,1880
,1906
,1931
,1951
,1974
,1802
,1791]
ast_sassy_JAC4_10p = [num / 1E3 for num in ast_sassy_JAC4_10p]


plt.clf()

figure, ((ax)) = plt.subplots(1, 1)
ax.plot(tput_scq_100p[:-2], ast_scq_100p[:-2], '--', color ='orchid', linewidth = 2.0, label="grp_ex", marker='o')
ax.plot(tput_sscq_100p[:-2], ast_sscq_100p[:-2], '--', color= 'purple', linewidth = 2.0, label="grp_sh", marker='o')
ax.plot(tput_sassy_100p[:-4], ast_sassy_JSC_100p[:-4], '-', color= 'navy', linewidth = 2.0, label="Sassy_JSC", marker='o')
ax.plot(tput_sassy_JAC1_100p[:-3], ast_sassy_JAC1_100p[:-3], '-', color= 'blue', linewidth = 2.0, label="Sassy_JAC", marker='o')
ax.plot(tput_sassy_JAC2_100p[:-3], ast_sassy_JAC2_100p[:-3], '-', color= 'cornflowerblue', linewidth = 2.0, label="Sassy_JAC_2", marker='o')
ax.plot(tput_sassy_JAC4_100p[:-3], ast_sassy_JAC4_100p[:-3], '-', color= 'lightsteelblue', linewidth = 2.0, label="Sassy_JAC_4", marker='o')
ax.tick_params(axis='both', labelsize=16)
#ax.set_xticks(np.arange(0, 8, 1))

#ax.legend(["group_ex 10%", "group_ex 100%"], loc = "center left",bbox_to_anchor=(1,0.5),fontsize=14,framealpha=0.5)
#ax.legend(["grp_ex", "grp_sh", "Sassy"], loc = "upper left",fontsize=16,framealpha=0.5)

#ax.set_title('10%', fontsize=20)

#ax.set_ylabel('Avg Service Time',fontsize=20)
ax.set_xlabel('Load (MRPS)',fontsize=20)
#ax.set_ylim(0, 2.5)
#ax.set_xscale('log', basex=2)
#ax.set_yscale('log', basey=2)

ax.set_xlim(0, 4)
formatter = FuncFormatter(lambda y, _: f'{y:.1f}')
ax.yaxis.set_major_formatter(formatter)

#figure.tight_layout(pad=1.0)
figure.set_size_inches(3.5, 2.0)

plt.savefig('ubench_rw_ast_100.pdf',bbox_inches='tight')


plt.clf()

figure, ((ax)) = plt.subplots(1, 1)
ax.plot(tput_scq_20p, ast_scq_20p, '--', color ='orchid', linewidth = 2.0, label="grp_ex", marker='o')
ax.plot(tput_sscq_20p[:-4], ast_sscq_20p[:-4], '--', color= 'purple', linewidth = 2.0, label="grp_sh", marker='o')
ax.plot(tput_sassy_20p[:-5], ast_sassy_JSC_20p[:-5], '-', color= 'navy', linewidth = 2.0, label="Sassy_JSC", marker='o')
ax.plot(tput_sassy_JAC1_20p[:-3], ast_sassy_JAC1_20p[:-3], '-', color= 'blue', linewidth = 2.0, label="Sassy_JAC", marker='o')
ax.plot(tput_sassy_JAC2_20p[:-2], ast_sassy_JAC2_20p[:-2], '-', color= 'cornflowerblue', linewidth = 2.0, label="Sassy_JAC_2", marker='o')
ax.plot(tput_sassy_JAC4_20p[:-2], ast_sassy_JAC4_20p[:-2], '-', color= 'lightsteelblue', linewidth = 2.0, label="Sassy_JAC_4", marker='o')
ax.tick_params(axis='both', labelsize=16)
#ax.set_xticks(np.arange(0, 8, 1))

#ax.legend(["group_ex 10%", "group_ex 100%"], loc = "center left",bbox_to_anchor=(1,0.5),fontsize=14,framealpha=0.5)
#ax.legend(["grp_ex", "grp_sh", "Sassy"], loc = "upper left",fontsize=16,framealpha=0.5)

#ax.set_title('10%', fontsize=20)

#ax.set_ylabel('Avg Service Time',fontsize=20)
ax.set_xlabel('Load (MRPS)',fontsize=20)
#ax.set_ylim(0, 1.5)
#ax.set_xscale('log', basex=2)
#ax.set_yscale('log', basey=2)
ax.set_xlim(0, 5)
formatter = FuncFormatter(lambda y, _: f'{y:.1f}')
ax.yaxis.set_major_formatter(formatter)
#figure.tight_layout(pad=1.0)
figure.set_size_inches(3.5, 2.0)

plt.savefig('ubench_rw_ast_20.pdf',bbox_inches='tight')

plt.clf()

figure, ((ax)) = plt.subplots(1, 1)
ax.plot(tput_scq_10p, ast_scq_10p, '--', color ='orchid', linewidth = 2.0, label="grp_ex", marker='o')
ax.plot(tput_sscq_10p[:-5], ast_sscq_10p[:-5], '--', color= 'purple', linewidth = 2.0, label="grp_sh", marker='o')
ax.plot(tput_sassy_10p[:-3], ast_sassy_JSC_10p[:-3], '-', color= 'navy', linewidth = 2.0, label="Sassy_JSC", marker='o')
ax.plot(tput_sassy_JAC1_10p[:-2], ast_sassy_JAC1_10p[:-2], '-', color= 'blue', linewidth = 2.0, label="Sassy_JAC", marker='o')
ax.plot(tput_sassy_JAC2_10p[:-1], ast_sassy_JAC2_10p[:-1], '-', color= 'cornflowerblue', linewidth = 2.0, label="Sassy_JAC_2", marker='o')
ax.plot(tput_sassy_JAC4_10p[:-1], ast_sassy_JAC4_10p[:-1], '-', color= 'lightsteelblue', linewidth = 2.0, label="Sassy_JAC_4", marker='o')
ax.tick_params(axis='both', labelsize=16)
#ax.set_xticks(np.arange(0, 8, 1))

#ax.legend(["group_ex 10%", "group_ex 100%"], loc = "center left",bbox_to_anchor=(1,0.5),fontsize=14,framealpha=0.5)
#ax.legend(["grp_ex", "grp_sh", "Sassy"], loc = "upper left",fontsize=16,framealpha=0.5)

#ax.set_title('10%', fontsize=20)

#ax.set_ylabel('Avg Service Time',fontsize=20)
ax.set_xlabel('Load (MRPS)',fontsize=20)
#ax.set_ylim(0, 1.5)
#ax.set_xscale('log', basex=2)
#ax.set_yscale('log', basey=2)
formatter = FuncFormatter(lambda y, _: f'{y:.1f}')
ax.yaxis.set_major_formatter(formatter)
ax.set_xlim(0, 5)

#figure.tight_layout(pad=1.0)
figure.set_size_inches(3.5, 2.0)

plt.savefig('ubench_rw_ast_10.pdf',bbox_inches='tight')