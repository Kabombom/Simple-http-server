#!/bin/bash
max=200
ip=localhost
filename=dijkstra.html
filename2=cgi-bin/test.html.gz
port=51000


for (( i=1; i<=max; ++i )) do 
    echo "Request $i of $max"
    curl -s "$ip:$port/$filename" > /dev/null
	#sleep 0.25
done
curl -s "$ip:$port/$filename2" > /dev/null
