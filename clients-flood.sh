#!/usr/bin/env bash
CMD="./client"

if [ "$#" -ne 3 ];then
    printf "usage:\n"
    printf "\t$0 <address> <port> <backlog-sz>\n"
    exit 1
fi

for i in `seq 1 10`;
do
    $CMD $1 $2 &
done

#netstat -anp | grep "./client" > "$3".txt
netstat -anp | grep ":1234" > server.txt

