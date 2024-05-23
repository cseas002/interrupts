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
    # cp server2.c $folder_name/
    cp parameters.txt $folder_name/
    scp parameters.txt node1:~/
    scp client_both.c node1:~/
    ssh node1 "gcc client_both.c -o client_both -lm"
    scp poisson.py node1:~/
    taskset -c 1 ./server 8080 > $folder_name/pre_requests_server.txt &
    taskset -c 1 ./server 8081 > $folder_name/requests_server.txt &
    # Run socwatch
    # sudo /opt/intel/oneapi/vtune/2024.1/socwatch/x64/socwatch -f cpu-cstate -m -r int -o $folder_name/Socwatch > /dev/null 2> /dev/null &

    # Capture the first output in interrupts_first.txt
    cat /proc/interrupts | grep "i40e" | grep "TxRx-1" | grep -v "TxRx-1[0-9]" > interrupts_first.txt
    
    # Get wireshark details
    # start_time=$(date +%s.%N) 
    # sudo tshark -i enp94s0f0 -Y "tcp.dstport == 8081 or tcp.srcport == 8081" -t r | awk -v st="$start_time" '{printf "%.9f %s\n", $1 + st, $0}' > "$folder_name"/wireshark_all.txt &

    # Run the command and use turbostat as well
    sudo turbostat --show sysfs,CPU,CPU%c1,CPU%c6 --hide POLL,C1,C1E,C6,POLL% -cpu 1 -q -o $folder_name/turbostat_output.txt ssh -A cseas002@node1 "./client_both parameters.txt" 2> $folder_name/times.txt >> $folder_name/output.txt
    # ssh -A cseas002@node1 "./client_both parameters.txt" 2> $folder_name/times.txt >> $folder_name/output.txt

    # Capture the second output in interrupts_second.txt
    cat /proc/interrupts | grep "i40e" | grep "TxRx-1" | grep -v "TxRx-1[0-9]" > interrupts_second.txt

    # Send SIGINT to processes with names containing "socwatch"
    # sudo pkill -SIGINT -f "socwatch"

    # Use diff to compare the two outputs and save the differences in diff.txt
    diff interrupts_first.txt interrupts_second.txt > $folder_name/interrupts.txt

    rm interrupts_first.txt
    rm interrupts_second.txt

    sleep 2
    # Kill wireshark
    # sudo pkill -SIGINT -f tshark
    sleep 2
    # cat $folder_name/wireshark_all.txt | grep "Len=17" > $folder_name/wireshark.txt    

    cat $folder_name/times.txt | grep -A 1 "Sent" | grep -v -- "^--$" > $folder_name/sent_get_client.txt
    # python3 correlation_analysis.py "$folder_name/wireshark.txt" "$folder_name/sent_get_client.txt" "$folder_name/requests_server.txt" "$folder_name"
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
