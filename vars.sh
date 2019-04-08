#!/bin/bash

#DEFAULT_HOST="mine.xmrpool.net"
#DEFAULT_PORT=80
#DEFAULT_USER="433hhduFBtwVXtQiTTTeqyZsB36XaBLJB6bcQfnqqMs5RJitdpi8xBN21hWiEfuPp2hytmf1cshgK5Grgo6QUvLZCP2QSMi"
#DEFAULT_PASS="x"

if [[ ! -v WORKERS_HOST ]]
then
	WORKERS_HOST=enwillyado.com
fi

if [[ ! -v WORKERS_PORT ]]
then
	WORKERS_PORT=8888
fi

if [[ ! -v WORKER_UDP_PORT ]]
then
	WORKER_UDP_PORT=8889
fi
