#include <linux/module.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/ioctl.h>

#define MYIOCTL_MAGIC 'k'
#define MYIOCTL_RESET _IO(MYIOCTL_MAGIC, 0)
#define MYIOCTL_GET_COUNT _IOR(MYIOCTL_MAGIC, 1, int)
#define MYIOCTL_INCREMENT _IOW(MYIOCTL_MAGIC, 2, int)

MODULE_LICENSE("GPL");
MODULE_AUTHOR("G20");
MODULE_DESCRIPTION("Simple IOCTL Example");

static int myioctl_major;
static int counter = 0;

static char buffer[1024];
static size_t buffer_size = sizeof(buffer);
static loff_t f_offset = 0;
static struct file *log_file;

// Function prototypes
static int myioctl_open(struct inode *inode, struct file *filp);
static int myioctl_release(struct inode *inode, struct file *filp);
static long myioctl_ioctl(struct file *filp, unsigned int cmd, unsigned long arg);

// File operations structure
static const struct file_operations myioctl_fops = {
    .open = myioctl_open,
    .release = myioctl_release,
    .unlocked_ioctl = myioctl_ioctl,
};

// Module initialization
static int __init myioctl_init(void){
    myioctl_major = register_chrdev(0, "myioctl", &myioctl_fops);

    if (myioctl_major < 0){
        pr_err("Failed to register character device");
        return myioctl_major;
    }

    pr_info("myioctl module loaded. Major number: %d\n", myioctl_major);

    // Open exsited log file
    log_file = filp_open("/var/log/mydevice.log", O_RDWR | O_CREAT, 0666);

    if(IS_ERR(log_file)) pr_err("myioctl: Unable to open log file.");
    else pr_info("myioctl: Log file open successfully.");

    // Read counter value 
    size_t bytes_read = kernel_read(log_file, buffer, buffer_size, &f_offset);

    if(bytes_read > 0){
        pr_info("myioctl: read success.");
    }
    else pr_err("myioctl: log file read fail.");

    // Load value into counter
    sscanf(buffer, "%d", &counter);
    pr_info("myioctl: Counter loaded..");

    return 0;
}

// Module cleanup
static void __exit myioctl_exit(void){
    unregister_chrdev(myioctl_major, "myioctl");

    // Write counter value back to log file
    if(log_file != NULL){
        memset(buffer, 0, buffer_size);
        f_offset = 0;
        sprintf(buffer, "%d", counter);
        kernel_write(log_file, buffer, buffer_size, &f_offset);
        pr_info("myioctl: Counter value saved.");
    }

    pr_info("myioctl module unloaded");
}

// Open function
static int myioctl_open(struct inode *inode, struct file *filp){
    pr_info("myioctl device opened");
    return 0;
}

// Release function
static int myioctl_release(struct inode *inode, struct file *filp){
    pr_info("myioctl device closed");
    return 0;
}

// IOCTL function
static long myioctl_ioctl(struct file *filp, unsigned int cmd, unsigned long arg){
    int err = 0;
    int tmp;

    if(_IOC_TYPE(cmd) != MYIOCTL_MAGIC){
        pr_err("Invalid magic number");
        return -ENOTTY;
    }

    switch(cmd){
        case MYIOCTL_RESET:
            pr_info("IOCTL: Resetting counter\n");
            counter = 0;
            break;

        case MYIOCTL_GET_COUNT:
            pr_info("IOCTL: Getting counter value\n");
            err = copy_to_user((int *)arg, &counter, sizeof(int));
            break;

        case MYIOCTL_INCREMENT:
            pr_info("IOCTL: Incrementing counter\n");
            err = copy_from_user(&tmp, (int *)arg, sizeof(int));
            if(err == 0){
                counter += tmp;
            }
            else pr_info("increment fail");
            break;

        default:
            pr_err("Unknown IOCTL command\n");
            return -ENOTTY;
    }

    return err;
}

module_init(myioctl_init);
module_exit(myioctl_exit);