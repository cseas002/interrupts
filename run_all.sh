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

for sleep_val in 100 1000 100000 1000000; do
    for pre_request in true false; do
        for pre_request_interval in 10 50 100 300; do
            parent_folder_name="${save_folder_name}/Pre-req=${pre_request}/Pre-req-interval=${pre_request_interval}"
            # mkdir -p $parent_folder_name
            for state in enable disable; do
                for distribution_type in fixed exponential; do
                    folder_name="${parent_folder_name}/State=${state}/${distribution_type}/Sleep=${sleep_val}"

                    # Update the parameters file with the current sleep_val, pre_request, pre_request_interval, and distribution_type
                    sed -i "s/sleep_usecs .*/sleep_usecs $sleep_val/" parameters.txt
                    sed -i "s/pre_request .*/pre_request $pre_request/" parameters.txt
                    sed -i "s/pre_request_interval .*/pre_request_interval $pre_request_interval/" parameters.txt
                    sed -i "s/distribution .*/distribution $distribution_type/" parameters.txt
                    bash ${state}_cstates.sh  # Change the c states

                    # mkdir -p $folder_name
                    
                    # Run the main script
                    bash run.sh start "$folder_name"
                done
            done
        done
    done
done