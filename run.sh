#!/bin/bash

if [ -z "$1" ] || [ -z "$2" ] || [ -z "$3" ]; then
    echo "Please enter the mode, the number of rounds, number of attacks, number of alterations, number of users, datarate, and latency. e.g.: ./run.sh H 12 10 5 5 100Mbps 1ms"
    exit 1
fi

if [ $1 != "S" ] && [ $1 != "H" ]; then
    echo "The mode can be only S or H"
    exit 1
fi

if [ $3 -lt 0 ]; then
    echo "The number of attacks must be equal or greater than 0"
    exit 1
fi

if [ ! -z "$4" ] && [ $4 -lt 0 ]; then
    echo "The number of alterations must be equal or greater than 0"
    exit 1
fi

ALTERATIONS="5"
if [ ! -z "$4" ]; then
    ALTERATIONS=$4
fi

USERS="5"
if [ ! -z "$5" ]; then
    USERS=$5
fi

DATARATE="100Mbps"
if [ ! -z "$6" ]; then
    DATARATE=$6
fi

LATENCY="1ms"
if [ ! -z "$7" ]; then
    LATENCY=$7
fi

MODE="H"
if [[ $1 = "S" ]]; then
    echo "Configuring for soft execution mode..."
    MODE="S"
else
    echo "Configuring for hard execution mode..."
    MODE="H"
fi

# echo "Executing Zash simulation..."
# time ./ns3 run "scratch/zash-simulator --mode=$MODE"

for (( counter=1; counter <= $2; counter++ ))
do
    echo "Executing $MODE Mode ($counter)..."
    time ./ns3 run "scratch/zash-simulator --mode=$MODE --attacks=$3 --alterations=$ALTERATIONS --usersNumber=$USERS --dataRate=$DATARATE --latency=$LATENCY"
done
