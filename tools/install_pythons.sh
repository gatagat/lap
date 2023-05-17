#!/bin/bash

apt-get update -y
DEBIAN_FRONTEND="noninteractive" apt-get -y install tzdata
ln -fs /usr/share/zoneinfo/Europe/Vienna /etc/localtime
dpkg-reconfigure --frontend noninteractive tzdata
apt-get install -y software-properties-common
add-apt-repository -y ppa:deadsnakes/ppa

apt-get install -y python3-pip
for PY_VER in 3.8 3.9 3.10 3.11; do
	apt-get install -y \
		"python${PY_VER}" \
		"python${PY_VER}-venv" \
		"libpython${PY_VER}-dev"
done

apt-get clean
rm -rf /var/lib/apt/lists/*
