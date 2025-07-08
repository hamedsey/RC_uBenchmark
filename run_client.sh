#!/usr/bin/env bash
#make
#i=1
dirName=$(echo $1)
#serverQpn=$(echo $2)
#queues=$(echo $2)
#serverIPAddr=$(echo $4)
#ib_devname=$(echo $5)
#gid=$(echo $6)

#./disable_icrc.sh
#ping 192.168.1.5 -c 3
#sleep 20
mkdir $dirName

#2 cores
: '
for serviceTime in 0;do #3 is EXP service time distribution
    #multi core

    for dist in 5;do #9,10
        echo $dist
        mkdir $dirName"/"$dist
        for queues in 1024;do #8 16 32 64 128 256;do
            mkdir $dirName"/"$dist"/"$queues
            for load in 775000 800000 825000 850000 875000 900000 1000000 1100000 1200000 1300000 1400000 1500000 1600000 1700000 1800000 1900000 2000000;do
                echo $load
                mkdir $dirName"/"$dist"/"$queues"/"$load
            
                ./RCC_Client -d mlx5_0 -s 20 -r 130 -g 3 -l $load 192.168.1.5 -z $dist -m $serviceTime -n $queues -t 2 -f $dirName"/"$dist"/"$queues"/"$load
                sleep 40
            done
        done
    done
    
    
    for dist in 6 7;do #9,10
        echo $dist
        mkdir $dirName"/"$dist
        for queues in 1024;do #8 16 32 64 128 256;do
            mkdir $dirName"/"$dist"/"$queues
            for load in 200000 400000 500000 600000 700000 725000 750000 775000 800000 825000 850000 875000 900000 1000000 1100000 1200000 1300000 1400000 1500000 1600000 1700000 1800000 1900000 2000000;do
                echo $load
                mkdir $dirName"/"$dist"/"$queues"/"$load
            
                ./RCC_Client -d mlx5_0 -s 20 -r 130 -g 3 -l $load 192.168.1.5 -z $dist -m $serviceTime -n $queues -t 2 -f $dirName"/"$dist"/"$queues"/"$load

                sleep 40
            done
        done
    done
    
    for dist in 5 7;do #9,10
        echo $dist
        mkdir $dirName"/"$dist
        for queues in 2;do #8 16 32 64 128 256;do
            mkdir $dirName"/"$dist"/"$queues
            for load in 200000 400000 500000 600000 700000 725000 750000 775000 800000 825000 850000 875000 900000 1000000 1100000 1200000 1300000 1400000 1500000 1600000 1700000 1800000 1900000 2000000;do
                echo $load
                mkdir $dirName"/"$dist"/"$queues"/"$load
            
                ./RCC_Client -d mlx5_0 -s 20 -r 130 -g 3 -l $load 192.168.1.5 -z $dist -m $serviceTime -n $queues -t 2 -f $dirName"/"$dist"/"$queues"/"$load

                sleep 40
            done
        done
    done
done
'
: '
#4 cores
for serviceTime in 0;do #3 is EXP service time distribution
    #multi core
    
    for dist in 5 6 14 15;do
        echo $dist
        mkdir $dirName"/"$dist
        for queues in 1024;do #8 16 32 64 128 256;do
            mkdir $dirName"/"$dist"/"$queues
            for load in 200000 600000 1000000 1400000 1500000 1600000 1700000 1800000 1900000 2000000 2100000 2200000 2300000 2400000 2500000 2600000 2700000 2800000 2900000 3000000;do
                echo $load
                mkdir $dirName"/"$dist"/"$queues"/"$load
            
                ./RCC_Client -d mlx5_0 -s 20 -r 200 -g 3 -l $load 192.168.1.5 -z $dist -m $serviceTime -n $queues -t 4 -f $dirName"/"$dist"/"$queues"/"$load
                #up to 337 window size with 4 threads
                sleep 60
            done
        done
    done
    
    
    for dist in 14 15;do #5,6
        echo $dist
        mkdir $dirName"/"$dist
        for queues in 1024;do #8 16 32 64 128 256;do
            mkdir $dirName"/"$dist"/"$queues
            for load in 100000 200000 300000 400000 500000 600000 700000 800000 900000 1000000 1100000 1200000 1300000 1400000 1500000 1600000 1700000 1800000 1900000 2000000;do
                echo $load
                mkdir $dirName"/"$dist"/"$queues"/"$load
            
                ./RCC_Client -d mlx5_0 -s 20 -r 200 -g 3 -l $load 192.168.1.5 -z $dist -m $serviceTime -n $queues -t 4 -f $dirName"/"$dist"/"$queues"/"$load
                #up to 337 window size with 4 threads
                sleep 60
            done
        done
    done
