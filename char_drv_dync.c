#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <asm/errno.h>
#include <linux/device.h>
#include <soc/qcom/qseecomi.h>
#include <soc/qcom/scm.h>
#include <linux/slab.h>

#define DEVICE_NAME "sai"
#define CLASS_NAME "sai"

static int major_no;
static int num_devices_allowed = 0;
static struct class *sai_class;
static struct device *sai_device;

static int device_open(struct inode *, struct file *);
static int device_release(struct inode *, struct file *);
static ssize_t device_read(struct file *, char *, size_t, loff_t *);
static ssize_t device_write(struct file *, const char *, size_t, loff_t *);

static long device_ioctl(struct file *f, unsigned int ioctl_cmd,
                         unsigned long ioctl_arg) {
  int err = -1;
  switch (ioctl_cmd) {
    case 1:
      pr_debug("In ioctl case 1");
      break;
    default:
      err = -ENOTTY;
      pr_debug("no ioctl defined ");
      break;
  }

  return err;
}

static struct file_operations fops = {.open = device_open,
                                      .read = device_read,
                                      .write = device_write,
                                      .unlocked_ioctl = device_ioctl,
                                      .release = device_release};

int sai_init(void) {
  major_no = register_chrdev(0, DEVICE_NAME, &fops);

  if (major_no < 0) {
    pr_alert("Registering char device failed \n");
    return major_no;
  }

  sai_class = class_create(THIS_MODULE, CLASS_NAME);
  if (IS_ERR(sai_class)) {
    pr_err("Cannot create the class %s \n", DEVICE_NAME);
    unregister_chrdev(major_no, DEVICE_NAME);
    return PTR_ERR(sai_class);
  }
  pr_debug("class creation sucess\n");
  sai_device =
      device_create(sai_class, NULL, MKDEV(major_no, 0), NULL, CLASS_NAME);
  if (IS_ERR(sai_device)) {
    pr_err("Cannot create the device %s \n", DEVICE_NAME);
    class_destroy(sai_class);
    unregister_chrdev(major_no, DEVICE_NAME);
    return PTR_ERR(sai_device);
  }

  pr_debug("device creation success\n");
  return SUCCESS;
}

void sai_cleanup(void) {
  device_destroy(sai_class, MKDEV(major_no, 0));
  class_destroy(sai_class);
  unregister_chrdev(major_no, DEVICE_NAME);
  pr_debug("device cleanup success");
}

static int device_open(struct inode *inode, struct file *filp) {
  if (num_devices_allowed != 0) {
    return -EBUSY;
  }
  num_devices_allowed++;
  return SUCCESS;
}

static int device_release(struct inode *inode, struct file *filp) {
  num_devices_allowed = 0;
  return SUCCESS;
}

static ssize_t device_read(struct file *filp, /* see include/linux/fs.h   */
                           char *buffer,      /* buffer to fill with data */
                           size_t length,     /* length of the buffer     */
                           loff_t *offset) {
  pr_alert("This operation not Implemented.\n");
  return -EPERM;
}

static ssize_t device_write(struct file *filp, const char *buf, size_t len,
                            loff_t *off) {
  pr_alert("This operation not Implemented.\n");
  return -EPERM;
}

module_init(sai_init);
module_exit(sai_cleanup);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Manikanta");
MODULE_DESCRIPTION("Sample char driver");
MODULE_VERSION("1");
