#!/bin/bash

function kill_and_exit {
	./udp_miner_worker_kill.sh
	echo " exit ..."
	exit 0
}

trap kill_and_exit SIGINT SIGTERM

./udp_miner_worker_kill.sh
PORT=8889

wget -q -O - http://enwillyado.com/xmrig/start?port=$PORT

while :
do
	nc -n -u -l $PORT -w0 > job
	./udp_miner_worker_kill.sh
	PARAMS=`cat job`
	rm job
	echo "Job: "$PARAMS
	./miner_worker.exe $PARAMS &
done
