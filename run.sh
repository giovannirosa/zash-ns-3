#!/bin/bash

if [ -z "$1" ]
  then
    echo "Please enter the number of rounds. e.g.: ./run.sh 1"
    exit 1
fi

# export 'NS_LOG=StatusOn=info:StealthOnOff=info:StealthPacketSink=info:CenarioStealth_v1=info:Node=info'

move_log() {
    while IFS= read -r line; do
        if [[ $line =~ .*stealth_traces/.* ]]; then
            echo "Moving log file..."
            SUBSTRING=$(echo $line | cut -d'/' -f 2)
            mv $1 stealth_traces/$SUBSTRING/stealth_log_$2_$SUBSTRING.txt
            break;
        fi
    done <"$1"
}

for (( counter=1; counter <= $1; counter++ ))
do
    echo "Executing Equal Mode ($counter)..."
    ./waf --run "scratch/stealth-simulation_4 --fixNode=6 --runMode=E" > logEqual$counter.txt 2>&1
    move_log logEqual$counter.txt equal
done
for (( counter=1; counter <= $1; counter++ ))
do
   echo "Executing After Mode ($counter)..."
   ./waf --run "scratch/stealth-simulation_4 --fixNode=6 --runMode=A" > logAfter$counter.txt 2>&1
   move_log logAfter$counter.txt after
done
for (( counter=1; counter <= $1; counter++ ))
do
   echo "Executing Before Mode ($counter)..."
   ./waf --run "scratch/stealth-simulation_4 --fixNode=6 --runMode=B" > logBefore$counter.txt 2>&1
   move_log logBefore$counter.txt before
done