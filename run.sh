#!/bin/bash

echo "Executing Zash simulation..."
./waf --run "scratch/zash-simulator" > log.txt 2>&1
# ./waf --run zash-simulator

    
