#!/bin/bash

target="$1"

if [ "${target}" == "status" ]
then
	ps -ef | grep "cupid " | grep -v grep
elif [ "${target}" == "stop" ]
then
	killall -s 9 cupid
elif [ "${target}" == "start" ]
then
	./cupid cupid.lua > cupid.log 2>&1 &
	sleep 1
	ps -ef | grep "cupid " | grep -v grep
elif [ "${target}" == "restart" ]
then
	ps -ef | grep "cupid " | grep -v grep
	sleep 1
	killall -s 9 cupid
	sleep 1
	./cupid cupid.lua > cupid.log 2>&1 &
	sleep 1
	ps -ef | grep "cupid " | grep -v grep
else
	echo "wrong command !"
fi
