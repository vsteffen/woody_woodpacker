#!/bin/bash

SCRIPT=`realpath $0`
SCRIPT_PATH=`dirname $SCRIPT`

LOG=$SCRIPT_PATH/exec_all.log
rm -rf $LOG
touch $LOG

TMP_DIR=$(mktemp -d)

cd $TMP

timeout_sec="$1"
if [ -z "$1" ] ; then
	timeout_sec="1"
fi

for entry in /bin/* /sbin/* /usr/bin/*
do
	# Removing programs which stop script
	if [[ "$entry" = "/bin/pidof" \
	|| "$entry" = "/usr/bin/yes" \
	|| "$entry" = "/sbin/killall5" ]] ; then
		continue
	fi
	printf "\n------------------> $entry\n"
	$SCRIPT_PATH/../woody_woodpacker $entry 2>&1 > /dev/null
	if [ $? -eq 0 ] ; then
		script -q -f -c "bash -c \"timeout --foreground --signal=SIGKILL $timeout_sec ./woody 0>&-\"" -a $LOG | grep -v 'Script done' | grep -v 'Script started'
	fi
done

rm -rf $TMP_DIR

exit 0
