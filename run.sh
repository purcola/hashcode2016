#!/bin/sh
# author: Pablo Urcola

# compile
mkdir -p build
cd build
cmake .. && make
cd .. 

#create output dir (just in case)
mkdir -p output

# run all the inputs 
for file in $(find dataset -name "*.in")
do
	name=$(basename $file .in)
	./build/hc dataset/${name}.in > output/${name}.out 2> /dev/null
done

