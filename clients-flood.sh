#!/usr/bin/env bash
CMD="./client"

if [ "$#" -ne 2 ];then
    printf "usage:\n"
    printf "\t$0 <address> <port>\n"
    exit 1
fi

for i in `seq 1 10`;
do
    $CMD $1 $2 &
    netstat -anp | grep "client" > 2.txt
done

