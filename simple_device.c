#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/slab.h>
#include <linux/device.h>
#include <linux/kdev_t.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Sazonov Pavel");
MODULE_DESCRIPTION("A simple driver");
MODULE_VERSION("0.1");

#define EXAMPLE_MSG "Hello world\n"
#define MSG_BUF_LEN 15

static ssize_t simple_read(struct file *, char *, size_t, loff_t *);
static ssize_t simple_write(struct file *, const char *, size_t, loff_t *); 

static int 	simple_major_num = 101;
static int	simple_size = MSG_BUF_LEN;
static char	simple_data[MSG_BUF_LEN];
static struct class *simple_class;

static struct file_operations file_ops = {
    .owner	 = THIS_MODULE,
    .read	 = simple_read,
    .write	 = simple_write
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

static int __init simple_device_init(void)
{
    printk(KERN_INFO "Loading simple device module\n");
    strncpy(simple_data, EXAMPLE_MSG, MSG_BUF_LEN);
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