done
'

# 8 cores
: '
for serviceTime in 0;do #3 is EXP service time distribution
    #multi core
    for dist in 5 6;do
        echo $dist
        mkdir $dirName"/"$dist
        for queues in 1024;do #8 16 32 64 128 256;do
            mkdir $dirName"/"$dist"/"$queues
            for load in 400000 1200000 2000000 2800000 3000000 3200000 3400000 3600000 3800000 4000000 4200000 4400000 4600000 4800000 5000000 5200000 5400000 5600000 5800000 6000000;do
                echo $load
                mkdir $dirName"/"$dist"/"$queues"/"$load
            
                ./RCC_Client -d mlx5_0 -s 20 -r 64 -g 3 -l $load 192.168.1.5 -z $dist -m $serviceTime -n $queues -t 8 -f $dirName"/"$dist"/"$queues"/"$load
                #up to 337 window size with 4 threads
                sleep 60
            done
        done
    done
    
    
    for dist in 14 15;do
        echo $dist
        mkdir $dirName"/"$dist
        for queues in 1024;do
            mkdir $dirName"/"$dist"/"$queues
            #for load in 100000 200000 300000 400000 500000 600000 700000 800000 900000 1000000 1100000 1200000 1300000 1400000 1500000 1600000 1700000 1800000 1900000 2000000;do
            for load in 400000 1200000 2000000 2800000 3000000 3200000 3400000 3600000 3800000 4000000 4200000 4400000 4600000 4800000 5000000 5200000 5400000 5600000 5800000 6000000;do
                echo $load
                mkdir $dirName"/"$dist"/"$queues"/"$load
            
                ./RCC_Client -d mlx5_0 -s 20 -r 64 -g 3 -l $load 192.168.1.5 -z $dist -m $serviceTime -n $queues -t 8 -f $dirName"/"$dist"/"$queues"/"$load
                #up to 337 window size with 4 threads
                sleep 60
            done
        done
    done   
done
'

