/* getpir.c : This software is released under GPL
 * For coreboot use only
 * Aug 26 2001 , Nikolai Vladychevski, <niko@isl.net.mx>
 * 2007.04.09 Jeremy Jackson <jerj@coplanar.net>
 *     updated for amd64 and general 64 bit portability
 * 2010.04.24 Marc Bertens <mbertens@xs4all.nl>
 *     Added functionality to read a image file for checking the checksum of the PIR
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

/**
 *	The probe_table() is now called with the pointer to the memory,
 *	this is to handle both the assessing memory and a file.
 *
 *	This function now dumps the table found to the stdout, with
 *	descriptions, it is special handy when building a PIRQ table
 *	for a board to check the checksum.
 */
static struct irq_routing_table *probe_table(char* ptr)
{
	/**
	 * Signature to search for $PIR<2-byte-version>
	 *
	 *	this is to be sure that we find the correct table.
	 */
	char signature[] = "$PIR\x00\x01";
	/** cast the pointer */
	struct irq_routing_table *rt = (struct irq_routing_table *)ptr;
	int size = 16;
	int checksum_result;
	do {
		/** find the PIRQ table */
		rt = (struct irq_routing_table *) memmem(ptr + size, 16, signature, 6);
		if (rt != NULL) {
			/** found the table */
			int i, ts = (rt->size - 32) / 16;
			struct irq_info *se_arr;
			se_arr = (struct irq_info *) ((char *) rt + 32);
			/** Dump the table information to the stdout */
			printf("Found PCI IRQ routing table signature at %p.\n", (void *) ((char *) rt - ptr + 0xf0000));
			printf("SIGNATURE            = %s\n", (char*)&rt->signature);
			printf("VERSION              = %04x\n", rt->version);
			printf("SIZE                 = %i\n", rt->size);
			printf("MAX_DEVICES_ON_BUS   = 32 + 16 * %d\n", ts);
			printf("INT_ROUTER_BUS       = 0x%02x\n", rt->rtr_bus);
			printf("INT_ROUTER DEVICE    = (0x%02x << 3) | 0x%01x\n", rt->rtr_devfn >> 3, rt->rtr_devfn & 7);
			printf("IRQ_DEVOTED_TO_PCI   = %#x\n", rt->exclusive_irqs);
			printf("VENDOR               = %#x\n", rt->rtr_vendor);
			printf("DEVICE               = %#x\n", rt->rtr_device);
			printf("MINIPORT             = %#x\n", rt->miniport_data);
			printf("CHECKSUM             = %#x\n", rt->checksum);
 			printf("\tbus , dev        | fn,   {link, bitmap}, {link, bitmap}, {link, bitmap}, {link, bitmap}, slot, rfu\n");
			for (i = 0; i < ts; i++) {
				printf("\t0x%02x, (0x%02x << 3) | 0x%01x, {{0x%02x, 0x%04x}, {0x%02x, 0x%04x}, {0x%02x, 0x%04x}, {0x%02x, 0x%04x}}, 0x%x, 0x%x},\n",
					(se_arr+i)->bus, 			(se_arr+i)->devfn >> 3,
					(se_arr+i)->devfn & 7, 		(se_arr+i)->irq[0].link,
					(se_arr+i)->irq[0].bitmap, 	(se_arr+i)->irq[1].link,
					(se_arr+i)->irq[1].bitmap, 	(se_arr+i)->irq[2].link,
					(se_arr+i)->irq[2].bitmap, 	(se_arr+i)->irq[3].link,
					(se_arr+i)->irq[3].bitmap, 	(se_arr+i)->slot,
					(se_arr+i)->rfu);
			}
			/** A table should not be over 0x400 bytes */
			if (rt->size > 0x400) {
				return NULL;
			}
			printf("Validating...\n");
			/** Calculate the checksum value */
			checksum_result	= calc_checksum(rt);
			/** Show the calculatedchecksum value */
			printf("CHECKSUM             = %#x\n", 0x100-((checksum_result - rt->checksum) & 0xFF));
			/** and the result of the calculation */
			if (!checksum_result) {
				printf("checksum is ok.\n");
				break;
			} else {
				printf("checksum is wrong.\n");
			}
		}
		size += 16;
	} while (size < 0xFFFF);
	if (size >= 0xFFFF) {
		/** When the functions comes here there is no PIRQ table found. */
		printf("No PCI IRQ routing table signature found.\n");
		return NULL;
	}

	return rt;
}

int main(int argc, char* argv[])
{
	char* ptr;
	int fd_mem = -1;
	struct irq_routing_table* rt = NULL;
	void* bios_image = NULL;
	if ( argc > 1 )
	{
		/** there a paramater passed to the program, assume that it is a menory file */
		printf("Opening memory image file '%s'\n", argv[1]);
		/** Open the file */
		fd_mem = open(argv[1], O_RDONLY);
		if (fd_mem > 0) {
			/** get tyhe size of the file */
			int file_size = lseek(fd_mem, 0, SEEK_END);
			printf("Memory image '%i'\n", file_size);
			/** get a memory block for it. */
			bios_image = malloc(file_size);
			if (bios_image) {
				/** Fill the created buffer */
				lseek(fd_mem, 0, SEEK_SET);
				read(fd_mem, bios_image, file_size);
				/** set the pointer for the probe function */
				ptr = (char*)bios_image;
			} else {
				/* no memory available ? */
				perror("Failed to open imagefile\n");
				return (-3);
			}
		} else {
			/** An error occourd, just exit with a message */
			perror("Failed to open imagefile");
			return (-2);
		}
	} else {
		/** No paramaters means that the program will access the system memory */
		printf("Accessing memory\n");
		if (getuid()) {
			/** i'm not root message !!!! */
			fprintf(stderr, "Run me as root, I need access to " MEM_DEV ".\n");
		}
		/** open the system memory */
		fd_mem = open(MEM_DEV, O_RDONLY);
		if (fd_mem < 0) {
			/** could not open the system memory, exit with a message */
			perror("Could not open " MEM_DEV ":");
			exit(1);
		}
		printf("Probing PIRQ table in memory.\n");
		ptr = mmap(0, 0x10000, PROT_READ, MAP_SHARED, fd_mem, (off_t)0xf0000);
		if (ptr == MAP_FAILED) {
			/** could not map the system memory, exit with a message */
			perror("Mapping system memory failed: ");
			close(fd_mem);
			return (1);
		}
	}
	if (ptr) {
		/** now do the actual probe function */
		rt = probe_table(ptr);
		if (rt != NULL && bios_image == NULL) {
			/** when probing system memory we write the 'irq_tables.c' code file */
			printf("Creating irq_tables.c ...\n");
			code_gen("irq_tables.c", rt);
			printf("Done, you can move the file to the coreboot tree now.\n");
		}
	} else {
		printf("invalid memory pointer\n");
	}
	if (bios_image) {
		/** when we are reading from a file the memory must be released */
		free(bios_image);
	} else {
		/** when reading from the system memory, it must be unmapped */
		munmap(ptr, 0x10000);
	}
	/** Close the file handle */
	close(fd_mem);
	/** return 0 as OK ) */
	return 0;
}
