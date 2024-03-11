#!/bin/bash

setup() {
    bash setup.sh
    # bash install_socwatch.sh
}

stop_processes() {
    pkill "server"
    pkill "server2"
    ssh node1 "pkill 'client_both parameters.txt'"
    echo "Processes killed"
}


start_processes() {
    if [ -z "$1" ]; then
        echo "Usage: $0 start <folder_name>"
        exit 1
    fi

    folder_name=$1

    if [ -d "$folder_name" ]; then
        echo "Error: Folder '$folder_name' already exists. Exiting."
        exit 1
    fi

    stop_processes
    make

    # Set cpu frequency to 2200MHz
    # for node in node1 node0
    # do
    # ssh -A $node "sudo cpupower frequency-set -g userspace; sudo cpupower frequency-set -f 2200MHz"
    # done
    
    # Create the specified folder
    mkdir -p "$folder_name"
    
    cp client_both.c $folder_name/
    cp server.c $folder_name/
    cp server2.c $folder_name/
    cp parameters.txt $folder_name/
    scp parameters.txt node1:~/
    scp client_both node1:~/
    scp poisson.py node1:~/
    taskset -c 1 ./server &
    taskset -c 1 ./server2 &
    # Run socwatch
    sudo /opt/intel/oneapi/vtune/2024.0/socwatch/x64/socwatch -f cpu-cstate -m -r int -o $folder_name/Socwatch > /dev/null 2> /dev/null &

    # Capture the first output in interrupts_first.txt
    cat /proc/interrupts | grep "i40e-enp94s0f1-TxRx-1" > interrupts_first.txt
    
    # Run the command and use turbostat as well
    sudo turbostat --show sysfs,CPU,CPU%c1,CPU%c6 --hide POLL,C1,C1E,C6,POLL% -cpu 1 -q -o $folder_name/turbostat_output.txt ssh -A cseas002@node1 "./client_both parameters.txt" 2> $folder_name/times.txt >> $folder_name/output.txt
    # ssh -A cseas002@node1 "./client_both parameters.txt" 2> $folder_name/times.txt >> $folder_name/output.txt

    # Capture the second output in interrupts_second.txt
    cat /proc/interrupts | grep "i40e-enp94s0f1-TxRx-1" > interrupts_second.txt

    # Send SIGINT to processes with names containing "socwatch"
    pkill -SIGINT -f "socwatch"

    # Use diff to compare the two outputs and save the differences in diff.txt
    diff interrupts_first.txt interrupts_second.txt > $folder_name/interrupts.txt
    rm interrupts_first.txt
    rm interrupts_second.txt
    
    stop_processes
}

case "$1" in
    setup)
        setup
        ;;
    start)
        start_processes "$2"
        ;;
    stop)
        stop_processes
        ;;
    *)
        echo "Usage: $0 {setup|start|stop}"
        exit 1
        ;;
esac

exit 0
