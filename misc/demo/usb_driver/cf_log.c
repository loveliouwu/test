#include<linux/version.h>
#include<linux/module.h>
#include<linux/kernel.h>







void log_print(const unsigned char *buff, unsigned int len)
{
	int i=0;
	for(i = 0;i<len;i++)
	{
		printk("%02x ",*(buff+i));
		if(i%16 == 15)
			printk("\n");
	}
	printk("\n");
}

