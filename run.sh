#!/bin/bash

setup() {
    bash setup.sh
}

stop_processes() {
    pkill "server"
    pkill "server2"
    ssh node1 "pkill 'client_both parameters.txt'"
    echo "Processes killed"
}


start_processes() {
    if [ "$#" -ne 1 ]; then
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
    # Create the specified folder
    mkdir "$folder_name"
    cp client_both.c $folder_name/
    cp server.c $folder_name/
    cp server2.c $folder_name/
    cp parameters.txt $folder_name/
    scp parameters.txt node1:~/
    scp client_both node1:~/
    taskset -c 1 ./server &
    taskset -c 1 ./server2 &
    # Run the command and use turbostat as well
    sudo turbostat --show sysfs,CPU --hide POLL,C1,C1E,C6,POLL% -cpu 1 -q -o $folder_name/turbostat_output.txt ssh -A cseas002@node1 "./client_both parameters.txt" >> $folder_name/output.txt
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
