#!/bin/bash

version="3.35"
root="/home/grosa/dev/source"

read -n 1 -s -r -p "Remember to copy the original node files as backup. Press any key to continue..."

echo "Copying node files..."
cp -f "node.cc" "${root}/ns-${version}/src/network/model/node.cc" 
cp -f "node.h" "${root}/ns-${version}/src/network/model/node.h" 

echo "Copying zash files..."
cp -avr "zash" "${root}/ns-${version}/src"

echo "Copying simulation zash file..."
cp -f "zash-simulation.cc" "${root}/ns-${version}/scratch/zash-simulation.cc"

echo "Copying run zash file..."
cp -f "run.sh" "${root}/ns-${version}/run.sh"

echo "Building ns-3..."
cd $root
./build.py