import matplotlib.pyplot as plt
import os
import numpy as np
#if not  os.path.exists('graphs_bm_r5_p10_30x_SLO'):
#	os.makedirs('graphs_bm_r5_p10_30x_SLO')
from matplotlib.ticker import FuncFormatter

#load imbalance
figure, ((ax)) = plt.subplots(1, 1)

tput_scq_100p = [299146
,596777
,892358
,1185276
,1477016
,1765306
,2047064
,2332710
,2606337
,2881059
,3147761
,3683497
,3922036
,4186118
,4434212
,4699207
,4942316]
lat_scq_100p  = [8832
,9126.4
,9494.4
,10156.8
,10672
,11334.4
,12070.4
,12953.6
,14057.6
,14867.2
,16339.2
,21049.6
,24656
,29145.6
,41363.2
,76028.8
,3464646.4]
tput_scq_100p = [num / 1E6 for num in tput_scq_100p]
lat_scq_100p = [num / 1E3 for num in lat_scq_100p]


tput_scq_20p = [299117
,596740
,891730
,1185897
,1478217
,1765640
,2051881
,2616030
,2892470
,3163225
,3441701
,3704717
,3955365
,4221738]
lat_scq_20p  = [6550.4
,6697.6
,6918.4
,7286.4
,7654.4
,8022.4
,8464
,9494.4
,10304
,11187.2
,12585.6
,16486.4
,26569.6
,94649.6]
tput_scq_20p = [num / 1E6 for num in tput_scq_20p]
lat_scq_20p = [num / 1E3 for num in lat_scq_20p]

tput_scq_10p = [299003
,596532
,892004
,1186054
,1477732
,1766221
,2051809
,2336464
,2614173
,2892181
,3159648
,3438307]
lat_scq_10p  = [5961.6
,6182.4
,6550.4
,6844.8
,7286.4
,7875.2
,8390.4
,9420.8
,10377.6
,12953.6
,16928
,1629577.6]
tput_scq_10p = [num / 1E6 for num in tput_scq_10p]
lat_scq_10p = [num / 1E3 for num in lat_scq_10p]
    
tput_sscq_100p = [299287
,596776
,891756
,1185025
,1478124
,1764390
,2049358
,2328656
,2606507
,2878236
,3134267
,3422888]
lat_sscq_100p  = [9936
,10230.4
,10672
,10892.8
,11996.8
,16560
,34297.6
,40112
,43203.2
,52844.8
,71392
,562156.8]
tput_sscq_100p = [num / 1E6 for num in tput_sscq_100p]
lat_sscq_100p = [num / 1E3 for num in lat_sscq_100p]

tput_sscq_10p = [87418
,596881
,892206
,1186416
,1477572
,1767402
,2051670
,2336368
,2613965
,2889132
,3157791
,3427516]
lat_sscq_10p  = [6108.8
,6992
,7212.8
,7433.6
,7654.4
,8022.4
,8684.8
,10819.2
,16339.2
,20387.2
,23331.2
,1610883.2]
tput_sscq_10p = [num / 1E6 for num in tput_sscq_10p]
lat_sscq_10p = [num / 1E3 for num in lat_sscq_10p]

tput_sscq_20p = [299061
,596506
,892185
,1186336
,1478467
,1767264
,2052041
,2336751
,2613951
,2888991
,3158324
,3431703
,3696329]
lat_sscq_20p  = [7580.8
,7728
,7948.8
,8169.6
,8390.4
,9052.8
,10451.2
,15750.4
,22153.6
,24508.8
,25760
,30249.6
,3060288]
tput_sscq_20p = [num / 1E6 for num in tput_sscq_20p]
lat_sscq_20p = [num / 1E3 for num in lat_sscq_20p]

tput_sassy_100p = [299290
,892308
,1184905
,1476783
,1765826
,2047776
,2606327
,2878255
,3145175
,3674202]
lat_sassy_100p  = [10524.8
,10966.4
,11702.4
,12291.2
,12806.4
,13542.4
,16560
,18988.8
,27452.8
,232060.8]
tput_sassy_100p = [num / 1E6 for num in tput_sassy_100p]
lat_sassy_100p = [num / 1E3 for num in lat_sassy_100p]

tput_sassy_10p = [299101
,892033
,1185969
,1478218
,1767156
,2052272
,2336168
,2614180
,2890476
,3161214
,3437354
,3695643
,3954686
,4208463
,4470065
,4709962
,5430806]
lat_sassy_10p  = [6844.8
,7065.6
,7139.2
,7286.4
,7507.2
,7801.6
,8169.6
,8390.4
,8684.8
,8905.6
,9200
,9494.4
,9936
,10451.2
,11187.2
,12144
,1367488]
tput_sassy_10p = [num / 1E6 for num in tput_sassy_10p]
lat_sassy_10p = [num / 1E3 for num in lat_sassy_10p]

