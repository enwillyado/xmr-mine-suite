#!/bin/bash
source vars.sh

wget -q -O - http://$WORKERS_HOST:$WORKERS_PORT/$1
