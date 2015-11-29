#!/usr/bin/env bash

if [ "$#" -ne 2 ]; then
    echo "Usage:"
    printf "\t $0 <number-of-lines> <output-file>\n"
    exit 1
fi

LINES=$1
OUTPUT=$2

if [ -f $OUTPUT ]; then
    rm $OUTPUT
fi

for ((a=0; a< $LINES; a++))
do
    RNDSTRING=$(cat /dev/urandom | tr -dc 'a-zA-Z0-9' | fold -w 32 | head -n 1)
    echo $RNDSTRING >> $OUTPUT
done

echo "$OUTPUT created with $LINES lines."

