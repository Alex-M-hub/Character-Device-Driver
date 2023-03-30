#include<linux/kernel.h>
#include<linux/init.h>
#include<linux/module.h>
#include<linux/kdev_t.h>
#include<linux/fs.h>
#include<linux/cdev.h>
#include<linux/device.h>
#include<linux/slab.h>
#include<linux/uaccess.h>

static int __init chr_driver_init(void);
static void __exit chr_driver_exit(void);
static int chr_open(struct inode *inode, struct file *file);
static int chr_release(struct inode *inode, struct file *file);
static ssize_t chr_read(struct file *filp,char __user *buf, size_t len,loff_t * off);
static ssize_t chr_write(struct file *filp,const char *buf, size_t len, loff_t * off);

uint8_t *kernel_buffer;
dev_t dev = 0;
static struct cdev chr_cdev;
static struct class *dev_class;

static struct file_operations fops =
{
	.owner = THIS_MODULE,
	.read = chr_read,
	.write = chr_write,
	.open = chr_open,
	.release = chr_release,
};

static int __init chr_driver_init(void) {
	if(alloc_chrdev_region(&dev, 0, 1, "driver") < 0) {
		printk(KERN_INFO "Failed to allocate major number.!\n");
		return -1;
}
	printk(KERN_INFO "Major = %d Minor = %d \n",MAJOR(dev), MINOR(dev));
	cdev_init(&chr_cdev,&fops);
	if((cdev_add(&chr_cdev,dev,1)) < 0){
		printk(KERN_INFO "Failed to add device to the system.\n");
		unregister_chrdev_region(dev,1);
		cdev_del(&chr_cdev);
		return -1;
}
	if((dev_class = class_create(THIS_MODULE,"chr_class")) == NULL){
		printk(KERN_INFO "Failed to create the device class. \n");
		unregister_chrdev_region(dev,1);
		cdev_del(&chr_cdev);
		return -1;
}
	if((device_create(dev_class,NULL,dev,NULL,"driver")) == NULL){
		printk(KERN_INFO "Failed to create the device.\n");
		unregister_chrdev_region(dev,1);
		cdev_del(&chr_cdev);
		return -1;
	}
	printk(KERN_INFO "Driver initialized.!!! \n");
	return 0;
}
void __exit chr_driver_exit(void)
{
	device_destroy(dev_class,dev);
	class_destroy(dev_class);
	cdev_del(&chr_cdev);
	unregister_chrdev_region(dev, 1);
	printk(KERN_INFO "Driver exited.!!!\n");
}

static int chr_open(struct inode *inode, struct file *file)
{
	printk(KERN_INFO "Open!!!\n");
	
	if((kernel_buffer = kmalloc(1024, GFP_KERNEL)) == 0){
		printk(KERN_INFO "Failed to allocate memory for the device buffer.");
		return -1;
	}
	return 0;

}
static int chr_release(struct inode *inode, struct file *file)
{
	printk(KERN_INFO "Close!!!\n");
	kfree(kernel_buffer);
	return 0;
}
static ssize_t chr_read(struct file *filp,char __user *buf, size_t len, loff_t *off)
{
printk(KERN_INFO "Read\n");

	size_t datalen = strlen(kernel_buffer);
	if(len > datalen){
		len = datalen;
	}
	if (copy_to_user(buf, kernel_buffer, len)) {
		return -EFAULT;
	}
return len;
}

static ssize_t chr_write(struct file *filp,const char __user *buf, size_t len, loff_t *off)
{
	printk(KERN_INFO "Write\n");
	if(len > 1024){
		len = 1024;
	}
	if(copy_from_user(kernel_buffer, buf, len)){
		return -EFAULT;
	}
	return len;
}

module_init(chr_driver_init);
module_exit(chr_driver_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Alexandru Maruntu");
MODULE_DESCRIPTION("Driver char");
