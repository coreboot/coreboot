#include <printk.h>
#include <stdlib.h>
#include <subr.h>
#include <stddef.h>
#include <rom/read_bytes.h>
//#include <delay.h>

#include <string.h>
//#include <floppy_subr.h>
#include "uhci.h"


typedef struct block_device {
	void (*shutdown)(void);
	int (*read)(int drive, void *buffer, unsigned int block);
	int blocks;
	int block_size;
} block_device_t;

typedef struct partition_entry {
	uchar boot_flag;

	uchar chs[7];

	unsigned int lba_start;
	unsigned int lba_len;
} __attribute__ ((packed)) partition_entry_t;

typedef struct partition {
	char loader[446];
	partition_entry_t entry[4];
	char sig[2];
} __attribute__ ((packed)) partition_t;

unsigned char usb_device_address=0;

/* read a sector or a partial sector */
extern int ll_read_block(unsigned char device, void *buffer, unsigned int block, unsigned int count);

extern int block_read_sector(int drive, void * buffer, unsigned int block, int byte_offset, 
	int n_bytes);

static unsigned long offset;
static unsigned long partition_start;
static unsigned long partition_end;

static int init_bytes(void)
{
	int i,res;
	int error_count=100;
	partition_t part;
	unsigned char sense_data[32];

	// find first usb device

	while(error_count && (res = poll_usb())) 	// keep polling usb until no more devices are enumerated
		if(res<0)
			if(!--error_count) 
				printk("There is a USB device, but it won't init! This is a bad thing.\n");

	for(i=0; i< next_usb_dev ; i++) {
		if(usb_device[i].class == 0x08 && usb_device[i].subclass == 0x06 && usb_device[i].protocol == 0x50) {
			printk("Found USB block device %u\n", i);
			usb_device_address = i;
			break;
		}
	}

	if(!usb_device_address) {
		printk("Massive failure, no suitable USB device!\n");
		return(-1);
	}


	printk("Requesting initial sense data\n");
	request_sense( usb_device_address, sense_data, 32);
	PrintSense(sense_data, 32);

//	get_capacity(addr, &block_count, &block_len);
//	printk("%u %u byte blocks\n", block_count, block_len);

	res = ll_read_block(usb_device_address, &part, 0, 1);

	printk("ll_read_block returns %d\n", res);

	res=-1;

	for(i=0; i<4; i++) {
		printk("%u: boot=%02x, start=%08x length=%08x\n",i,  part.entry[i].boot_flag, part.entry[i].lba_start, part.entry[i].lba_len);
		if(part.entry[i].boot_flag & 0x80) {
			printk("Using partition %u\n", i);
			partition_start = part.entry[i].lba_start;
			partition_end = partition_start + part.entry[i].lba_len;
			res=i;
			break;
		}
	}

	return res;
}

static void fini_bytes(void)
{
	return;
}

#define CHUNK_SIZE 16384
#define BLOCK_PER_CHUNK CHUNK_SIZE/512

static unsigned char buffer[CHUNK_SIZE];
static unsigned int block_num = 0;
static unsigned int first_fill = 1;

static byte_offset_t usb_read(void *vdest, byte_offset_t offset, byte_offset_t count)
{
	byte_offset_t bytes = 0;
	unsigned char *dest = vdest;

	//printk_debug("ide_read count = %x\n", count);
	while (bytes < count) {
		unsigned int byte_offset, len;
		int result;
		int i, j;

		/* The block is not cached in memory or frist time called */
		if (block_num != offset / CHUNK_SIZE || first_fill) {
			block_num  = offset / CHUNK_SIZE;
			printk_notice (".");
			ll_read_block(usb_device_address, buffer, partition_start + block_num*BLOCK_PER_CHUNK, BLOCK_PER_CHUNK);
			first_fill = 0;
#if 0
			//printk_debug("ide_read offset = %x\n", offset);
			//printk_debug("ide_read block_num = %x\n", block_num);
			for (i = 0; i < 16; i++) {
			    for (j = 0; j < 16; j++) {
				printk_debug("%02x ", buffer[i*16 +j]);
			    }
			    printk_debug("\n");
			}

			printk_debug("\n");
#endif
		}

		byte_offset = offset % CHUNK_SIZE;
		len = CHUNK_SIZE - byte_offset;
		if (len > (count - bytes)) {
			len = (count - bytes);
		}

		memcpy(dest, buffer + byte_offset, len);

		offset += len;
		bytes += len;
		dest += len;

	}
	return bytes;
}

static byte_offset_t read_bytes(void *vdest, byte_offset_t count)
{
	byte_offset_t len;

	len = usb_read(vdest, offset, count);
	if (len > 0) {
		offset += len;
	}

	return len;
}

static byte_offset_t skip_bytes(byte_offset_t count)
{
	offset += count;
	return count;
}

struct stream usb_stream __stream = {
	.init = init_bytes,
	.read = read_bytes,	       
	.skip = skip_bytes,
	.fini = fini_bytes,
};
