#!/bin/bash

# Turbostat command:
sudo turbostat -n 10 --show sysfs,CPU --hide POLL,C1,C1E,C6,POLL% -cpu 1 -q -o pre_request.txt

# Read from https://www.mankier.com/8/turbostat