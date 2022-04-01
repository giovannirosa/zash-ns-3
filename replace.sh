#!/bin/bash

version="3.35"
root="/home/grosa/Dev/ns-allinone-3.35"

# read -n 1 -s -r -p "Remember to copy the original node files as backup. Press any key to continue..."

# echo "Copying node files..."
# cp -f "node.cc" "${root}/ns-${version}/src/network/model/node.cc" 
# cp -f "node.h" "${root}/ns-${version}/src/network/model/node.h" 

echo "Copying zash files..."
cp -avr "zash" "${root}/ns-${version}/src"

echo "Copying data files..."
cp -avr "data" "${root}/ns-${version}"

echo "Copying simulation zash file..."
cp -f "zash-simulator.cc" "${root}/ns-${version}/scratch/zash_simulator.cc"

echo "Copying run zash file..."
cp -f "run.sh" "${root}/ns-${version}/run.sh"

echo "Building ns-3..."
cd $root
./build.py