#!/bin/bash

for node in node1 node0
do
ssh -A $node "sudo sed -i '\/GRUB_CMDLINE_LINUX=/ s/\"$/ intel_pstate=disable\"/' /etc/default/grub; sudo update-grub2; sudo reboot"
done