# 12 cores
for serviceTime in 0;do #3 is EXP service time distribution
    #multi core
    : '
    for dist in 5;do
        echo $dist
        mkdir $dirName"/"$dist
        for queues in 1024;do #8 16 32 64 128 256;do
            mkdir $dirName"/"$dist"/"$queues
            #for load in 600000 1800000 3000000 4200000 4500000 4800000 5100000 5400000 5700000 6000000 6300000 6600000 6900000 7200000 7500000 7800000 8100000 8400000 8700000 9000000;do
            for load in 3000000 4200000 4500000;do
                echo $load
                mkdir $dirName"/"$dist"/"$queues"/"$load
            
                ./RCC_Client -d mlx5_0 -s 20 -r 64 -g 3 -l $load 192.168.1.5 -z $dist -m $serviceTime -n $queues -t 12 -f $dirName"/"$dist"/"$queues"/"$load
                #up to 337 window size with 4 threads
                sleep 60
            done
        done
    done
    for dist in 6;do
        echo $dist
        mkdir $dirName"/"$dist
        for queues in 1024;do #8 16 32 64 128 256;do
            mkdir $dirName"/"$dist"/"$queues
            #for load in 600000 1800000 3000000 4200000 4500000 4800000 5100000 5400000 5700000 6000000 6300000 6600000 6900000 7200000 7500000 7800000 8100000 8400000 8700000 9000000;do
            for load in 600000 1800000 3000000 4200000 4500000;do
                echo $load
                mkdir $dirName"/"$dist"/"$queues"/"$load
            
                ./RCC_Client -d mlx5_0 -s 20 -r 64 -g 3 -l $load 192.168.1.5 -z $dist -m $serviceTime -n $queues -t 12 -f $dirName"/"$dist"/"$queues"/"$load
                #up to 337 window size with 4 threads
                sleep 60
            done
        done
    done
    '
    : '
    for dist in 15;do
        echo $dist
        mkdir $dirName"/"$dist
        for queues in 1024;do
            mkdir $dirName"/"$dist"/"$queues
            for load in 100000 200000 300000 400000 500000 600000 700000 800000 900000 1000000 1100000 1200000;do
            #for load in 600000 1800000 3000000 4200000 4500000 4800000 5100000 5400000 5700000 6000000 6300000 6600000 6900000 7200000 7500000 7800000 8100000 8400000 8700000 9000000;do
            #for load in 3900000 4200000 4500000 4800000 5100000 5400000 5700000 6000000;do
                echo $load
                mkdir $dirName"/"$dist"/"$queues"/"$load
            
                ./RCC_Client -d mlx5_0 -s 20 -r 64 -g 3 -l $load 192.168.1.5 -z $dist -m $serviceTime -n $queues -t 12 -f $dirName"/"$dist"/"$queues"/"$load
                #up to 337 window size with 4 threads
                sleep 60
            done
        done
    done   
    '
    for dist in 21;do #19 20
        echo $dist
        mkdir $dirName"/"$dist
        for queues in 1000;do
            mkdir $dirName"/"$dist"/"$queues
            #for load in 100000 200000 300000 400000 500000 600000 700000 800000 900000 1000000 1100000 1200000;do
            #for load in 3900000 4200000 4500000 4800000 5100000 5400000 5700000 6000000;do
            #fixed
            #for load in 3000000 4000000 5000000 5500000 6000000 6200000 6400000 6600000 6800000 7000000 7200000 7400000 7600000 7800000 8000000;do
            #bimodal 10x - 40%
            #for load in 500000 1000000 1500000 2000000 2200000 2400000 2600000 2800000 3000000 3200000 3400000 3600000 3800000 4000000;do
            #rebalance64
            #for load in 500000 1000000 1500000 2000000 2200000 2400000 2600000 2800000 3000000 3200000 3400000 3600000 3800000 4000000 4200000 4400000 4600000;do
            #rebalance32        
            #for load in 1000000 2000000 2500000 3000000 3500000 4000000 4200000 4400000 4600000 4800000 5000000 5200000 5400000 5600000 5800000 6000000;do
            #rebalance16        
            #for load in 1000000 2000000 3000000 4000000 4200000 4400000 4600000 4800000 5000000 5200000 5400000 5600000 5800000 6000000 6200000 6400000 6600000;do
            #rebalance1       
            #for load in 1000000 2000000 3000000 4000000 5000000 5500000 6000000 6300000 6600000 6900000 7200000 7500000 7800000 8100000;do
            #bimodal 10x - 40%
            #for load in 1000000 2000000 3000000 4000000 4500000 4800000 5100000 5400000 5700000 6000000 6300000 6600000;do

            #for load in 100000 200000 300000 400000 500000 600000 700000 800000 900000 1000000;do
            #for load in 1000000 2000000 3000000 4000000 4200000 4400000 4600000 4800000;do
            #for load in 5000000 5500000 6000000 6300000 6600000 6900000 7200000 7500000 7800000 8100000;do

            #kernel0 
            #for load in 300000 600000 900000 1200000 1500000 1800000 2100000 2400000 2700000 3000000 3300000 3600000 3900000;do
            #sharedCQ
            #for load in 1000000 2000000 2500000 3000000 3200000 3400000 3600000 3800000 4000000 4200000 4400000 4600000 4800000;do
            #bitvector/aff
            #for load in 1000000 2000000 2500000 3000000 3500000 4000000 4500000 5000000 5200000 5400000 5600000 5800000 6000000;do

            #kernel1
            #for load in 1000000 1500000 2000000 2300000 2600000 2900000 3200000 3500000 3800000 4000000;do
            
            #fixed
            #for load in 1000000 2000000 3000000 4000000 5000000 5500000 6000000 6200000 6400000 6600000 6800000 7000000 7200000 7400000 7600000 7800000 8000000;do
            for load in 6800001 7000001 7200001 7400001 7600001 7800001 8000001;do

            #for load in 300000 600000 900000 1200000 1500000 1800000 ;do

            #for load in 300000 600000 900000 1200000 1500000 1800000 2100000 2400000 2700000 3000000 3300000 3600000 3900000 4200000 4500000 4800000 5100000 5400000 5700000 6000000 6300000 6600000 6900000 7200000 7500000 7800000 8100000 8400000 8700000 9000000;do
            #for load in 10000 300000 600000 900000 1200000 1800000 2400000 3000000 3600000 4200000 4800000 5400000;do
            #for load in 10000 100000 200000 300000 400000 500000 600000 700000 800000;do
            #for load in 2500000 2600000 2700000 2800000 2900000;do

            #for load in 10000 20000 30000 40000 50000 60000 70000 80000 90000 100000;do

                echo $load
                mkdir $dirName"/"$dist"/"$queues"/"$load
                #./RCC_Client -d mlx5_0 -s 20 -r 64 -g 3 -l $load 192.168.1.5 -z $dist -m $serviceTime -n $queues -t 12 -f $dirName"/"$dist"/"$queues"/"$load >> "runlog_client_"$dist"_warmup_sharedcq_12conn_kernelST_w_QD_combinedfuncs_0counters.txt"
                #./RCC_Client -d mlx5_0 -s 20 -r 64 -g 3 -l $load 192.168.1.5 -z $dist -m $serviceTime -n $queues -t 12 -f $dirName"/"$dist"/"$queues"/"$load #>> "runlog_client_"$dist"_warmup_aff4_kernelST.txt"
                
                #./RCC_Client -d mlx5_0 -s 20 -r 64 -g 3 -l $load 192.168.1.5 -z $dist -m 4 -n $queues -t 12 -f $dirName"/"$dist"/"$queues"/"$load -x 10 -p 40
                #./RCC_Client -d mlx5_0 -s 20 -r 64 -g 3 -l $load 192.168.1.5 -z $dist -m 4 -n $queues -t 12 -f $dirName"/"$dist"/"$queues"/"$load -x 5 -p 10

                ./RCC_Client -d mlx5_0 -s 20 -r 64 -g 3 -l $load 192.168.1.5 -z $dist -m 4 -n $queues -t 12 -f $dirName"/"$dist"/"$queues"/"$load -x 1 -p 100
                #./RCC_Client -d mlx5_0 -s 20 -r 64 -g 3 -l $load 192.168.1.5 -z $dist -m 4 -n $queues -t 12 -f $dirName"/"$dist"/"$queues"/"$load -x 10 -p 11
                #./RCC_Client -d mlx5_0 -s 20 -r 64 -g 3 -l $load 192.168.1.5 -z $dist -m 4 -n $queues -t 12 -f $dirName"/"$dist"/"$queues"/"$load -x 10 -p 22
                #./RCC_Client -d mlx5_0 -s 20 -r 64 -g 3 -l $load 192.168.1.5 -z $dist -m 4 -n $queues -t 12 -f $dirName"/"$dist"/"$queues"/"$load -x 10 -p 33
    
                #up to 337 window size with 4 threads
                sleep 40
            done
        done
    done   
done