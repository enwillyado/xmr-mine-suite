#!/bin/bash
source vars.sh

wget -q -O - http://$SERVER_HOST:$SERVER_PORT/$1
