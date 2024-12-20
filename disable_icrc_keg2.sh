#!/bin/bash

sudo /etc/init.d/openibd restart
sudo mst start

# Disable ICRC and RoCE IP header checksum checks in ConnectX hardware
# .rxt.checks.rxt_checks_packet_checks_wrapper.g_check_mask.packet_checks_action0.bits_ng.bad_icrc
sudo mcra /dev/mst/mt4117_pciconf0 0x5363c.12:1 0
# .rxt.checks.rxt_checks_packet_checks_wrapper.g_check_mask.packet_checks_action1.bits_ng.bad_icrc
sudo mcra /dev/mst/mt4117_pciconf0 0x5367c.12:1 0
# .rxt.checks.rxt_checks_packet_checks_wrapper.g_check_mask.packet_checks_action0.bits_ng.bad_roce_l3_header_corrupted
sudo mcra /dev/mst/mt4117_pciconf0 0x53628.3:1 0
# .rxt.checks.rxt_checks_packet_checks_wrapper.g_check_mask.packet_checks_action1.bits_ng.bad_roce_l3_header_corrupted
sudo mcra /dev/mst/mt4117_pciconf0 0x53668.3:1 0

sudo iptables -F; sudo iptables -t mangle -F
sudo iptables -D INPUT -m conntrack --ctstate INVALID -j DROP
sudo ethtool --set-priv-flags enp24s0 sniffer off