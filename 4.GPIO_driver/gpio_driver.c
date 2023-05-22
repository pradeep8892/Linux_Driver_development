#include <linux/module.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/uaccess.h>
#include <linux/gpio.h>

/* Meta information*/
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Pradeep");
MODULE_DESCRIPTION("A simple gpio driver for setting a LED and reading a button");

/* Buffer for storing data*/
static int buffer[255];
static int buffer_pointer;

/* Variable for device and device class*/
static dev_t my_device_nr;
static struct class *my_class;
static struct cdev my_device;

#define DRIVER_NAME "my_gpio_driver"
#define DRIVER_CLASS "MyModuleClass"
/**
 * @brief Read data out of the buffer
 */

static ssize_t driver_read(struct file *File, char *user_buffer, size_t count, loff_t *offs){
	char tmp[3] = "\n";
	/* get amount of data to copy*/
	int to_copy = min(count, sizeof(tmp));

	/* Read value of button*/
	printk("Value of Button: %d\n", gpio_get_value(17));
	tmp[0] = gpio_get_value(17)+'0';
	/*copy data to user buffer*/
	int not_copied = copy_to_user(user_buffer, tmp, to_copy);

	/*Calculate data*/
	int delta = to_copy - not_copied;

	return delta;
}

/**
 * @brief Write data to the buffer
 */
static ssize_t driver_write(struct file *File,const  char *user_buffer, size_t count, loff_t *offs){
	int to_copy, not_copied, delta;
	char value;
	/* get amount of data to copy*/
	to_copy = min(count, sizeof(value));

	/*copy data to user buffer*/
	not_copied = copy_from_user(&value, user_buffer, to_copy);
	
	/* Setting the LED*/
	switch(value)
	{
		case '0':
			gpio_set_value(4,0);
			break;
		case '1':
			gpio_set_value(4,1);
			break;
		default:
			printk("Invalid Input");
			break;
	}
	/*Calculate data*/
	delta = to_copy - not_copied;

	return delta;
}

/**
 * @brief thie function is called, when the device file is opened
 */
static int driver_open(struct inode *device_file, struct file *instance)
{
	printk("dev_nr - Open was called!\n");
	return 0;
}
static int driver_close(struct inode *device_file, struct file *instance)
{
	printk("dev_nr - close was called!\n");
	return 0;
}
static struct file_operations fops={
	.owner = THIS_MODULE,
	.open = driver_open,
	.release = driver_close,
	.read = driver_read,
	.write = driver_write

};


/**
 * this function is called when module is loaded into the kernel
 */
 int __init ModuleInit(void)
{
	int retval;

	printk("Hello, kernel!\n");

	/* ALlocate a device number*/
	if(alloc_chrdev_region(&my_device_nr, 0, 1, DRIVER_NAME) < 0){
		printk("Device Nr. could not be allocated!\n");
		return -1;
	}
	printk("read_write - Device Nr. Major: %d, Minor: %d was registered!\n", my_device_nr>>20, my_device_nr && 0xfffff);
	/*create device class*/
	if((my_class = class_create(THIS_MODULE, DRIVER_CLASS)) == NULL){
		printk("Device class can not be created!\n");
		goto ClassError;
	}
	/* create device file*/
	if(device_create(my_class, NULL, my_device_nr, NULL, DRIVER_NAME) == NULL){
		printk("Can not create device file!\n");
		goto FileError;
	}

	/* Initialize device file*/
	cdev_init(&my_device, &fops);

	/*Registering device to kernel*/
	if(cdev_add(&my_device, my_device_nr, 1) == -1){
		printk("Registering of device to kernel failed!\n");
		goto AddError;
	}
	/* GPIO 4 Init*/
	if(gpio_request(4, "rpi-gpio-4")) {
		printk("Can not allocate GPIO 4\n");
		goto AddError;
	}
	/* Set GPIO 4  direction*/
	if(gpio_direction_output(4, 0)) {
		printk("Can not set GPIO 4 to output!\n");
		goto Gpio4Error;
	}

	/* GPIO 17  Init*/
	if(gpio_request(17, "rpi-gpio-17")) {
		printk("Can not allocate GPIO 4\n");
		goto AddError;
	}
	/* Set GPIO 17  direction*/
	if(gpio_direction_input(17)) {
		printk("Can not set GPIO 17 to output!\n");
		goto Gpio4Error;
	}

	return 0;
Gpio17Error:
	gpio_free(17);
Gpio4Error:
	gpio_free(4);
AddError:
	device_destroy(my_class, my_device_nr);
FileError:
	class_destroy(my_class);
ClassError:
	unregister_chrdev(my_device_nr, DRIVER_NAME);
	return -1;

	
}

static void __exit ModuleExit(void)
{
	gpio_set_value(4,0);
	gpio_free(17);
	gpio_free(4);
	cdev_del(&my_device);
	device_destroy(my_class, my_device_nr);
	class_destroy(my_class);
	unregister_chrdev(my_device_nr, DRIVER_NAME);
	printk("Goodbye, Kernel!\n");

}

module_init(ModuleInit);
module_exit(ModuleExit);

