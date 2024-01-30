#!/bin/bash

for sleep_val in 1000 10000 100000 1000000; do
    folder_name="poisson_states_disabled_${sleep_val}"

    # Update the parameters file with the current sleep_val
    sed -i "s/sleep_usecs .*/sleep_usecs $sleep_val/" parameters.txt

    # Run the main script
    bash run.sh start "$folder_name"
done
