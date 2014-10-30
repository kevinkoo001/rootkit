static ssize_t rk_read(struct file *filp,  char __user *buf, size_t len, loff_t offset);
static ssize_t rk_write(struct file *filp, char __user *buf, size_t len, loff_t offset);
static int rk_open(struct inode* ino, struct file *filp);
static int rk_close(struct inode* ino, struct file *filep);



char rk_buf[BUFLEN];
int is_open = 0;

const struct file_operations rk_fops = {

	.owner= THIS_MODULE,
	.read = rk_read,
	.write= rk_write,
	.open = rk_open,
	.release = rk_close,
	

};

struct miscdevice evil = {

	 .minor = MISC_DYNAMIC_MINOR,
	 .name  = "evil",
	 .fops  = &rk_fops,
	 .mode  = 0666

};



//EXPORT_SYMBOL(evil);

