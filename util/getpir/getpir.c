/* getpir.c : This software is released under GPL
 * For coreboot use only
 * Aug 26 2001 , Nikolai Vladychevski, <niko@isl.net.mx>
 * 2007.04.09 Jeremy Jackson <jerj@coplanar.net>
 *     updated for amd64 and general 64 bit portability
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "pirq_routing.h"
#include "checksum.h"
#include "code_gen.h"

#if defined (__sun) && (defined(__i386) || defined(__amd64))
#  define MEM_DEV "/dev/xsvc"
#else
#  define MEM_DEV "/dev/mem"
#endif

static struct irq_routing_table *probe_table(int fd_mem)
{
	char *ptr, signature[] = "$PIR";
	struct irq_routing_table *rt;
	int size = 16;

	ptr = mmap(0, 0x10000, PROT_READ, MAP_SHARED,
		   fd_mem, (off_t) 0xf0000);

	if (ptr == MAP_FAILED) {
		perror("Mapping system memory failed: ");
		return NULL;
	}

	do {
		rt = (struct irq_routing_table *) memmem(ptr + size, 16, signature, 4);
		if (rt != NULL) {
			printf("Found PCI IRQ routing table signature at %p.\n",
			     (void *) ((char *) rt - ptr + 0xf0000));
			printf("Validating... ");
			if (!calc_checksum(rt)) {
				printf("checksum is ok.\n");
				break;
			} else {
				printf("checksum is wrong.\n");
			}
		}
		size += 16;
	} while (size < 0xFFFF);

	if (size >= 0xFFFF) {
		printf("No PCI IRQ routing table signature found.\n");
		munmap(ptr, 0x10000);
		return NULL;
	}

	return rt;
}

int main(void)
{
	int fd_mem;
	struct irq_routing_table *rt;

	if (getuid()) {
		fprintf(stderr, "Run me as root, I need access to " MEM_DEV ".\n");
	}

	fd_mem = open(MEM_DEV, O_RDONLY);
	if (fd_mem < 0) {
		perror("Could not open " MEM_DEV ":");
		exit(1);
	}

	printf("Probing PIRQ table in memory.\n");
	rt = probe_table(fd_mem);
	if (rt != NULL) {
		printf("Creating irq_tables.c ...\n");
		code_gen("irq_tables.c", rt);
		printf
		    ("Done, you can move the file to the coreboot tree now.\n");
	}
	close(fd_mem);


	return 0;
}
