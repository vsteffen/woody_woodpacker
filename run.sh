#!/bin/bash

IMAGE_NAME="woody"
SCRIPT_PATH="$( cd "$(dirname "$0")" >/dev/null 2>&1 ; pwd -P )"

if [[ "$(docker images -q woody 2> /dev/null)" == "" ]]; then
	docker build -t "$IMAGE_NAME" "$SCRIPT_PATH"
	if [ $? -ne 0 ]; then
		exit 1
	fi
fi

docker run -it --mount type=bind,source="$SCRIPT_PATH",target=/root/woody_woodpacker woody
