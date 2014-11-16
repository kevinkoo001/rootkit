/**********************************************************
	Last Updated: Nov 16, 2014
	CSE509 System Security 2014 Fall @CS SBU
	Written By: 
		Hyungjoon Koo (hykoo@cs.stonybrook.edu)
		Yaohui Chen (yaohchen@cs.stonybrook.edu)

	Description: 
		Define global variables and macros
***********************************************************/

#define PROT_ENABLE write_cr0(read_cr0() | 0x10000)			// CR0 Register protection Enabled
#define PROT_DISABLE write_cr0(read_cr0() & (~ 0x10000))	// CR0 Register protection Disabled

#define MAJOR_NUM 104		// misc major number
#define BUFLEN  512			// rk_buf length
#define PASSBUF 4096		// passwd bug
#define DEBUG 0				// debug mode