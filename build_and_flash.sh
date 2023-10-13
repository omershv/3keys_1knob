#!/bin/bash

set -eu

docker compose build

docker compose run base make bin

echo Please put the device in flash mode and press enter
read

docker compose run base make flash
