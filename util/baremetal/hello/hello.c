#include <printk.h>


char buffer[512];

int main(void)
{
	int i;

	printk("Hello World!\n");

	ide_init();

	ide_read_sector(0, buffer, 0, 0, 512);

	printk("%s", buffer+4);

	printk("\nBYE!\n");




	return(0);
}