tput_sassy_20p = [299006
,596560
,892424
,1186094
,1478484
,1767353
,2051014
,2337049
,2614419
,2892452
,3162794
,3699335
,3957015
,4213731
,4471410
,4715338
,4960331
,5200402
,5445180
,5670207]
lat_sassy_20p  = [7580.8
,7580.8
,7728
,7801.6
,7948.8
,8096
,8464
,8758.4
,9052.8
,9273.6
,9494.4
,10230.4
,10745.6
,11481.6
,12732.8
,15088
,19209.6
,27232
,45705.6
,2213004.8]
tput_sassy_20p = [num / 1E6 for num in tput_sassy_20p]
lat_sassy_20p = [num / 1E3 for num in lat_sassy_20p]

tput_sassy_JAC1_100p = [299157
,892483
,1185523
,1477983
,1766759
,2050787
,2335322
,2612520
,2886783
,3157001
,3430795
,3490352]
lat_sassy_JAC1_100p  = [9641.6
,10598.4
,11260.8
,11849.6
,12364.8
,13027.2
,13910.4
,15014.4
,17148.8
,23625.6
,35180.8
,19662608]
tput_sassy_JAC1_100p = [num / 1E6 for num in tput_sassy_JAC1_100p]
lat_sassy_JAC1_100p = [num / 1E3 for num in lat_sassy_JAC1_100p]

tput_sassy_JAC2_100p = [299226
,891934
,1185970
,1478079
,1766898
,2051218
,2336168
,2613567
,2884764
,3155417
,3430370
,3475820]
lat_sassy_JAC2_100p  = [10598.4
,10672
,10966.4
,11260.8
,12144
,12806.4
,13836.8
,14940.8
,16560
,18915.2
,24214.4
,22355264]
tput_sassy_JAC2_100p = [num / 1E6 for num in tput_sassy_JAC2_100p]
lat_sassy_JAC2_100p = [num / 1E3 for num in lat_sassy_JAC2_100p]

tput_sassy_JAC4_100p = [299111
,892299
,1186359
,1478123
,1763062
,2049560
,2333573
,2610002
,2882693
,3150829
,3416870
,3490915]
lat_sassy_JAC4_100p  = [11113.6
,12217.6
,12585.6
,13174.4
,13984
,14646.4
,15824
,16928
,18400
,20092.8
,24067.2
,20079552]
tput_sassy_JAC4_100p = [num / 1E6 for num in tput_sassy_JAC4_100p]
lat_sassy_JAC4_100p = [num / 1E3 for num in lat_sassy_JAC4_100p]

tput_sassy_JAC1_20p = [299186
,892470
,1186486
,1478446
,1767603
,2052328
,2337729
,2615596
,2892102
,3163037
,3437566
,3701173
,3960876
,4223949
,4473735
,4713995
,4963755
,5207337
,5450266
,5684200]
lat_sassy_JAC1_20p  = [7212.8
,7360
,7507.2
,7654.4
,7875.2
,8022.4
,8243.2
,8537.6
,8832
,9126.4
,9420.8
,9715.2
,10083.2
,10451.2
,11555.2
,13027.2
,15529.6
,22374.4
,31942.4
,266284.8]
tput_sassy_JAC1_20p = [num / 1E6 for num in tput_sassy_JAC1_20p]
lat_sassy_JAC1_20p = [num / 1E3 for num in lat_sassy_JAC1_20p]

tput_sassy_JAC2_20p = [299293
,892617
,1185947
,1477929
,1766681
,2051916
,2337403
,2616096
,2892734
,3440052
,3701847
,3954515
,4221246
,4474233
,4719210
,4966165
,5207971
,5453912
,5681946
,5483148]
lat_sassy_JAC2_20p  = [7212.8
,7360
,7433.6
,7580.8
,7728
,7948.8
,8169.6
,8537.6
,8905.6
,9420.8
,9715.2
,10156.8
,10672
,12585.6
,11923.2
,12659.2
,15308.8
,19283.2
,29881.6
,4431676.8]
tput_sassy_JAC2_20p = [num / 1E6 for num in tput_sassy_JAC2_20p]
lat_sassy_JAC2_20p = [num / 1E3 for num in lat_sassy_JAC2_20p]

tput_sassy_JAC4_20p = [299118
,892637
,1186417
,1478533
,1766113
,2052265
,2336741
,2615466
,3438995
,3698915
,3960268
,4215201
,4469998
,4715812
,4961165
,5204077
,5446762
,5684662
,5524735]
lat_sassy_JAC4_20p  = [7433.6
,8022.4
,8464
,8758.4
,9126.4
,9347.2
,9715.2
,9936
,10966.4
,11481.6
,12070.4
,12806.4
,13395.2
,14499.2
,15676.8
,17443.2
,23920
,34739.2
,4290659.2]
tput_sassy_JAC4_20p = [num / 1E6 for num in tput_sassy_JAC4_20p]
lat_sassy_JAC4_20p = [num / 1E3 for num in lat_sassy_JAC4_20p]

