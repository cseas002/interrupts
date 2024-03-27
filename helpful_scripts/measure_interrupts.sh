#!/bin/bash

cat /proc/interrupts > interrupts1; 
sleep 10;
cat /proc/interrupts > interrupts2;
python diff_interrupts.py interrupts1 interrupts2 > diff