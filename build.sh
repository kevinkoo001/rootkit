###########################################################
#	Last Updated: Nov 16, 2014
#	CSE509 System Security 2014 Fall @CS SBU
#	Written By: 
#		Hyungjoon Koo (hykoo@cs.stonybrook.edu)
#		Yaohui Chen (yaohchen@cs.stonybrook.edu)
#
#	Description: Setup module automatically
############################################################

#!/bin/sh

INSMOD=`which insmod`
MOD_NAME='kcr.ko'

make && (sudo $INSMOD $MOD_NAME || (./flush.sh && make && sudo $INSMOD $MOD_NAME))
