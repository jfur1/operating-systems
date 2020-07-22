#include<linux/kernel.h>
#include<linux/linkage.h>
#include<linux/uaccess.h>

asmlinkage long sys_cs3753_add(int a, int b, int* c)
{
	int tmp = a + b;
	printk(KERN_ALERT "Value of a is %d\n", a);
	printk(KERN_ALERT "Value of b is %d\n", b);

	put_user(tmp, c);

	printk(KERN_ALERT "Value of a + b is %d\n", tmp);
	return 0;
}