tput_sassy_JAC1_10p = [299128
,892259
,1185747
,1477212
,1766482
,2051206
,2335757
,2613288
,2889485
,3159598
,3435414
,3699519
,3956335
,4213397
,4469418
,4714469
,4956250
,5200618]
lat_sassy_JAC1_10p  = [6476.8
,6771.2
,6918.4
,7065.6
,7212.8
,7433.6
,7728
,8022.4
,8316.8
,8611.2
,8832
,9052.8
,9494.4
,10009.6
,11260.8
,11408
,12953.6
,1307062.4]
tput_sassy_JAC1_10p = [num / 1E6 for num in tput_sassy_JAC1_10p]
lat_sassy_JAC1_10p = [num / 1E3 for num in lat_sassy_JAC1_10p]

tput_sassy_JAC2_10p = [299085
,891988
,1186046
,1477613
,1765902
,2050195
,2335696
,2614218
,2890852
,3159821
,3437307
,3698689
,4213667
,4466910
,4706035
,4960292
,5203158]
lat_sassy_JAC2_10p  = [6476.8
,6697.6
,6844.8
,6992
,7139.2
,7360
,7728
,7948.8
,8316.8
,8611.2
,8905.6
,9347.2
,10083.2
,10745.6
,12659.2
,13174.4
,1250979.2]
tput_sassy_JAC2_10p = [num / 1E6 for num in tput_sassy_JAC2_10p]
lat_sassy_JAC2_10p = [num / 1E3 for num in lat_sassy_JAC2_10p]

tput_sassy_JAC4_10p = [299230
,892243
,1186134
,1477787
,1766675
,2051479
,2336491
,2615566
,2890963
,3159973
,3430994
,3699633
,4210132
,4466480
,4718578
,4949512
,5196751]
lat_sassy_JAC4_10p  = [6771.2
,7580.8
,8022.4
,8316.8
,8758.4
,9052.8
,9420.8
,9715.2
,10009.6
,10377.6
,10819.2
,11334.4
,12585.6
,13468.8
,15088
,17148.8
,233974.4]
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

ax.set_ylabel('p99 latency\n(us)',fontsize=20)
ax.set_xlabel('Load (MRPS)',fontsize=20)
ax.set_ylim(0, 50)
ax.set_xlim(0, 5)
#ax.set_xscale('log', basex=2)
#ax.set_yscale('log', basey=2)


#figure.tight_layout(pad=1.0)
figure.set_size_inches(3.5, 2.0)

plt.savefig('ubench_ro_100.pdf',bbox_inches='tight')

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
ax.set_xlim(0, 6)

#ax.set_xscale('log', basex=2)
#ax.set_yscale('log', basey=2)


#figure.tight_layout(pad=1.0)
figure.set_size_inches(3.5, 2.0)

plt.savefig('ubench_ro_20.pdf',bbox_inches='tight')

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
ax.set_xlim(0, 6)

#ax.set_xscale('log', basex=2)
#ax.set_yscale('log', basey=2)


#figure.tight_layout(pad=1.0)
figure.set_size_inches(3.5, 2.0)

plt.savefig('ubench_ro_10.pdf',bbox_inches='tight')

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
,0
,0
,0]

rebal_sscq_100p  = [270599
,543019
,813168
,1081582
,1349475
,1610809
,1869086
,2119784
,2375050
,2622272
,2860178
,3130179]
rebal_sscq_20p  = [270545
,543642
,815252
,1085619
,1354274
,1620412
,1882938
,2145996
,2406078
,2668807
,2922078
,3179428
,3427389]
rebal_sscq_10p  = [79335
,544542
,816456
,1087754
,1356876
,1625032
,1888510
,2153113
,2413095
,2677473
,2940698
,3199691]

rebal_sassy_100p  = [226176
,783636
,1057406
,1321440
,1577574
,1824402
,2309250
,2540353
,2748640
,2743685]
rebal_sassy_20p  = [208888
,475049
,745069
,1011675
,1270918
,1519418
,1754461
,1984720
,2200375
,2413572
,2616014
,3002263
,3179696
,3342220
,3492030
,3603528
,3664106
,3622676
,3364751
,2845501]
rebal_sassy_10p  = [200694
,717858
,970598
,1214453
,1445918
,1664138
,1868613
,2059767
,2237238
,2409071
,2573170
,2719787
,2855039
,2975854
,3090897
,3164890
,2930356]

rebal_sassy_JAC1_100p  = [88195
,475132
,724035
,942997
,1116156
,1277568
,1461568
,1650089
,1822565
,1974815
,2123984
,485]
rebal_sassy_JAC1_20p  = [72695
,389744
,601009
,829963
,1060456
,1264361
,1436920
,1581383
,1722077
,1870965
,2035483
,2195155
,2342342
,2479179
,2586767
,2682821
,2771018
,2807669
,2767807
,2493145]
rebal_sassy_JAC1_10p  = [64844
,352820
,539338
,744415
,963611
,1169152
,1351388
,1503484
,1629785
,1742744
,1865049
,1987329
,2107171
,2222871
,2311833
,2398106
,2436203
,2406131]

