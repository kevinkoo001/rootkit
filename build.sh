#!/bin/sh

make && (sudo insmod ./evil.ko || (./flush.sh && make && sudo insmod ./evil.ko))
