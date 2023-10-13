#!/bin/bash

set -eu

docker compose build

echo Please put the device in flash mode and press enter
read
docker compose run base isp55e0 --data-dump flashdata.bin

docker compose run base python3 tools/set_keys.py

echo Please put the device in flash mode and press enter
read
docker compose run base isp55e0 --data-flash flashdata.bin
