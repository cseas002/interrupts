#!/bin/bash

# Check if there is exactly one parameter
if [ "$#" -ne 1 ]; then
    echo "Usage: $0 <input_folder>"
    exit 1
fi

# Read the first parameter and save it to a variable
input_folder=$1

for state in enable disable
do
    for distribution in exponential fixed 
    do 
        python3 create_graphs.py "$input_folder/State=${state}/${distribution}"
    done
done
