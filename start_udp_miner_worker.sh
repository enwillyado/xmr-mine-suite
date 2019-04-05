#!/bin/bash
source vars.sh

function kill_and_exit {
	wget -q -O - http://$SERVER_HOST:$SERVER_PORT/xmrig/proxy?q=end
	./killall_udp_miner_workers.sh
	echo " exit ..."
	exit 0
}

trap kill_and_exit SIGINT SIGTERM

./killall_udp_miner_workers.sh

wget -q -O - http://$SERVER_HOST:$SERVER_PORT/xmrig/proxy?q=start\&port=$UDP_PORT

while :
do
	nc -n -u -l $UDP_PORT -w0 > job
	./killall_udp_miner_workers.sh
	PARAMS=`cat job`
	rm job
	echo "Job: "$PARAMS
	./miner_worker.exe $PARAMS && wget -q -O - http://$SERVER_HOST:$SERVER_PORT/xmrig/proxy?q=complete &
done
