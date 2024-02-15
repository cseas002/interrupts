#!/bin/bash

# Check if there is exactly one parameter
if [ "$#" -ne 1 ]; then
    echo "Usage: $0 <input_folder>"
    exit 1
fi

# Read the first parameter and save it to a variable
input_folder=$1

for interval in 10 50 100 300
do
    for pre_req in true false 
    do 
        python3 create_graphs.py "$input_folder/Pre-req=${pre_req}/Pre-req-interval=${interval}"
    done
done
