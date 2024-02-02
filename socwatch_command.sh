sudo /opt/intel/oneapi/vtune/2024.0/socwatch/x64/socwatch -t {{ MONITOR_TIME }} -f cpu-cstate -m -r int -o {{ OUTPUT_FILE }}

sudo /opt/intel/oneapi/vtune/2024.0/socwatch/x64/socwatch -f cpu-cstate -m -r int -o OUTPUT_FILE &  # Run in background
socwatch_pid=$!  # Get the PID of socwatch