#!/bin/bash

# cd
# git clone -y git@github.com:cseas002/interrupts.git
# cd interrupts
for node in node1 node0
do
    ssh -A $node "sudo apt update && sudo apt-get install lmbench; sudo apt-get -y install libgsl-dev; sudo apt -y install python3-pip; sudo apt-get -y install linux-tools-common linux-tools-\$(uname -r)"
done
# ssh -A node1 "sudo apt-get -y install libgsl-dev"
ssh -A node1 "pip install numpy"
pip install scipy
pip install matplotlib
chmod a+x grow-rootfs.sh

# Install SocWatch
sudo env RESIZEROOT=200 ./grow-rootfs.sh
wget -O- https://apt.repos.intel.com/intel-gpg-keys/GPG-PUB-KEY-INTEL-SW-PRODUCTS.PUB | gpg --dearmor | sudo tee /usr/share/keyrings/oneapi-archive-keyring.gpg > /dev/null
echo "deb [signed-by=/usr/share/keyrings/oneapi-archive-keyring.gpg] https://apt.repos.intel.com/oneapi all main" | sudo tee /etc/apt/sources.list.d/oneAPI.list
sudo apt install -y intel-basekit
sudo apt install -y linux-tools-common linux-tools-$(uname -r)
make

# bash fixed_cpu.sh

# To disable smt, run
# echo "off" | sudo tee /sys/devices/system/cpu/smt/control