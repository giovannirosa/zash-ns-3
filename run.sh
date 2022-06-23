#!/bin/bash

move_log() {
    while IFS= read -r line; do
        echo $line
        if [[ $line =~ .*zash_traces/.* ]]; then
            echo "Moving log file..."
            SUBSTRING=$(echo $line | cut -d'/' -f 2)
            mv $1 zash_traces/$SUBSTRING/zash_log_$SUBSTRING.txt
            break;
        fi
    done <"$1"
}

echo "Executing Zash simulation..."
./waf --run "scratch/zash-simulator" > log.txt 2>&1
move_log log.txt
# ./waf --run zash-simulator

    
