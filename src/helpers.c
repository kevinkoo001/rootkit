#include "../headers/all.h"
#include"../headers/helpers.h"

EXPORT_SYMBOL(getHideStr);
EXPORT_SYMBOL(my_memcpy);


/*
 * extern symbols here
 *
 */

extern char *(hide_prefix[]);



void 
getHideStr(char *buf){
	
	char* ptr; 
	char  c;
	unsigned int pos1,pos2,pos3; 

	memset(buf,0,BUFLEN);
	pos1= pos2 = 0;
	ptr= *hide_prefix;
	while(ptr){
		pos3 = 0;
		c = *ptr;
		while(c){
			pos3++;
			buf[pos2++] = c;
			c = *(ptr+pos3);
		}
		buf[pos2++] = '@';
		pos1++;
		ptr  = *(hide_prefix+pos1);
	}

	buf[pos2++] = '\n';

}



/*static void setHideStr(char *(buf[])){*/
/*//some mem leaking here : (, for now just assume you wouldn't do too much hide_prefix reset operation.*/
	/*int idx, hidestrCount, lenbuf;*/
	/*char *found, *hidestr, *rest;*/

	/*idx = 0;*/
		
	/*while(*(buf+idx)){*/
		/*kfree(*(buf+idx));*/
	/*}*/
	/*hidestrCount = 0;*/
	/*rest = kmalloc(strlen(buf)+1, GFP_KERNEL);*/
	/*my_memcpy(rest, buf,strlen(buf)+1);*/
	/*lenbuf=strlen(rest);*/
	/*found = strstr(rest,"@");*/
	/*while(found != NULL){*/
		/*idx = found - rest;*/
		/*hidestr = kmalloc(idx+1,GFP_KERNEL);*/
		/*my_memcpy(hidestr, rest, idx);*/
		/*buf[hidestrCount++] = kmalloc(sizeof(char *),GFP_KERNEL);*/
		/*buf[ hidestrCount] = hidestr;*/
		/*//rest = kmalloc(strlen(buf+idx+1)+1,GFP_KERNEL);*/
		/*printk("hidestr %s\n",buf[hidestrCount]);*/
		/*lenbuf = lenbuf - strlen(hidestr) -1 ;*/
		/*my_memcpy(rest, rest+idx+1, lenbuf);	*/
		/*printk("rest %s idx %d strlen %d\n",rest, idx, lenbuf);*/
		/*found = strstr(rest,"@");*/
	/*}*/
	/*kfree(rest);*/

	/*printk("%s %s %s\n",*hide_prefix, *(hide_prefix +1), *(hide_prefix+2));*/

/*}*/



void 
my_memcpy(char * dst, char *src, int len){
	int i;
	for(i=0; i<len ; i++){
	
		dst[i]=src[i];
	}
	dst[i] = '\0';
}
