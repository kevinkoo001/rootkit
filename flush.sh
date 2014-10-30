#!/bin/sh
sudo rmmod evil
sudo dmesg -c > /dev/null
make clean
