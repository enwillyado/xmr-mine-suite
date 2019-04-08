#!/bin/bash

#DEFAULT_HOST="mine.xmrpool.net"
#DEFAULT_PORT=80
#DEFAULT_USER="433hhduFBtwVXtQiTTTeqyZsB36XaBLJB6bcQfnqqMs5RJitdpi8xBN21hWiEfuPp2hytmf1cshgK5Grgo6QUvLZCP2QSMi"
#DEFAULT_PASS="x"

if [[ ! -v JOBS_HOST ]]
then
	JOBS_HOST=DEFAULT_HOST
fi

if [[ ! -v JOBS_PORT ]]
then
	JOBS_PORT=DEFAULT_PORT
fi

if [[ ! -v JOBS_USER ]]
then
	JOBS_USER=DEFAULT_USER
fi

if [[ ! -v JOBS_PASS ]]
then
	JOBS_PASS=DEFAULT_PASS
fi

################

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
