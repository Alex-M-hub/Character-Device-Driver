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

uint8_t *kernel_buffer; // This declares a pointer to an 8-bit unsigned integer
dev_t dev = 0; // This declares an integer called dev, initialized to 0
static struct cdev chr_cdev; // This declares a structure of type cdev, which will be used to represent the character device
static struct class *dev_class; // This declares a pointer to a structure of type class

static struct file_operations fops =
{
	.owner = THIS_MODULE,
	.read = chr_read,
	.write = chr_write,
	.open = chr_open,
	.release = chr_release,
};

static int __init chr_driver_init(void) {
	//Allocating a character device number using alloc_chrdev_region() function and registering it with the kernel.
	if(alloc_chrdev_region(&dev, 0, 1, "driver") < 0) {
		printk(KERN_INFO "Failed to allocate major number.!\n");
		return -1;
}
	printk(KERN_INFO "Major = %d Minor = %d \n",MAJOR(dev), MINOR(dev));
	cdev_init(&chr_cdev,&fops); //Initializing the character device using cdev_init() function and setting its file operations using fops.
	if((cdev_add(&chr_cdev,dev,1)) < 0){ //Adding the device to the system using cdev_add() function.
		printk(KERN_INFO "Failed to add device to the system.\n");
		unregister_chrdev_region(dev,1);
		cdev_del(&chr_cdev);
		return -1;
}	//Creating a device class using class_create() function.
	if((dev_class = class_create(THIS_MODULE,"chr_class")) == NULL){
		printk(KERN_INFO "Failed to create the device class. \n");
		unregister_chrdev_region(dev,1);
		cdev_del(&chr_cdev);
		return -1;
}	//Creating a device node in the device class using device_create() function.
	if((device_create(dev_class,NULL,dev,NULL,"driver")) == NULL){
		printk(KERN_INFO "Failed to create the device.\n");
		unregister_chrdev_region(dev,1);
		cdev_del(&chr_cdev);
		return -1;
	}
	printk(KERN_INFO "Driver initialized.!!! \n"); //
	return 0; //Returning 0 to indicate successful initialization
}
void __exit chr_driver_exit(void) //serves as the exit function.
{
	device_destroy(dev_class,dev); //destroys the device specified by the dev parameter
	class_destroy(dev_class); //destroys the class specified by the dev_class parameter.
	cdev_del(&chr_cdev); //emoves the character device specified by &chr_cdev from the system.
	unregister_chrdev_region(dev, 1); //unregisters the character device associated with the dev parameter from the kernel, releasing the allocated major and minor numbers
	printk(KERN_INFO "Driver exited.!!!\n"); //logs a message to the system log indicating that the driver has exited.
}

static int chr_open(struct inode *inode, struct file *file)
{
	printk(KERN_INFO "Open!!!\n"); //prints a message to the kernel log 
	
	if((kernel_buffer = kmalloc(1024, GFP_KERNEL)) == 0){ //The function then checks if memory can be allocated dynamically.
		printk(KERN_INFO "Failed to allocate memory for the device buffer."); //f memory allocation fails, the function prints an error message to the kernel log 
		return -1; //The function returns -1 to indicate the failure.
	}
	return 0; //If the memory allocation is successful, the function returns 0 to indicate success.

}
//Define a function named chr_release that is called when a user process closes a device file.
static int chr_release(struct inode *inode, struct file *file)
{
	printk(KERN_INFO "Close!!!\n"); //prints a message to the kernel log
	kfree(kernel_buffer); //this function frees the memory allocated to the kerner_buffer.
	return 0;
}
static ssize_t chr_read(struct file *filp,char __user *buf, size_t len, loff_t *off)
{
printk(KERN_INFO "Read\n");
	//Determines the length of the data stored in the kernel_buffer using the strlen() function.
	size_t datalen = strlen(kernel_buffer);
	//If the specified length of data to be read is greater than the length of the data available, the length is adjusted to be the length of available data
	if(len > datalen){
		len = datalen;
	}
	//This function copies the contents of kernel_buffer to the user-space buffer.
	if (copy_to_user(buf, kernel_buffer, len)) {
		return -EFAULT;
	}
return len;
}
//This code defines a function called chr_write which is used to handle write operations.
static ssize_t chr_write(struct file *filp,const char __user *buf, size_t len, loff_t *off)
{
	printk(KERN_INFO "Write\n");
	//If the length of the data is greater than 1024 bytes, and if so, sets the length to 1024 bytes instead.
	if(len > 1024){
		len = 1024;
	}
	//copy the data from the user space buffer indicated by buf to a kernel space buffer called kernel_buffer.
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
