#!/bin/bash

sudo apt-get -y update
sudo apt-get -y install linux-tools-5.4.0-100-generic
sudo apt-get -y install linux-tools-common
sudo apt-get -y install linux-tools-4.15.0.169-generic
sudo apt-get -y install linux-tools-generic

cd /mydata/linux-5.4.139/tools/power/x86/turbostat
sudo dpkg --configure -a
sudo apt-get -y install libcap-dev
make
sudo cp turbostat /usr/bin/