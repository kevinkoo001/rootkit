###########################################################
#	Last Updated: Nov 16, 2014
#	CSE509 System Security 2014 Fall @CS SBU
#	Written By: 
#		Hyungjoon Koo (hykoo@cs.stonybrook.edu)
#		Yaohui Chen (yaohchen@cs.stonybrook.edu)
#
#	Description: 
#		Clear module automatically
#		Should be all exported 
#		Won't be static
############################################################

#!/bin/sh

RMMOD=`which rmmod`
REG_DEV='kcr'
MOD_NAME='kcr'

sudo $RMMOD $MOD_NAME
sudo dmesg -c > /dev/null
make clean
rm -f /dev/$REG_DEV
rm -f /dev/$REG_DEV ./.*.o.cmd ./src/.*.cmd
