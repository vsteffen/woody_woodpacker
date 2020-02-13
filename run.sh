#!/bin/bash
docker run -it --mount type=bind,source=$(pwd),target=/root/woody_woodpacker woody