rebal_sassy_JAC2_100p  = [11637
,100654
,180382
,285553
,410187
,524371
,632480
,730290
,823631
,911240
,992157
,17850]
rebal_sassy_JAC2_20p  = [7831
,72906
,126277
,195502
,282310
,384416
,495755
,609319
,715487
,900721
,983215
,1062370
,1143489
,1206301
,1280140
,1341539
,1390245
,1438005
,1455233
,942321]
rebal_sassy_JAC2_10p  = [7019
,64302
,109453
,167294
,237376
,322035
,420931
,521363
,627770
,723443
,813008
,899654
,1039878
,1106862
,1158667
,1217474
,1237586]

rebal_sassy_JAC4_100p  = [124
,3569
,9428
,20215
,38975
,67710
,110575
,160310
,215608
,275378
,347251
,10221]
rebal_sassy_JAC4_20p  = [74
,1810
,4123
,9180
,16509
,27481
,44297
,65497
,170777
,214212
,262222
,311810
,362004
,415625
,467440
,519029
,566650
,617578
,425636]
rebal_sassy_JAC4_10p  = [273
,1909
,4993
,7722
,13538
,22797
,33756
,52701
,71703
,103086
,133295
,172735
,261983
,310447
,357680
,405306
,457779]


print(str(len(tput_scq_100p))+" "+str(len(rebal_scq_100p)))
rebal_scq_100p = [num / 1E6 for num in rebal_scq_100p]
rebal_scq_100p = [req / reb if reb != 0 else float('inf') for req, reb in zip(tput_scq_100p, rebal_scq_100p)]
#print("rebal_scq_100p")
#print(rebal_scq_100p)

print(str(len(tput_sscq_100p))+" "+str(len(rebal_sscq_100p)))
rebal_sscq_100p = [num / 1E6 for num in rebal_sscq_100p]
rebal_sscq_100p  = [req / reb for req, reb in zip(tput_sscq_100p, rebal_sscq_100p)]
#print("rebal_sscq_100p")
#print(rebal_sscq_100p)

print(str(len(tput_sassy_100p))+" "+str(len(rebal_sassy_100p)))
rebal_sassy_100p = [num / 1E6 for num in rebal_sassy_100p]
rebal_sassy_100p  = [req / reb for req, reb in zip(tput_sassy_100p, rebal_sassy_100p)]
#print("rebal_sassy_100p")
#print(rebal_sassy_100p)

print(str(len(tput_sassy_JAC1_100p))+" "+str(len(rebal_sassy_JAC1_100p)))
rebal_sassy_JAC1_100p = [num / 1E6 for num in rebal_sassy_JAC1_100p]
rebal_sassy_JAC1_100p  = [req / reb for req, reb in zip(tput_sassy_JAC1_100p, rebal_sassy_JAC1_100p)]
#print("rebal_sassy_JAC1_100p")
#print(rebal_sassy_JAC1_100p)

print(str(len(tput_sassy_JAC2_100p))+" "+str(len(rebal_sassy_JAC2_100p)))
rebal_sassy_JAC2_100p = [num / 1E6 for num in rebal_sassy_JAC2_100p]
rebal_sassy_JAC2_100p  = [req / reb for req, reb in zip(tput_sassy_JAC2_100p, rebal_sassy_JAC2_100p)]
#print("rebal_sassy_JAC2_100p")
#print(rebal_sassy_JAC2_100p)

print(str(len(tput_sassy_JAC4_100p))+" "+str(len(rebal_sassy_JAC4_100p)))
rebal_sassy_JAC4_100p = [num / 1E6 for num in rebal_sassy_JAC4_100p]
rebal_sassy_JAC4_100p  = [req / reb for req, reb in zip(tput_sassy_JAC4_100p, rebal_sassy_JAC4_100p)]
#print("rebal_sassy_JAC4_100p")
#print(rebal_sassy_JAC4_100p)

print(str(len(tput_scq_20p))+" "+str(len(rebal_scq_20p)))
rebal_scq_20p = [num / 1E6 for num in rebal_scq_20p]
rebal_scq_20p = [req / reb if reb != 0 else float('inf') for req, reb in zip(tput_scq_20p, rebal_scq_20p)]
#print("rebal_scq_20p")
#print(rebal_scq_20p)

print(str(len(tput_sscq_20p))+" "+str(len(rebal_sscq_20p)))
rebal_sscq_20p = [num / 1E6 for num in rebal_sscq_20p]
rebal_sscq_20p  = [req / reb for req, reb in zip(tput_sscq_20p, rebal_sscq_20p)]
#print("rebal_sscq_20p")
#print(rebal_sscq_20p)

print(str(len(tput_sassy_20p))+" "+str(len(rebal_sassy_20p)))
rebal_sassy_20p = [num / 1E6 for num in rebal_sassy_20p]
rebal_sassy_20p  = [req / reb for req, reb in zip(tput_sassy_20p, rebal_sassy_20p)]
#print("rebal_sassy_20p")
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
#print("rebal_sassy_JAC1_20p")
#print(rebal_sassy_JAC1_20p)

print(str(len(tput_sassy_JAC2_20p))+" "+str(len(rebal_sassy_JAC2_20p)))
rebal_sassy_JAC2_20p = [num / 1E6 for num in rebal_sassy_JAC2_20p]


