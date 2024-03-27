# #!/bin/bash

# for node in node2 node1 node0
# do
# ssh -A $node 'cd /tmp;
# wget https://apt.repos.intel.com/intel-gpg-keys/GPG-PUB-KEY-INTEL-SW-PRODUCTS.PUB;
# sudo apt-key add GPG-PUB-KEY-INTEL-SW-PRODUCTS.PUB;
# rm GPG-PUB-KEY-INTEL-SW-PRODUCTS.PUB;
# echo "deb https://apt.repos.intel.com/oneapi all main" | sudo tee /etc/apt/sources.list.d/oneAPI.list;
# sudo apt update;
# sudo apt install intel-oneapi-vtune -y;'
# done

# # SocWatch command:
# # $sudo /opt/intel/oneapi/vtune/2024.0/socwatch/x64/socwatch -s 60 (DELAY) -t 5(MONITOR TIME) -f cpu-cstate -m -r int -o test.txt (OUTPUT FILE)

# # NOTE THAT IT CHANGES BASED ON THE VERSION (2024.0)