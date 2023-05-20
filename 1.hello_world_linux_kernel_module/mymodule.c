#include <linux/module.h>
#include <linux/init.h>

/* Meta Information */
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Pradeep");
MODULE_DESCRIPTION(" Hello world LKM");

/**
 * @brief This function is called, when the module is loaded into the kernel
 */
static int __init ModuleInit(void) {
	printk("Hello, Kernel!\n");
	return 0;
}

/**
 * @brief this function is called when module is removed from the kernel
 */
static void __exit ModuleExit(void) {
	printk("GoodBye, Kernel\n");
}

module_init(ModuleInit);
module_exit(ModuleExit);