'''
print("tput")
print(tput_sassy_JAC2_20p)
print("rebal")
print(rebal_sassy_JAC2_20p)
'''
rebal_sassy_JAC2_20p  = [req / reb for req, reb in zip(tput_sassy_JAC2_20p, rebal_sassy_JAC2_20p)]
#print("rebal_sassy_JAC2_20p")
#print(rebal_sassy_JAC2_20p)

print(str(len(tput_sassy_JAC4_20p))+" "+str(len(rebal_sassy_JAC4_20p)))
rebal_sassy_JAC4_20p = [num / 1E6 for num in rebal_sassy_JAC4_20p]
rebal_sassy_JAC4_20p  = [req / reb for req, reb in zip(tput_sassy_JAC4_20p, rebal_sassy_JAC4_20p)]
#print("rebal_sassy_JAC4_20p")
#print(rebal_sassy_JAC4_20p)

print(str(len(tput_scq_10p))+" "+str(len(rebal_scq_10p)))
rebal_scq_10p = [num / 1E6 for num in rebal_scq_10p]
rebal_scq_10p = [req / reb if reb != 0 else float('inf') for req, reb in zip(tput_scq_10p, rebal_scq_10p)]
#print("rebal_scq_10p")
#print(rebal_scq_10p)

print(str(len(tput_sscq_10p))+" "+str(len(rebal_sscq_10p)))
rebal_sscq_10p = [num / 1E6 for num in rebal_sscq_10p]
rebal_sscq_10p  = [req / reb for req, reb in zip(tput_sscq_10p, rebal_sscq_10p)]
#print("rebal_sscq_10p")
#print(rebal_sscq_10p)


print(str(len(tput_sassy_10p))+" "+str(len(rebal_sassy_10p)))
rebal_sassy_10p = [num / 1E6 for num in rebal_sassy_10p]
rebal_sassy_10p  = [req / reb for req, reb in zip(tput_sassy_10p, rebal_sassy_10p)]
#print("rebal_sassy_10p")
#print(rebal_sassy_10p)

print(str(len(tput_sassy_JAC1_10p))+" "+str(len(rebal_sassy_JAC1_10p)))
rebal_sassy_JAC1_10p = [num / 1E6 for num in rebal_sassy_JAC1_10p]
rebal_sassy_JAC1_10p  = [req / reb for req, reb in zip(tput_sassy_JAC1_10p, rebal_sassy_JAC1_10p)]
#print("rebal_sassy_JAC1_10p")
#print(rebal_sassy_JAC1_10p)


print(str(len(tput_sassy_JAC2_10p))+" "+str(len(rebal_sassy_JAC2_10p)))
rebal_sassy_JAC2_10p = [num / 1E6 for num in rebal_sassy_JAC2_10p]
rebal_sassy_JAC2_10p  = [req / reb for req, reb in zip(tput_sassy_JAC2_10p, rebal_sassy_JAC2_10p)]
#print("rebal_sassy_JAC2_10p")
#print(rebal_sassy_JAC2_10p)

print(str(len(tput_sassy_JAC4_10p))+" "+str(len(rebal_sassy_JAC4_10p)))
rebal_sassy_JAC4_10p = [num / 1E6 for num in rebal_sassy_JAC4_10p]
rebal_sassy_JAC4_10p  = [req / reb for req, reb in zip(tput_sassy_JAC4_10p, rebal_sassy_JAC4_10p)]
#print("rebal_sassy_JAC4_10p")
#print(rebal_sassy_JAC4_10p)

figure, ((ax)) = plt.subplots(1, 1)
ax.plot(tput_scq_10p, rebal_scq_10p, '--', color ='orchid', linewidth = 2.0, label="grp_ex", marker='o')
ax.plot(tput_sscq_10p, rebal_sscq_10p, '--', color= 'purple', linewidth = 2.0, label="grp_sh", marker='o')
ax.plot(tput_sassy_10p, rebal_sassy_10p, '-', color= 'navy', linewidth = 2.0, label="Sassy_JSC", marker='o')
ax.plot(tput_sassy_JAC1_10p, rebal_sassy_JAC1_10p, '-', color= 'blue', linewidth = 2.0, label="Sassy_JAC", marker='o')
ax.plot(tput_sassy_JAC2_10p, rebal_sassy_JAC2_10p, '-', color= 'cornflowerblue', linewidth = 2.0, label="Sassy_JAC_2", marker='o')
ax.plot(tput_sassy_JAC4_10p, rebal_sassy_JAC4_10p, '-', color= 'lightsteelblue', linewidth = 2.0, label="Sassy_JAC_4", marker='o')
ax.tick_params(axis='both', labelsize=16)
#ax.set_xticks(np.arange(0, 8, 1))

#ax.legend(["group_ex 10%", "group_ex 100%"], loc = "center left",bbox_to_anchor=(1,0.5),fontsize=14,framealpha=0.5)
#ax.legend(["grp_ex", "grp_sh", "Sassy"], loc = "upper left",fontsize=16,framealpha=0.5)

#ax.set_title('10%', fontsize=20)
plt.yscale('log')
ax.set_xlim(0, 6)

