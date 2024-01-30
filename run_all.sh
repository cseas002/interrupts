#!/bin/bash

if [ "$#" -ne 1 ]; then
        echo "Usage: $0 <folder_name>"
        exit 1
fi

save_folder_name=$1

if [ -d "$save_folder_name" ]; then
    echo "Error: Folder '$save_folder_name' already exists. Exiting."
    exit 1
fi

mkdir $save_folder_name

for sleep_val in 1000 10000 100000 1000000; do
    for pre_request in true false; do
        parent_folder_name="${save_folder_name}/Pre-req=${pre_request}"
        mkdir -p $parent_folder_name
        for state in enable disable; do
            # Make the folder State_${state}
            folder_name=${parent_folder_name}/State=${state}
            mkdir -p $folder_name
            folder_name="${folder_name}/Poisson_${sleep_val}"

            # Update the parameters file with the current sleep_val and pre_request
            sed -i "s/sleep_usecs .*/sleep_usecs $sleep_val/" parameters.txt
            sed -i "s/pre_request .*/pre_request $pre_request/" parameters.txt
            bash ${state}_cstates.sh  # Change the c states

            # Run the main script
            bash run.sh start "$folder_name"
        done
    done
done
