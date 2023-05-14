#!/bin/bash

apt-get update -y
apt-get install -y software-properties-common
add-apt-repository -y ppa:deadsnakes/ppa

apt-get install python3-pip
for PY_VER in 3.8 3.9 3.10 3.11; do
	apt-get install -y \
		"python${PY_VER}" \
		"python${PY_VER}-venv" \
		"libpython${PY_VER}-dev"
done

apt-get clean
