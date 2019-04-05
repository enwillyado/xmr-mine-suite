#!/bin/bash

PORT=8889

wget -q -O - http://enwillyado.com/xmrig/start?port=$PORT

while :
do
	nc -n -u -l $PORT -w0 | tee job
	./udp_miner_worker_kill.sh
	./miner_worker.exe `cat job` &
done
