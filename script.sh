#!/bin/bash
max=120
ip=localhost
filename=dijkstra.html
port=51000


for (( i=1; i<=max; ++i )) do 
    echo "Request $i of $max"
    curl -s "$ip:$port/$filename" > /dev/null
	sleep 0.25
done
