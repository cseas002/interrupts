#!/bin/bash

sudo apt-get update
sudo apt-get install -y wireshark
sudo usermod -aG wireshark $USER
sudo apt install -y tshark

# COMMAND:
# sudo tshark -i enp94s0f0 -Y "ip.dst == 10.10.1.1 and tcp.dstport == 8080"  # (enp94s0f0 is the interface)

# To show the timestamp (first column) 
# start_time=$(date +%s.%N); sudo tshark -i enp94s0f0 -Y "tcp.dstport == 8081 or tcp.srcport == 8081" -t r | grep "Len=17" | awk -v st="$start_time" '{print $1 + st, $0'} &


# To stop:
# start_time=$(date +%s.%N); sudo tshark -i enp94s0f0 -Y "tcp.dstport == 8081 or tcp.srcport == 8081" -t r | grep "Len=17" | awk -v st="$start_time" 'BEGIN {count=1} {count++} {print $1 + st, $0} count==2008 {exit}' > ../output.txt