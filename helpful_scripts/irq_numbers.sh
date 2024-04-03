#!/bin/bash

# Function to convert CPU bitmask to core numbers
cpu_mask_to_cores() {
    local mask=$1
    local cores=""
    local core_num=0
    local bit_pos=0
    
    # Loop through each bit in the bitmask
    while [ $mask -gt 0 ]; do
        # Check if the least significant bit is set
        if [ $((mask & 1)) -eq 1 ]; then
            # If set, add the corresponding core number to the list
            cores="$cores $core_num"
        fi
        
        # Shift the bitmask to the right by one position
        mask=$((mask >> 1))
        # Increment the core number
        core_num=$((core_num + 1))
    done
    
    echo "$cores"
}

# Get the interrupt breakdown
interrupts=$(cat /proc/interrupts)

# Find the lines corresponding to TxRx-x interrupts and their associated cores
txrx_lines=$(echo "$interrupts" | grep -E 'TxRx-[0-9]+')

# Iterate over each TxRx line and set its affinity
echo "Setting IRQ affinities for TxRx-x interrupts:"
echo "$txrx_lines" | while IFS= read -r line; do
    # Extract IRQ number and associated CPU
    irq=$(echo "$line" | awk '{print $1}' | sed 's/://')
    cpu=$(echo "$line" | grep "i40e" | awk -F '-' '{print $NF}')
    # Validate IRQ number and ensure it's not 0
    if [ "$irq" != "0" ] && [ -n "$cpu" ]; then
        # Check if the smp_affinity file exists
        if [ -e "/proc/irq/$irq/smp_affinity" ]; then
            # Set the affinity for TxRx-x IRQ to the corresponding core x
            echo "Setting IRQ $irq to core $cpu"
            affinity=$((1 << cpu))
            # echo $affinity
            # echo "2^$cpu"
            printf "%x\n" "$affinity"
            echo "$affinity" | tee "/proc/irq/$irq/smp_affinity" >/dev/null
        else
            echo "Error: /proc/irq/$irq/smp_affinity does not exist. Skipping IRQ $irq."
        fi
    fi
done

exit 0
