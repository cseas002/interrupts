#!/bin/bash

sudo apt-get update
sudo apt-get install -y wireshark
sudo usermod -aG wireshark $USER
sudo apt install -y tshark

# COMMAND:
# sudo tshark -i enp94s0f0 -Y "ip.dst == 10.10.1.1 and tcp.dstport == 8080"  # (enp94s0f0 is the interface)