## Installing SoC Watch

### Resize the root partition to ensure you have enough free space:

chmod a+x grow-rootfs.sh

sudo env RESIZEROOT=200 ./grow-rootfs.sh

### Install oneAPI base kit using the APT package manager

wget -O- https://apt.repos.intel.com/intel-gpg-keys/GPG-PUB-KEY-INTEL-SW-PRODUCTS.PUB \ | gpg --dearmor | sudo tee /usr/share/keyrings/oneapi-archive-keyring.gpg > /dev/null

echo "deb [signed-by=/usr/share/keyrings/oneapi-archive-keyring.gpg] https://apt.repos.intel.com/oneapi all main" | sudo tee /etc/apt/sources.list.d/oneAPI.list

sudo apt update

sudo apt install intel-basekit

## Installing Turbostat

sudo apt install linux-tools-common linux-tools-$(uname -r)

## Enabling C6

echo "0" | sudo tee /sys/devices/system/cpu/cpu*/cpuidle/state3/disable

## Compile

make

## Alternative, just run setup.sh 

bash setup.sh

## Experiment
bash run.sh start <folder name>
e.g.
bash run.start MY_RUN

Everything will be saved in the folder MY_RUN
## Experiment (old)

On node0:

./server

On node1:

To send messages to server running on node0 (10.10.1.1) every 100ms (100000us) 

./client 10.10.1.1 100000

On node0:

To measure interrupts within a 10s time window:

cat /proc/interrupts > interrupts1; sleep 10; cat /proc/interrupts > interrupts2;
python diff_interrupts.py interrupts1 interrupts2

To capture interrupts causing a C-state transition with SoC Watch:

sudo /opt/intel/oneapi/vtune/2024.0/socwatch/x64/socwatch -f cpu-cstate -m -r int -o temp -t 10