#ax.set_ylabel('Requests/rebalanes',fontsize=20)
ax.set_xlabel('Load (MRPS)',fontsize=20)
ax.set_ylim(1, 1000)
#ax.set_xscale('log', basex=2)
#ax.set_yscale('log', basey=2)


#figure.tight_layout(pad=1.0)
figure.set_size_inches(3.5, 2.0)

plt.savefig('ubench_ro_rebal_10.pdf',bbox_inches='tight')

plt.clf()

figure, ((ax)) = plt.subplots(1, 1)
ax.plot(tput_scq_20p, rebal_scq_20p, '--', color ='orchid', linewidth = 2.0, label="grp_ex", marker='o')
ax.plot(tput_sscq_20p, rebal_sscq_20p, '--', color= 'purple', linewidth = 2.0, label="grp_sh", marker='o')
ax.plot(tput_sassy_20p, rebal_sassy_20p, '-', color= 'navy', linewidth = 2.0, label="Sassy_JSC", marker='o')
ax.plot(tput_sassy_JAC1_20p[:-1], rebal_sassy_JAC1_20p[:-1], '-', color= 'blue', linewidth = 2.0, label="Sassy_JAC", marker='o')
ax.plot(tput_sassy_JAC2_20p[:-1], rebal_sassy_JAC2_20p[:-1], '-', color= 'cornflowerblue', linewidth = 2.0, label="Sassy_JAC_2", marker='o')
ax.plot(tput_sassy_JAC4_20p[:-1], rebal_sassy_JAC4_20p[:-1], '-', color= 'lightsteelblue', linewidth = 2.0, label="Sassy_JAC_4", marker='o')
ax.tick_params(axis='both', labelsize=16)
#ax.set_xticks(np.arange(0, 8, 1))

#ax.legend(["group_ex 10%", "group_ex 100%"], loc = "center left",bbox_to_anchor=(1,0.5),fontsize=14,framealpha=0.5)
#ax.legend(["grp_ex", "grp_sh", "Sassy"], loc = "upper left",fontsize=16,framealpha=0.5)

#ax.set_title('10%', fontsize=20)
plt.yscale('log')
ax.set_xlim(0, 6)

#ax.set_ylabel('Requests/rebalanes',fontsize=20)
ax.set_xlabel('Load (MRPS)',fontsize=20)
ax.set_ylim(1, 1000)
#ax.set_xscale('log', basex=2)
#ax.set_yscale('log', basey=2)


#figure.tight_layout(pad=1.0)
figure.set_size_inches(3.5, 2.0)

plt.savefig('ubench_ro_rebal_20.pdf',bbox_inches='tight')

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
ax.set_xlim(0, 5)

ax.set_ylabel('Requests/\nrebalance',fontsize=20)
ax.set_xlabel('Load (MRPS)',fontsize=20)
ax.set_ylim(1, 1000)
#ax.set_xscale('log', basex=2)
#ax.set_yscale('log', basey=2)


#figure.tight_layout(pad=1.0)
figure.set_size_inches(3.5, 2.0)

plt.savefig('ubench_ro_rebal_100.pdf',bbox_inches='tight')

plt.clf()

ast_scq_100p = [1590
,1565
,1556
,1595
,1595
,1592
,1596
,1606
,1617
,1599
,1603
,1620
,1637
,1622
,1647
,1646
,1648]
ast_scq_100p = [num / 1E3 for num in ast_scq_100p]

ast_sscq_100p = [2043
,2089
,2122
,2134
,2162
,2162
,2188
,2214
,2177
,2219
,2202
,2199]
ast_sscq_100p = [num / 1E3 for num in ast_sscq_100p]

ast_sassy_JSC_100p = [2134
,2164
,2154
,2180
,2230
,2204
,2154
,2143
,2211
,2221]
ast_sassy_JSC_100p = [num / 1E3 for num in ast_sassy_JSC_100p]

ast_sassy_JAC1_100p  = [1981
,2101
,2120
,2123
,2140
,2123
,2132
,2193
,2155
,2227
,2195
,2195]
ast_sassy_JAC1_100p = [num / 1E3 for num in ast_sassy_JAC1_100p]

ast_sassy_JAC2_100p  = [1763
,1843
,1915
,1920
,1958
,1989
,2017
,2032
,2034
,2245
,2230
,2244]
ast_sassy_JAC2_100p = [num / 1E3 for num in ast_sassy_JAC2_100p]

ast_sassy_JAC4_100p  = [1694
,1706
,1735
,1764
,1804
,1824
,1829
,1835
,1888
,2227
,2197
,2243]
ast_sassy_JAC4_100p = [num / 1E3 for num in ast_sassy_JAC4_100p]

ast_scq_20p = [1307
,1305
,1305
,1304
,1303
,1303
,1304
,1303
,1302
,1302
,1302
,1304
,1303
,1303]
ast_scq_20p = [num / 1E3 for num in ast_scq_20p]

ast_sscq_20p = [1402
,1407
,1412
,1414
,1415
,1417
,1418
,1419
,1418
,1416
,1413
,1414
,1413]
ast_sscq_20p = [num / 1E3 for num in ast_sscq_20p]

