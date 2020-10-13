#!/bin/bash -


debug() {
	if [ $DEBUG ];
	then
		echo ""
		echo "[DEBUG]: $@"
	fi
}

function exec_print() {
	if [ $DEBUG ];
	then
		echo ""
		echo "[DEBUG]: $@"
		$@
	else
		echo $@
		$@ 2> /dev/null > /dev/null
	fi
}

exec_print "ls -la ."

