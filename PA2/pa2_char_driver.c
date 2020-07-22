#include<linux/init.h>
#include<linux/module.h>

#include<linux/fs.h>
#include<linux/slab.h>
#include<linux/uaccess.h>

#define BUFFER_SIZE 1024

/* Define device_buffer and other global data structures you will need here */
static char device_buffer[BUFFER_SIZE];
//char* buffer;
// Counts for open/close functions
int o_count = 0;
int c_count = 0;


ssize_t pa2_char_driver_read (struct file *pfile, char __user *buffer, size_t length, loff_t *offset)
{
	/* *buffer is the userspace buffer to where you are writing the data you want to be read from the device file*/
	/* length is the length of the userspace buffer*/
	/* offset will be set to current position of the opened file after read*/
	/* copy_to_user function: source is device_buffer and destination is the userspace buffer *buffer */

	int bytesRead = 0;
	if (*offset >= BUFFER_SIZE){
		bytesRead = 0;
		return bytesRead;
	}
	if (*offset + length > BUFFER_SIZE){
		length = BUFFER_SIZE - *offset;
	}
	printk(KERN_ALERT "Reading from device\n");
	if (copy_to_user(buffer, device_buffer + *offset, length) != 0){
		return -EFAULT;
	}
	copy_to_user(buffer, device_buffer + *offset, length);
	*offset += length;
	printk(KERN_ALERT "Read: %s", buffer);
	printk(KERN_ALERT "%d bytes read\n", bytesRead); 
	return bytesRead;
}



ssize_t pa2_char_driver_write (struct file *pfile, const char __user *buffer, size_t length, loff_t *offset)
{
	/* *buffer is the userspace buffer where you are writing the data you want to be written in the device file*/
	/* length is the length of the userspace buffer*/
	/* current position of the opened file*/
	/* copy_from_user function: destination is device_buffer and source is the userspace buffer *buffer */
    printk(KERN_INFO "Writing to device\n");
    copy_from_user(device_buffer + *offset, buffer, length);
    *offset = strlen(device_buffer);
    printk(KERN_ALERT "%zu bytes written\n", strlen(buffer));
    printk(KERN_ALERT "%s", device_buffer);
    return length;
}


int pa2_char_driver_open (struct inode *pinode, struct file *pfile)
{
	/* print to the log file that the device is opened and also print the number of times this device has been opened until now*/
	printk(KERN_ALERT "Device is open\n");
	o_count++;
	printk(KERN_ALERT "Device has been opened %d times\n", o_count);
	return 0;
}

int pa2_char_driver_close (struct inode *pinode, struct file *pfile)
{
	/* print to the log file that the device is closed and also print the number of times this device has been closed until now*/
	printk(KERN_ALERT "Device is closed\n");
	c_count++;
	printk(KERN_ALERT "Device has been closed %d times\n", c_count);
	return 0;
}

loff_t pa2_char_driver_seek (struct file *pfile, loff_t offset, int whence)
{
	/* Update open file position according to the values of offset and whence */
	loff_t new_pos = 0;
	printk(KERN_ALERT "Begin lseek\n");
	switch(whence){
		case 0:		// SEEK SET
			new_pos = offset;
			break;
		case 1:		// SEEK_CUR
			new_pos = pfile->f_pos + offset;
			break;
		case 2:		// SEEK_END
			new_pos = BUFFER_SIZE - offset;
			break;
	}
	if(new_pos > BUFFER_SIZE)
		new_pos = BUFFER_SIZE;
	if(new_pos < 0)
		new_pos = 0;

	pfile->f_pos = new_pos;
	return new_pos;
}

struct file_operations pa2_char_driver_file_operations = {

	.owner   = THIS_MODULE,
	/* add the function pointers to point to the corresponding file operations. look at the file fs.h in the linux souce code*/
	.open = pa2_char_driver_open,
	.release = pa2_char_driver_close,
	.llseek = pa2_char_driver_seek,
	.read = pa2_char_driver_read,
	.write = pa2_char_driver_write,
};

static int pa2_char_driver_init(void)
{
	/* print to the log file that the init function is called.*/
	/* register the device */
	printk(KERN_ALERT "Within %s function\n", __FUNCTION__);
	//buffer = kmalloc(BUFFER_SIZE, GFP_KERNEL);
	register_chrdev(261, "pa2_char_driver", &pa2_char_driver_file_operations);
	return 0;
}

static void pa2_char_driver_exit(void)
{
	/* print to the log file that the exit function is called.*/
	/* unregister  the device using the register_chrdev() function. */
	printk(KERN_ALERT "Within %s function\n", __FUNCTION__);
	//kfree(buffer);
	unregister_chrdev(261, "pa2_char_driver");
}

/* add module_init and module_exit to point to the corresponding init and exit function*/
module_init(pa2_char_driver_init);
module_exit(pa2_char_driver_exit);
