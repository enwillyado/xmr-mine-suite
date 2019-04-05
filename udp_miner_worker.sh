#!/bin/bash
source vars.sh

function kill_and_exit {
	wget -q -O - http://$SERVER_HOST:$SERVER_PORT/xmrig/proxy?q=end
	./udp_miner_worker_kill.sh
	echo " exit ..."
	exit 0
}

trap kill_and_exit SIGINT SIGTERM

./udp_miner_worker_kill.sh

wget -q -O - http://$SERVER_HOST:$SERVER_PORT/xmrig/proxy?q=start\&port=$UDP_PORT

while :
do
	nc -n -u -l $UDP_PORT -w0 > job
	./udp_miner_worker_kill.sh
	PARAMS=`cat job`
	rm job
	echo "Job: "$PARAMS
	./miner_worker.exe $PARAMS && wget -q -O - http://$SERVER_HOST:$SERVER_PORT/xmrig/proxy?q=complete &
done
