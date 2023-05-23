#include <linux/module.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/uaccess.h>
#include <linux/gpio.h>
#include <linux/delay.h>

/* Meta information*/
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Pradeep");
MODULE_DESCRIPTION("A driver to write data to a LCD text Display");


/* Variable for device and device class*/
static dev_t my_device_nr;
static struct class *my_class;
static struct cdev my_device;

#define DRIVER_NAME "lcd"
#define DRIVER_CLASS "MyModuleClass"

/* LCD Char buffer*/
static char lcd_buffer[17];

/* pinout for LCD Display*/
unsigned int gpios[] = {
	3, 	// Enable pin
	2,	// Register select pin
	4,	// Data Pin 0
	17,	// Data Pin 1	
	27,	// Data Pin 2
	22, 	// Data Pin 3
	10, 	// Data Pin 4
	9,	// Data Pin 5
	11,	// Data Pin 6
	5,	// Data Pin 7
};

#define ENABLE_PIN gpios[0]
#define REGISTER_SELECT gpios[1]

void lcd_enable(void)
{
	gpio_set_value(ENABLE_PIN, 1);
	msleep(5);
	gpio_set_value(ENABLE_PIN,0);
}
void lcd_send_byte(char data)
{
	int i;
	for(i = 0; i<8;i++)
		gpio_set_value(gpios[i+2], ((data) & (1<<i))>>i);
	lcd_enable();
	msleep(5);
}

void lcd_command(uint8_t data)
{
	gpio_set_value(REGISTER_SELECT, 0); //RS to instruction
	lcd_send_byte(data);
}

void lcd_data(uint8_t data)
{
	gpio_set_value(REGISTER_SELECT, 1);
	lcd_send_byte(data);
}


/**
 * @brief Read data out of the buffer
 */

//static ssize_t driver_read(struct file *File, char *user_buffer, size_t count, loff_t *offs){
//	char tmp[3] = "\n";
	/* get amount of data to copy*/
//	int to_copy = min(count, sizeof(tmp));

	/* Read value of button*/
//	printk("Value of Button: %d\n", gpio_get_value(17));
//	tmp[0] = gpio_get_value(17)+'0';
	/*copy data to user buffer*/
//	int not_copied = copy_to_user(user_buffer, tmp, to_copy);

	/*Calculate data*/
//	int delta = to_copy - not_copied;

//	return delta;
//}

/**
 * @brief Write data to the buffer
 */
static ssize_t driver_write(struct file *File,const  char *user_buffer, size_t count, loff_t *offs){
	int to_copy,i, not_copied, delta;
	
	/* get amount of data to copy*/
	to_copy = min(count, sizeof(lcd_buffer));

	/*copy data to user buffer*/
	not_copied = copy_from_user(lcd_buffer, user_buffer, to_copy);
	
	/* Calculate data*/
	delta = to_copy - not_copied;
	/* set the new data to the display*/

	lcd_command(0x1);
	for(i = 0;i<to_copy;i++)
		lcd_data(lcd_buffer[i]);

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
/**
 * @brief This function is called, when the device file is opened
 */
static int driver_close(struct inode *device_file, struct file *instance)
{
	printk("dev_nr - close was called!\n");
	return 0;
}
static struct file_operations fops={
	.owner = THIS_MODULE,
	.open = driver_open,
	.release = driver_close,
//	.read = driver_read,
	.write = driver_write

};


/**
 * this function is called when module is loaded into the kernel
 */
 int __init ModuleInit(void)
{
	int i;
	char *names[] = {"ENABLE_PIN", "REGISTER_SELECT","DATA_PIN0","DATA_PIN1","DATA_PIN2","DATA_PIN3","DATA_PIN4","DATA_PIN5","DATA_PIN6", "DATA_PIN7"};
	
	printk("Hello, kernel!\n");
	char text[] = "Hello LCD";

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
		printk("LCD Driver - Registering of device to kernel failed!\n");
		goto AddError;
	}
	printk(" LCD driver - GPIO Init");
	for(i = 0;i<10;i++)
	{

		if(gpio_request(gpios[i], names[i])){
			printk("LCD-Driver, error init GPIO %d\n", gpios[i]);
			goto GpioInitError;
		}
	}
	for (i = 0;i<10 ;i++)
	{
		if(gpio_direction_output(gpios[i], 0)){
			printk("LCD-Driver - Error Setting gpio %d to output", gpios[i]);
			goto GpioDirectionError;
		}
	}
	
	/* Init the display*/
	lcd_command(0x30); // Set the display for 8 bit data interface

	lcd_command(0xf); // Turn display on, turn cusror on, set cursar blinking

	lcd_command(0x1);

	char txt[] = "Hello LCD";
	for (i = 0;i<sizeof(txt) - 1; i++)
		lcd_data(txt[i]);
	return 0;
GpioDirectionError:
	i=9;
GpioInitError:
	for(;i>=0;i--)
		gpio_free(gpios[i]);
AddError:
	device_destroy(my_class, my_device_nr);
FileError:
	class_destroy(my_class);
ClassError:
	unregister_chrdev(my_device_nr, 1);
	return -1;

	
}

static void __exit ModuleExit(void)
{
	int i = 0;
	lcd_command(0x1);
	for (i = 0;i<10;i++)
	{
		gpio_set_value(gpios[i], 0);
		gpio_free(gpios[i]);
	}
	cdev_del(&my_device);
	device_destroy(my_class, my_device_nr);
	class_destroy(my_class);
	unregister_chrdev(my_device_nr, DRIVER_NAME);
	printk("Goodbye, Kernel!\n");

}

module_init(ModuleInit);
module_exit(ModuleExit);

