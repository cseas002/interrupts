# To disable smt, run
# echo "off" | sudo tee /sys/devices/system/cpu/smt/control

# Disable c states
for i in {1..3}
do 
echo 0 | sudo tee /sys/devices/system/cpu/cpu*/cpuidle/state$i/disable
done