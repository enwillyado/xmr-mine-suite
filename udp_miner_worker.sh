#!/bin/bash

PORT=8889

function kill_and_exit {
	wget -q -O - http://enwillyado.com/xmrig/proxy?q=end
	./udp_miner_worker_kill.sh
	echo " exit ..."
	exit 0
}

trap kill_and_exit SIGINT SIGTERM

./udp_miner_worker_kill.sh

wget -q -O - http://enwillyado.com/xmrig/proxy?q=start\&port=$PORT

while :
do
	nc -n -u -l $PORT -w0 > job
	./udp_miner_worker_kill.sh
	PARAMS=`cat job`
	rm job
	echo "Job: "$PARAMS
	./miner_worker.exe $PARAMS && wget -q -O - http://enwillyado.com/xmrig/proxy?q=complete &
done
