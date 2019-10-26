#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/slab.h>
#include <linux/device.h>
#include <linux/kdev_t.h>
#include <linux/uaccess.h>
#include "simple_driver.h"

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Sazonov Pavel");
MODULE_DESCRIPTION("A simple driver");
MODULE_VERSION("0.2");


static ssize_t simple_read(struct file *, char *, size_t, loff_t *);
static ssize_t simple_write(struct file *, const char *, size_t, loff_t *); 
static ssize_t simple_ioctl(struct file *, unsigned int, unsigned long);

static int 	simple_major_num = 101;
static int	simple_size = MSG_BUF_LEN;
static int 	simple_num_for_ioctl = 1;
static char	simple_data[MSG_BUF_LEN];
static struct 	class *simple_class;

static struct file_operations file_ops = {
    .owner	 	= THIS_MODULE,
    .read	 	= simple_read,
    .write	 	= simple_write,
    .unlocked_ioctl	= simple_ioctl
};

static ssize_t simple_read(struct file *file, char __user *buf, size_t len, loff_t *offset)
{
    ssize_t ret;
    ret = simple_read_from_buffer(buf, len, offset, simple_data, simple_size);
    return ret;
}

static ssize_t simple_write(struct file *file, const char *buffer, size_t len, loff_t *offset)
{
    ssize_t ret;
    char *buf;
    buf = kmalloc(len + 1, GFP_KERNEL);
    if(!buf)
        return -ENOMEM;
    ret = simple_write_to_buffer(buf, len, offset, buffer, len);
    buf[len] = 0;
    if(ret > 0){
        printk(KERN_INFO "buf: %s", buf);
    }
    kfree(buf);
    return ret;
}

static ssize_t simple_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{
    void __user *arg_user;
    arg_user = (void __user *)arg;
    switch(cmd){
    case SIMPLE_DRIVER_WRITE:
        if(copy_from_user(&simple_num_for_ioctl, arg_user, sizeof(simple_num_for_ioctl)))
        {
            return -EFAULT;
        }
        printk(KERN_INFO "new simple_num_for_ioctl: %d", simple_num_for_ioctl);
        break;
    case SIMPLE_DRIVER_READ:
        if(copy_to_user(arg_user, &simple_num_for_ioctl, sizeof(simple_num_for_ioctl)))
        {
            return -EFAULT;
        }
        printk(KERN_INFO "ioctl read");
        break;
    default:
        printk(KERN_INFO "Unknown command ioctl");
    }
    return 0;
}

static int __init simple_device_init(void)
{
    printk(KERN_INFO "Loading simple device module\n");
    strncpy(simple_data, EXAMPLE_MSG, simple_size);
    simple_major_num = register_chrdev(0, "simple_device", &file_ops);
    if(simple_major_num < 0){
        printk(KERN_ALERT "Could not register device: %d\n", simple_major_num);
        return -1;
    }else{
        simple_class = class_create(THIS_MODULE, "simple_device");
	if(IS_ERR(simple_class)){
            unregister_chrdev(simple_major_num, "simple_device");
            return -2;
        }
        device_create(simple_class, NULL, MKDEV(simple_major_num, 0), NULL, "simple_device");
        printk(KERN_INFO "simple device module loaded with device major number: %d\n", simple_major_num);
    }

    printk(KERN_INFO "Load simple device module successful\n");
    return 0;
}

static void __exit simple_device_exit(void)
{
    device_destroy(simple_class, MKDEV(simple_major_num, 0));
    class_destroy(simple_class);
    unregister_chrdev(simple_major_num, "simple_device");
    printk(KERN_INFO "Simple device module unload\n");
}

module_init(simple_device_init);
module_exit(simple_device_exit);
