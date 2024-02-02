#!/bin/bash

# cd
# git clone -y git@github.com:cseas002/interrupts.git
# cd interrupts
ssh -A node1 "sudo apt-get -y install libgsl-dev"
ssh -A node1 "sudo apt -y install python3-pip"
ssh -A node1 "pip install numpy"
pip install scipy
chmod a+x grow-rootfs.sh
sudo env RESIZEROOT=200 ./grow-rootfs.sh
wget -O- https://apt.repos.intel.com/intel-gpg-keys/GPG-PUB-KEY-INTEL-SW-PRODUCTS.PUB | gpg --dearmor | sudo tee /usr/share/keyrings/oneapi-archive-keyring.gpg > /dev/null
echo "deb [signed-by=/usr/share/keyrings/oneapi-archive-keyring.gpg] https://apt.repos.intel.com/oneapi all main" | sudo tee /etc/apt/sources.list.d/oneAPI.list
sudo apt update
sudo apt install -y intel-basekit
sudo apt install -y linux-tools-common linux-tools-$(uname -r)
make

# To disable smt, run
# echo "off" | sudo tee /sys/devices/system/cpu/smt/control