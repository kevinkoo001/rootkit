/**********************************************************
	Last Updated: Nov 1, 2014
	CSE509 System Security 2014 Fall @CS SBU
	Written By: 
		Hyungjoon Koo (hykoo@cs.stonybrook.edu)
		Yaohui Chen (yaohchen@cs.stonybrook.edu)

	Description: Common linux header files
***********************************************************/

#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/errno.h>		// error numbers
#include <linux/types.h>

#include <linux/module.h>		// struct module
#include <linux/unistd.h>		// all system call numbers
#include <linux/cred.h>			// struct cred
#include <linux/sched.h>		// struct task_struct
#include <linux/syscalls.h>		// all system calls
#include <linux/slab.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/ioctl.h>
#include <linux/miscdevice.h>
#include <linux/string.h>		// String manipulation

#include <asm/system.h>
#include <asm/current.h>

#include "config.h"
