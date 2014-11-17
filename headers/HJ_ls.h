/**********************************************************
	Last Updated: Nov 16, 2014
	CSE509 System Security 2014 Fall @CS SBU
	Written By: 
		Hyungjoon Koo (hykoo@cs.stonybrook.edu)
		Yaohui Chen (yaohchen@cs.stonybrook.edu)

	Description: 
		Headers for hijacking 'getdents' system call
		Defines a certain prefix for hiding files
***********************************************************/

static struct dirent
{
	uint64_t		d_ino;
	int64_t		d_off;
	unsigned short int	d_reclen;
//	unsigned char	d_type;
	char		d_name[];
};


char *(hide_prefix[]) = {"bad_","hide_"};

asmlinkage int
my_getdents(unsigned int fd, struct linux_dirent64 *dirp, unsigned int count);

