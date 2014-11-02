###########################################################
#	Last Updated: Nov 1, 2014
#	CSE509 System Security 2014 Fall @CS SBU
#	Written By: 
#		Hyungjoon Koo (hykoo@cs.stonybrook.edu)
#		Yaohui Chen (yaohchen@cs.stonybrook.edu)
#
#	Description: 
#		Should be all exported 
#		Won't be static
############################################################

#!/bin/sh
sudo rmmod kcr.ko
#sudo dmesg -c > /dev/null
make clean