ast_sassy_JSC_20p = [1390
,1395
,1402
,1396
,1396
,1395
,1394
,1395
,1395
,1394
,1394
,1394
,1392
,1392
,1391
,1390
,1389
,1386
,1375
,1362]
ast_sassy_JSC_20p = [num / 1E3 for num in ast_sassy_JSC_20p]

ast_sassy_JAC1_20p  = [1360
,1355
,1362
,1368
,1371
,1371
,1370
,1370
,1371
,1369
,1371
,1370
,1368
,1370
,1370
,1369
,1366
,1364
,1356
,1344]
ast_sassy_JAC1_20p = [num / 1E3 for num in ast_sassy_JAC1_20p]

ast_sassy_JAC2_20p  = [1319
,1322
,1322
,1324
,1326
,1330
,1323
,1333
,1339
,1335
,1335
,1335
,1336
,1336
,1336
,1326
,1336
,1336
,1335
,1330
]
ast_sassy_JAC2_20p = [num / 1E3 for num in ast_sassy_JAC2_20p]

ast_sassy_JAC4_20p  = [1312
,1311
,1310
,1310
,1311
,1311
,1311
,1310
,1313
,1314
,1315
,1310
,1316
,1318
,1319
,1318
,1318
,1316
,1308]
ast_sassy_JAC4_20p = [num / 1E3 for num in ast_sassy_JAC4_20p]


ast_scq_10p = [1304
,1302
,1301
,1303
,1302
,1302
,1301
,1301
,1301
,1301
,1301
,1302]
ast_scq_10p = [num / 1E3 for num in ast_scq_10p]

ast_sscq_10p = [1379
,1377
,1378
,1380
,1381
,1382
,1382
,1383
,1382
,1382
,1381
,1379]
ast_sscq_10p = [num / 1E3 for num in ast_sscq_10p]

ast_sassy_JSC_10p = [1360
,1363
,1359
,1358
,1358
,1357
,1355
,1355
,1355
,1354
,1354
,1353
,1352
,1351
,1352
,1348
,1304]
ast_sassy_JSC_10p = [num / 1E3 for num in ast_sassy_JSC_10p]

ast_sassy_JAC1_10p  = [1330
,1329
,1335
,1338
,1342
,1342
,1342
,1341
,1342
,1340
,1335
,1339
,1340
,1340
,1339
,1337
,1335
,1306]
ast_sassy_JAC1_10p = [num / 1E3 for num in ast_sassy_JAC1_10p]

ast_sassy_JAC2_10p  = [1304
,1307
,1308
,1310
,1312
,1314
,1316
,1318
,1318
,1319
,1320
,1315
,1321
,1320
,1320
,1319
,1302]
ast_sassy_JAC2_10p = [num / 1E3 for num in ast_sassy_JAC2_10p]

ast_sassy_JAC4_10p  = [1299
,1304
,1304
,1303
,1303
,1303
,1304
,1304
,1304
,1304
,1305
,1296
,1308
,1308
,1309
,1309
,1305]
ast_sassy_JAC4_10p = [num / 1E3 for num in ast_sassy_JAC4_10p]


plt.clf()

figure, ((ax)) = plt.subplots(1, 1)
ax.plot(tput_scq_100p[:-2], ast_scq_100p[:-2], '--', color ='orchid', linewidth = 2.0, label="grp_ex", marker='o')
ax.plot(tput_sscq_100p, ast_sscq_100p, '--', color= 'purple', linewidth = 2.0, label="grp_sh", marker='o')
ax.plot(tput_sassy_100p[:-2], ast_sassy_JSC_100p[:-2], '-', color= 'navy', linewidth = 2.0, label="Sassy_JSC", marker='o')
ax.plot(tput_sassy_JAC1_100p[:-3], ast_sassy_JAC1_100p[:-3], '-', color= 'blue', linewidth = 2.0, label="Sassy_JAC", marker='o')
ax.plot(tput_sassy_JAC2_100p[:-3], ast_sassy_JAC2_100p[:-3], '-', color= 'cornflowerblue', linewidth = 2.0, label="Sassy_JAC_2", marker='o')
ax.plot(tput_sassy_JAC4_100p[:-3], ast_sassy_JAC4_100p[:-3], '-', color= 'lightsteelblue', linewidth = 2.0, label="Sassy_JAC_4", marker='o')
ax.tick_params(axis='both', labelsize=16)
#ax.set_xticks(np.arange(0, 8, 1))

#ax.legend(["group_ex 10%", "group_ex 100%"], loc = "center left",bbox_to_anchor=(1,0.5),fontsize=14,framealpha=0.5)
#ax.legend(["grp_ex", "grp_sh", "Sassy"], loc = "upper left",fontsize=16,framealpha=0.5)

#ax.set_title('10%', fontsize=20)

ax.set_ylabel('Avg Service\nTime (us)',fontsize=20)
ax.set_xlabel('Load (MRPS)',fontsize=20)
#ax.set_ylim(0, 2.5)
#ax.set_xscale('log', basex=2)
#ax.set_yscale('log', basey=2)

