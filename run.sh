#!/bin/bash

# export 'NS_LOG=StatusOn=info:StealthOnOff=info:StealthPacketSink=info:CenarioStealth_v1=info:Node=info'

echo "Executing Zash simulation..."
# ./waf --run "scratch/zash-simulator" > log.txt 2>&1
./waf --run "scratch/zash-simulator"