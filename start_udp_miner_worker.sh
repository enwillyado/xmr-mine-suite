#!/bin/bash
#
#	WORKERS_HOST=x.x.x.x WORKERS_PORT=8888 WORKER_UDP_PORT=8889 ./start_udp_miner_worker.sh
#
source vars.sh

function kill_and_exit {
	wget -q -O - http://$WORKERS_HOST:$WORKERS_PORT/xmrig/proxy?\&q=end\&port=$WORKER_UDP_PORT\& &
	./killall_udp_miner_workers.sh
	echo " exit ..."
	exit 0
}

trap kill_and_exit SIGINT SIGTERM

./killall_udp_miner_workers.sh

wget -q -O - http://$WORKERS_HOST:$WORKERS_PORT/xmrig/proxy?\&q=start\&port=$WORKER_UDP_PORT\& &

while :
do
	nc -n -u -l $WORKER_UDP_PORT -w0 > job
	./killall_udp_miner_workers.sh
	PARAMS=`cat job`
	rm job
	echo "Job: "$PARAMS
	./miner_worker.exe $PARAMS && wget -q -O - http://$WORKERS_HOST:$WORKERS_PORT/xmrig/proxy?\&q=complete\&port=$WORKER_UDP_PORT\& &
done