ax.set_xlim(0, 5)
formatter = FuncFormatter(lambda y, _: f'{y:.1f}')
ax.yaxis.set_major_formatter(formatter)

#figure.tight_layout(pad=1.0)
figure.set_size_inches(3.5, 2.0)

plt.savefig('ubench_ro_ast_100.pdf',bbox_inches='tight')


plt.clf()

figure, ((ax)) = plt.subplots(1, 1)
ax.plot(tput_scq_20p, ast_scq_20p, '--', color ='orchid', linewidth = 2.0, label="grp_ex", marker='o')
ax.plot(tput_sscq_20p, ast_sscq_20p, '--', color= 'purple', linewidth = 2.0, label="grp_sh", marker='o')
ax.plot(tput_sassy_20p[:-2], ast_sassy_JSC_20p[:-2], '-', color= 'navy', linewidth = 2.0, label="Sassy_JSC", marker='o')
ax.plot(tput_sassy_JAC1_20p[:-3], ast_sassy_JAC1_20p[:-3], '-', color= 'blue', linewidth = 2.0, label="Sassy_JAC", marker='o')
ax.plot(tput_sassy_JAC2_20p[:-2], ast_sassy_JAC2_20p[:-2], '-', color= 'cornflowerblue', linewidth = 2.0, label="Sassy_JAC_2", marker='o')
ax.plot(tput_sassy_JAC4_20p[:-2], ast_sassy_JAC4_20p[:-2], '-', color= 'lightsteelblue', linewidth = 2.0, label="Sassy_JAC_4", marker='o')
ax.tick_params(axis='both', labelsize=16)
#ax.set_xticks(np.arange(0, 8, 1))

#ax.legend(["group_ex 10%", "group_ex 100%"], loc = "center left",bbox_to_anchor=(1,0.5),fontsize=14,framealpha=0.5)
#ax.legend(["grp_ex", "grp_sh", "Sassy"], loc = "upper left",fontsize=16,framealpha=0.5)

#ax.set_title('10%', fontsize=20)
ax.set_xlim(0, 6)
ax.set_ylim(1.3, 1.5)

#ax.set_ylabel('Avg Service Time',fontsize=20)
ax.set_xlabel('Load (MRPS)',fontsize=20)
#ax.set_ylim(0, 1.5)
#ax.set_xscale('log', basex=2)
#ax.set_yscale('log', basey=2)
formatter = FuncFormatter(lambda y, _: f'{y:.1f}')
ax.yaxis.set_major_formatter(formatter)

#figure.tight_layout(pad=1.0)
figure.set_size_inches(3.5, 2.0)

plt.savefig('ubench_ro_ast_20.pdf',bbox_inches='tight')

plt.clf()

figure, ((ax)) = plt.subplots(1, 1)
ax.plot(tput_scq_10p, ast_scq_10p, '--', color ='orchid', linewidth = 2.0, label="grp_ex", marker='o')
ax.plot(tput_sscq_10p, ast_sscq_10p, '--', color= 'purple', linewidth = 2.0, label="grp_sh", marker='o')
ax.plot(tput_sassy_10p[:-1], ast_sassy_JSC_10p[:-1], '-', color= 'navy', linewidth = 2.0, label="Sassy_JSC", marker='o')
ax.plot(tput_sassy_JAC1_10p[:-1], ast_sassy_JAC1_10p[:-1], '-', color= 'blue', linewidth = 2.0, label="Sassy_JAC", marker='o')
ax.plot(tput_sassy_JAC2_10p[:-1], ast_sassy_JAC2_10p[:-1], '-', color= 'cornflowerblue', linewidth = 2.0, label="Sassy_JAC_2", marker='o')
ax.plot(tput_sassy_JAC4_10p[:-1], ast_sassy_JAC4_10p[:-1], '-', color= 'lightsteelblue', linewidth = 2.0, label="Sassy_JAC_4", marker='o')
ax.tick_params(axis='both', labelsize=16)
#ax.set_xticks(np.arange(0, 8, 1))

#ax.legend(["group_ex 10%", "group_ex 100%"], loc = "center left",bbox_to_anchor=(1,0.5),fontsize=14,framealpha=0.5)
#ax.legend(["grp_ex", "grp_sh", "Sassy"], loc = "upper left",fontsize=16,framealpha=0.5)

#ax.set_title('10%', fontsize=20)
ax.set_xlim(0, 6)
ax.set_ylim(1.3, 1.5)

#ax.set_ylabel('Avg Service Time',fontsize=20)
ax.set_xlabel('Load (MRPS)',fontsize=20)
#ax.set_ylim(0, 1.5)
#ax.set_xscale('log', basex=2)
#ax.set_yscale('log', basey=2)
formatter = FuncFormatter(lambda y, _: f'{y:.1f}')
ax.yaxis.set_major_formatter(formatter)
#figure.tight_layout(pad=1.0)
figure.set_size_inches(3.5, 2.0)

plt.savefig('ubench_ro_ast_10.pdf',bbox_inches='tight')