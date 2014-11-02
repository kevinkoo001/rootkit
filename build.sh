###########################################################
#	Last Updated: Nov 1, 2014
#	CSE509 System Security 2014 Fall @CS SBU
#	Written By: 
#		Hyungjoon Koo (hykoo@cs.stonybrook.edu)
#		Yaohui Chen (yaohchen@cs.stonybrook.edu)
#
#	Description: Setup module automatically
############################################################

#!/bin/sh

make && (sudo insmod ./kcr.ko || (./flush.sh && make && sudo insmod ./kcr.ko))
