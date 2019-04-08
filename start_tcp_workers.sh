#!/bin/bash
#
#	WORKERS_PORT=8888 JOBS_HOST=mine.xmrpool.net JOBS_PORT=3333 JOBS_USER=wallet JOBS_PASS=x ./start_tcp_workers.sh
#
source vars.sh

./finder_app.exe $WORKERS_PORT $JOBS_HOST $JOBS_PORT $JOBS_USER $JOBS_PASS
