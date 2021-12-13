#include <linux/uaccess.h>
#include <linux/kernel.h>
#include <linux/string.h>
#include <linux/errno.h>
#include <linux/kdev_t.h>
#include <linux/device.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/types.h>
#include <linux/cdev.h>
#define STRING_SIZE 101

MODULE_LICENSE("Dual BSD/GPL");

char string[STRING_SIZE];
int endRead = 0;
int duzina = 0;

dev_t my_dev_id;
static struct class *my_class;
static struct device *my_device;
static struct cdev *my_cdev;

int stred_open(struct inode *pinode, struct file *pfile);
int stred_close(struct inode *pinode, struct file *pfile);
ssize_t stred_read(struct file *pfile, char __user *buffer, size_t length, loff_t *offset);
ssize_t stred_write(struct file *pfile, const char __user *buffer, size_t length, loff_t *offset);

struct file_operations my_fops =
{
	.owner = THIS_MODULE,
	.open = stred_open,
	.read = stred_read,
	.write = stred_write,
	.release = stred_close,
};


int stred_open(struct inode *pinode, struct file *pfile)
{

	printk(KERN_INFO "Succesfully opened file\n");

	return 0;
}  

int stred_close(struct inode *pinode, struct file *pfile)
{
	printk(KERN_INFO "Succesfully closed file\n");
	return 0;
}

ssize_t stred_read(struct file *pfile, char __user *buffer, size_t length, loff_t *offset)
{
	int ret;
	char buff[STRING_SIZE];
	long int len;
	
	if (endRead){
		endRead = 0;
		printk(KERN_INFO "Succesfully read from file\n");
		return 0;
	}
	
	len = scnprintf(buff, STRING_SIZE, "String u buferu: %s\n" , string);
	ret = copy_to_user(buffer, buff, len);
	if(ret)
		return -EFAULT;

	endRead = 1;

	return len;
}

ssize_t stred_write(struct file *pfile, const char __user *buffer, size_t length, loff_t *offset)
{
	char buff[STRING_SIZE];
	int ret;

	ret = copy_from_user(buff, buffer, length);
	if(ret)
		return -EFAULT;

	buff[length-1] = '\0';

	if (strstr(buff, "string=") == buff)
	{
		char* reci = buff + 7; //da ne bi upisivao i string=
		int i;
		int len;

		for (i=0; i<101; i++)
		{
			string[i] = 0;
		}
		
		len = strlen(reci);
		
		if(len < 101)
		{
			strncpy(string, reci, len);
			printk(KERN_INFO "Upisan string u bafer: %s\n", string);
		}
		else
		{	
			printk(KERN_ERR "Preko 100 karaktera ne moze se uneti");
		}

		duzina = strlen(string);

	}
	else if(strstr(buff, "clear") == buff)
	{
		int i;

		for (i=0; i<101; i++)
		{
			string[i] = 0;
		}

		printk(KERN_INFO "Uspesno obrisan string iz bufera");


		duzina = strlen(string);

	}
	else if(strstr(buff, "shrink") == buff)
	{
		char* novi = string;

		while(string[0] == ' ')
		{

			novi = string + 1; //pocinje novi niz karaktera od jednog karaktera napred
			strncpy(string, novi, strlen(novi));
		}
		
		while(string[strlen(string)-1] == ' ')
		{
			string[strlen(string)-1] = 0; //terminiran pretposlednji karakter

		}	
		
		printk(KERN_INFO "Novi niz je: %s\n", string);
		
		duzina = strlen(string);
	}
	else if(strstr(buff, "append=") == buff)
	{
		char* dodatak = buff + 7;
		int len = strlen(dodatak);

		if((duzina + len) > 100)
		{
			printk(KERN_WARNING "Prepun bafer\nNije dodat string na postojeci\n");
		}	
		else
		{
			strcat(string, dodatak);
			printk(KERN_INFO "Novi string je: %s\n", string);
		}

	}
	else if(strstr(buff, "truncate=") == buff)
	{
		char* novi = buff + 9;
		int broj;

		ret = sscanf(novi, "%d", &broj);

		if(ret == 1) //samo jedan broj parsiran
		{
			int i;

			for(i=0; i < broj; i++)
			{
				string[duzina - 1] = 0;
				duzina = strlen(string);
			}

			printk(KERN_INFO "Novi string je: %s", string);
		}
		else
		{
			printk(KERN_WARNING "Samo jedan broj uneti\n"); 
		}
	}
	else if(strstr(buff, "remove=") == buff)
	{
		char* rec = buff + 7;
		int len = strlen(rec);
		
		char* novi = strstr(string, rec);
		int i;
		int len1,len2;
		int pozicija;
		
		if(novi)
		{
			while(novi)
			{
				len1 = strlen(string);
				len2 = strlen(novi);
				
				pozicija = len1 - len2; //na ovaj nacin dobijam poziciju u string-u gde se nalazi rec

				for(i=0; i < len2; i++)
				{
					string[pozicija + i] = string[pozicija + i + len];
				}

				for(i=0; i < len; i++)
				{
					string[len1-1] = 0;
				}

				novi = strstr(string, rec); //while se ponavlja sgve dok nema rec u stringu
			}

			printk(KERN_INFO "Uspesno obrisano\nNovi string je: %s\n", string);

			duzina = strlen(string);
		}
		else
		{
			printk(KERN_WARNING "Ne postoji ovaj niz karaktera u stringu\n");
		}

	}
	else
       	{
		printk(KERN_WARNING "Pogresno napisana komanda\n");
	}

	return length;
}

static int __init stred_init(void)
{
	int ret = 0;
       	ret = alloc_chrdev_region(&my_dev_id, 0, 1, "stred");
	if (ret){
	       printk(KERN_ERR "failed to register char device\n");
	       return ret;
 	}
 	printk(KERN_INFO "char device region allocated\n");

	my_class = class_create(THIS_MODULE, "stred_class");
	if (my_class == NULL){
	  	printk(KERN_ERR "failed to create class\n");
		goto fail_0;
	}
	printk(KERN_INFO "class created\n");

	my_device = device_create(my_class, NULL, my_dev_id, NULL, "stred");
       	if (my_device == NULL){
 		printk(KERN_ERR "failed to create device\n");
		goto fail_1;
	}
	printk(KERN_INFO "device created\n");
	
	my_cdev = cdev_alloc();
	my_cdev->ops = &my_fops;
	my_cdev->owner = THIS_MODULE;
	ret = cdev_add(my_cdev, my_dev_id, 1);
	if (ret)
	{
		printk(KERN_ERR "failed to add cdev\n");
		goto fail_2;
	}
	printk(KERN_INFO "cdev added\n");
	printk(KERN_INFO "Hello world\n");
	return 0;

	fail_2:
		device_destroy(my_class, my_dev_id);
	fail_1:
		class_destroy(my_class);
	fail_0:
		unregister_chrdev_region(my_dev_id, 1);
	return -1;
}

static void __exit stred_exit(void)
{
	cdev_del(my_cdev);
	device_destroy(my_class, my_dev_id);
	class_destroy(my_class);
	unregister_chrdev_region(my_dev_id,1);
	printk(KERN_INFO "Goodbye, cruel world\n");
}


module_init(stred_init);
module_exit(stred_exit);
