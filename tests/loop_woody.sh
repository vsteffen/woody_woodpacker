#!/bin/bash

if [ ! $# -eq 2 ] ; then
	echo "Usage: $0 elf_binary number_of_iteration"
	exit 1
fi

if [ "$2" -lt 1 ] ; then
	echo "Wrong number of iteration"
	exit 1
fi

SCRIPT=`realpath $0`
SCRIPT_PATH=`dirname $SCRIPT`

cd $SCRIPT_PATH/..

TMP_WOODY=$(mktemp)
cp $1 $TMP_WOODY

i=1
while [ "$i" -le "$2" ]; do
	./woody_woodpacker $TMP_WOODY 2>&1 > /dev/null
	mv ./woody $TMP_WOODY
	i=$(($i + 1))
done

read -p "Execute woody ? [Y/n]" -n 1 -r
if [[ $REPLY =~ ^[Yy]$ || -z $REPLY ]] ; then
	$TMP_WOODY
fi

echo

mv $TMP_WOODY ./woody

exit 0
