/* getpir.c : This software is released under GPL
   For Linuxbios use only
   Aug 26 2001 , Nikolai Vladychevski, <niko@isl.net.mx>
*/

#include <stdio.h>
#include <sys/mman.h>

#include <arch/pirq_routing.h>

#define O_RDONLY 0x00

static struct irq_routing_table *probe_table(int fd_mem)
{
	char *ptr, signature[] = "$PIR";
	struct irq_routing_table *rt;

	ptr =  mmap(0, 0x10000, PROT_READ, MAP_SHARED,
		    fd_mem, (off_t) 0xf0000);

	rt = (struct irq_routing_table *) memmem(ptr, 0xFFFF, signature, 4);

	if (rt != NULL) {
		printf("Found PCI IRQ Routing table signature at 0x%04x of system memory\n",
		       (char *) rt - ptr + 0xf0000);
	} else {
		printf("No PCI IRQ Routing table signature in the memory\n");		
		exit(1);
	}
	return rt;
}

main()
{
	int fd_mem;
 	struct irq_routing_table *rt;

	if (getuid()) {
		perror("Run me as root, I need access to /dev/mem");
		exit(1);
	}
	fd_mem = open("/dev/mem", O_RDONLY);

	printf("Probing PIRQ table in memory\n");
	rt = probe_table(fd_mem);

	printf("Validating..\n");
	if (!calc_checksum(rt))
		printf("Checksum is ok!\n");

	printf("Creating irq_tables.c .....\n");
	code_gen("irq_tables.c", rt);

	close(fd_mem);

	printf("Done, you can move the file to the LinuxBios tree now.\n");
}
