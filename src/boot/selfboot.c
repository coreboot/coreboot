/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2003 Eric W. Biederman <ebiederm@xmission.com>
 * Copyright (C) 2009 Ron Minnich <rminnich@gmail.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA, 02110-1301 USA
 */

#include <console/console.h>
#include <part/fallback_boot.h>
#include <boot/elf.h>
#include <boot/elf_boot.h>
#include <boot/coreboot_tables.h>
#include <ip_checksum.h>
#include <stream/read_bytes.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <cbfs.h>

#ifndef CONFIG_BIG_ENDIAN
#define ntohl(x) ( ((x&0xff)<<24) | ((x&0xff00)<<8) | \
		((x&0xff0000) >> 8) | ((x&0xff000000) >> 24) )
#else
#define ntohl(x) (x)
#endif

/* Maximum physical address we can use for the coreboot bounce buffer.
 */
#ifndef MAX_ADDR
#define MAX_ADDR -1UL
#endif

extern unsigned char _ram_seg;
extern unsigned char _eram_seg;

struct segment {
	struct segment *next;
	struct segment *prev;
	struct segment *phdr_next;
	struct segment *phdr_prev;
	unsigned long s_dstaddr;
	unsigned long s_srcaddr;
	unsigned long s_memsz;
	unsigned long s_filesz;
	int compression;
};

struct verify_callback {
	struct verify_callback *next;
	int (*callback)(struct verify_callback *vcb, 
		Elf_ehdr *ehdr, Elf_phdr *phdr, struct segment *head);
	unsigned long desc_offset;
	unsigned long desc_addr;
};

struct ip_checksum_vcb {
	struct verify_callback data;
	unsigned short ip_checksum;
};

/* The problem:  
 * Static executables all want to share the same addresses
 * in memory because only a few addresses are reliably present on
 * a machine, and implementing general relocation is hard.
 *
 * The solution:
 * - Allocate a buffer twice the size of the coreboot image.
 * - Anything that would overwrite coreboot copy into the lower half of
 *   the buffer. 
 * - After loading an ELF image copy coreboot to the upper half of the
 *   buffer.
 * - Then jump to the loaded image.
 * 
 * Benefits:
 * - Nearly arbitrary standalone executables can be loaded.
 * - Coreboot is preserved, so it can be returned to.
 * - The implementation is still relatively simple,
 *   and much simpler then the general case implemented in kexec.
 * 
 */

static unsigned long get_bounce_buffer(struct lb_memory *mem)
{
	unsigned long lb_size;
	unsigned long mem_entries;
	unsigned long buffer;
	int i;
	lb_size = (unsigned long)(&_eram_seg - &_ram_seg);
	/* Double coreboot size so I have somewhere to place a copy to return to */
	lb_size = lb_size + lb_size;
	mem_entries = (mem->size - sizeof(*mem))/sizeof(mem->map[0]);
	buffer = 0;
	for(i = 0; i < mem_entries; i++) {
		unsigned long mstart, mend;
		unsigned long msize;
		unsigned long tbuffer;
		if (mem->map[i].type != LB_MEM_RAM)
			continue;
		if (unpack_lb64(mem->map[i].start) > MAX_ADDR)
			continue;
		if (unpack_lb64(mem->map[i].size) < lb_size)
			continue;
		mstart = unpack_lb64(mem->map[i].start);
		msize = MAX_ADDR - mstart +1;
		if (msize > unpack_lb64(mem->map[i].size))
			msize = unpack_lb64(mem->map[i].size);
		mend = mstart + msize;
		tbuffer = mend - lb_size;
		if (tbuffer < buffer) 
			continue;
		buffer = tbuffer;
	}
	return buffer;
}

static int valid_area(struct lb_memory *mem, unsigned long buffer,
	unsigned long start, unsigned long len)
{
	/* Check through all of the memory segments and ensure
	 * the segment that was passed in is completely contained
	 * in RAM.
	 */
	int i;
	unsigned long end = start + len;
	unsigned long mem_entries = (mem->size - sizeof(*mem))/sizeof(mem->map[0]);

	/* See if I conflict with the bounce buffer */
	if (end >= buffer) {
		return 0;
	}

	/* Walk through the table of valid memory ranges and see if I
	 * have a match.
	 */
	for(i = 0; i < mem_entries; i++) {
		uint64_t mstart, mend;
		uint32_t mtype;
		mtype = mem->map[i].type;
		mstart = unpack_lb64(mem->map[i].start);
		mend = mstart + unpack_lb64(mem->map[i].size);
		if ((mtype == LB_MEM_RAM) && (start < mend) && (end > mstart)) {
			break;
		}
		if ((mtype == LB_MEM_TABLE) && (start < mend) && (end > mstart)) {
			printk_err("Payload is overwriting Coreboot tables.\n");
			break;
		}
	}
	if (i == mem_entries) {
		printk_err("No matching ram area found for range:\n");
		printk_err("  [0x%016lx, 0x%016lx)\n", start, end);
		printk_err("Ram areas\n");
		for(i = 0; i < mem_entries; i++) {
			uint64_t mstart, mend;
			uint32_t mtype;
			mtype = mem->map[i].type;
			mstart = unpack_lb64(mem->map[i].start);
			mend = mstart + unpack_lb64(mem->map[i].size);
			printk_err("  [0x%016lx, 0x%016lx) %s\n",
				(unsigned long)mstart, 
				(unsigned long)mend, 
				(mtype == LB_MEM_RAM)?"RAM":"Reserved");
			
		}
		return 0;
	}
	return 1;
}

static void relocate_segment(unsigned long buffer, struct segment *seg)
{
	/* Modify all segments that want to load onto coreboot
	 * to load onto the bounce buffer instead.
	 */
	unsigned long lb_start = (unsigned long)&_ram_seg;
	unsigned long lb_end = (unsigned long)&_eram_seg;
	unsigned long start, middle, end;

	printk_spew("lb: [0x%016lx, 0x%016lx)\n", 
		lb_start, lb_end);

	start = seg->s_dstaddr;
	middle = start + seg->s_filesz;
	end = start + seg->s_memsz;
	/* I don't conflict with coreboot so get out of here */
	if ((end <= lb_start) || (start >= lb_end))
		return;

	printk_spew("segment: [0x%016lx, 0x%016lx, 0x%016lx)\n", 
		start, middle, end);

	if (seg->compression == CBFS_COMPRESS_NONE) {
		/* Slice off a piece at the beginning
		 * that doesn't conflict with coreboot.
		 */
		if (start < lb_start) {
			struct segment *new;
			unsigned long len = lb_start - start;
			new = malloc(sizeof(*new));
			*new = *seg;
			new->s_memsz = len;
			seg->s_memsz -= len;
			seg->s_dstaddr += len;
			seg->s_srcaddr += len;
			if (seg->s_filesz > len) {
				new->s_filesz = len;
				seg->s_filesz -= len;
			} else {
				seg->s_filesz = 0;
			}

			/* Order by stream offset */
			new->next = seg;
			new->prev = seg->prev;
			seg->prev->next = new;
			seg->prev = new;
			/* Order by original program header order */
			new->phdr_next = seg;
			new->phdr_prev = seg->phdr_prev;
			seg->phdr_prev->phdr_next = new;
			seg->phdr_prev = new;

			/* compute the new value of start */
			start = seg->s_dstaddr;
			
			printk_spew("   early: [0x%016lx, 0x%016lx, 0x%016lx)\n", 
				new->s_dstaddr, 
				new->s_dstaddr + new->s_filesz,
				new->s_dstaddr + new->s_memsz);
			}
			
			/* Slice off a piece at the end 
		 * that doesn't conflict with coreboot 
		 */
		if (end > lb_end) {
			unsigned long len = lb_end - start;
			struct segment *new;
			new = malloc(sizeof(*new));
			*new = *seg;
			seg->s_memsz = len;
			new->s_memsz -= len;
			new->s_dstaddr += len;
			new->s_srcaddr += len;
			if (seg->s_filesz > len) {
				seg->s_filesz = len;
				new->s_filesz -= len;
			} else {
				new->s_filesz = 0;
			}
			/* Order by stream offset */
			new->next = seg->next;
			new->prev = seg;
			seg->next->prev = new;
			seg->next = new;
			/* Order by original program header order */
			new->phdr_next = seg->phdr_next;
			new->phdr_prev = seg;
			seg->phdr_next->phdr_prev = new;
			seg->phdr_next = new;

			/* compute the new value of end */
			end = start + len;
			
			printk_spew("   late: [0x%016lx, 0x%016lx, 0x%016lx)\n", 
				new->s_dstaddr, 
				new->s_dstaddr + new->s_filesz,
				new->s_dstaddr + new->s_memsz);
			
		}
	}
	/* Now retarget this segment onto the bounce buffer */
	/* sort of explanation: the buffer is a 1:1 mapping to coreboot. 
	 * so you will make the dstaddr be this buffer, and it will get copied
	 * later to where coreboot lives.
	 */
	seg->s_dstaddr = buffer + (seg->s_dstaddr - lb_start);

	printk_spew(" bounce: [0x%016lx, 0x%016lx, 0x%016lx)\n", 
		seg->s_dstaddr, 
		seg->s_dstaddr + seg->s_filesz, 
		seg->s_dstaddr + seg->s_memsz);
}


static int build_self_segment_list(
	struct segment *head, 
	unsigned long bounce_buffer, struct lb_memory *mem,
	struct cbfs_payload *payload, u32 *entry)
{
	struct segment *new;
	struct segment *ptr;
	int datasize;
	struct cbfs_payload_segment *segment, *first_segment;
	memset(head, 0, sizeof(*head));
	head->phdr_next = head->phdr_prev = head;
	head->next = head->prev = head;
	first_segment = segment = &payload->segments;

	while(1) {
		printk_debug("Segment %p\n", segment);
		switch(segment->type) {
		default: printk_emerg("Bad segment type %x\n", segment->type);
			return -1;
		case PAYLOAD_SEGMENT_PARAMS:
			printk_info("found param section\n");
			segment++;
			continue;
		case PAYLOAD_SEGMENT_CODE:
		case PAYLOAD_SEGMENT_DATA:
			printk_info( "%s: ", segment->type == PAYLOAD_SEGMENT_CODE ? 
				"code" : "data");
		new = malloc(sizeof(*new));
		new->s_dstaddr = ntohl((u32) segment->load_addr);
		new->s_memsz = ntohl(segment->mem_len);
		new->compression = ntohl(segment->compression);

		datasize = ntohl(segment->len);
		new->s_srcaddr = (u32) ((unsigned char *) first_segment) + ntohl(segment->offset);
		new->s_filesz = ntohl(segment->len);
		printk_debug("New segment dstaddr 0x%lx memsize 0x%lx srcaddr 0x%lx filesize 0x%lx\n",
			new->s_dstaddr, new->s_memsz, new->s_srcaddr, new->s_filesz);
		/* Clean up the values */
		if (new->s_filesz > new->s_memsz)  {
			new->s_filesz = new->s_memsz;
		}
		printk_debug("(cleaned up) New segment addr 0x%lx size 0x%lx offset 0x%lx filesize 0x%lx\n",
			new->s_dstaddr, new->s_memsz, new->s_srcaddr, new->s_filesz);
		break;
		case PAYLOAD_SEGMENT_BSS:
			printk_info("BSS %p/%d\n", (void *) ntohl((u32) segment->load_addr),
				 ntohl(segment->mem_len));
			new = malloc(sizeof(*new));
			new->s_filesz = 0;
			new->s_dstaddr = ntohl((u32) segment->load_addr);
			new->s_memsz = ntohl(segment->mem_len);

			break;

		case PAYLOAD_SEGMENT_ENTRY:
			printk_info("Entry %p\n", (void *) ntohl((u32) segment->load_addr));
			*entry =  ntohl((u32) segment->load_addr);
			return 1;
		}
		segment++;
		for(ptr = head->next; ptr != head; ptr = ptr->next) {
			if (new->s_srcaddr < ntohl((u32) segment->load_addr))
				break;
		}
		/* Order by stream offset */
		new->next = ptr;
		new->prev = ptr->prev;
		ptr->prev->next = new;
		ptr->prev = new;
		/* Order by original program header order */
		new->phdr_next = head;
		new->phdr_prev = head->phdr_prev;
		head->phdr_prev->phdr_next  = new;
		head->phdr_prev = new;

		/* Verify the memory addresses in the segment are valid */
		if (!valid_area(mem, bounce_buffer, new->s_dstaddr, new->s_memsz)) 
			goto out;

		/* Modify the segment to load onto the bounce_buffer if necessary.
		 */
		relocate_segment(bounce_buffer, new);
	}
	return 1;
 out:
	return 0;
}

static int load_self_segments(
	struct segment *head, struct cbfs_payload *payload)
{
	unsigned long offset;
	struct segment *ptr;
	
	offset = 0;
	for(ptr = head->next; ptr != head; ptr = ptr->next) {
		unsigned char *dest, *middle, *end, *src;
		printk_debug("Loading Segment: addr: 0x%016lx memsz: 0x%016lx filesz: 0x%016lx\n",
			ptr->s_dstaddr, ptr->s_memsz, ptr->s_filesz);
		
		/* Compute the boundaries of the segment */
		dest = (unsigned char *)(ptr->s_dstaddr);
		src = (unsigned char *)(ptr->s_srcaddr);
		
		/* Copy data from the initial buffer */
		if (ptr->s_filesz) {
			size_t len;
			len = ptr->s_filesz;
			switch(ptr->compression) {
#if CONFIG_COMPRESSED_PAYLOAD_LZMA==1
				case CBFS_COMPRESS_LZMA: {
					printk_debug("using LZMA\n");
					unsigned long ulzma(unsigned char *src, unsigned char *dst);		
					len = ulzma(src, dest);
					break;
				}
#endif
#if CONFIG_COMPRESSED_PAYLOAD_NRV2B==1
				case CBFS_COMPRESS_NRV2B: {
					printk_debug("using NRV2B\n");
					unsigned long unrv2b(u8 *src, u8 *dst, unsigned long *ilen_p);
					unsigned long tmp;
					len = unrv2b(src, dest, &tmp);
					break;
				}
#endif
				case CBFS_COMPRESS_NONE: {
					printk_debug("it's not compressed!\n");
					memcpy(dest, src, len);
					break;
				}
				default:
					printk_info( "CBFS:  Unknown compression type %d\n", ptr->compression);
					return -1;
			}
			end = dest + ptr->s_memsz;
			middle = dest + len;
			printk_spew("[ 0x%016lx, %016lx, 0x%016lx) <- %016lx\n",
				(unsigned long)dest,
				(unsigned long)middle,
				(unsigned long)end,
				(unsigned long)src);
		}
		/* Zero the extra bytes between middle & end */
		if (middle < end) {
			printk_debug("Clearing Segment: addr: 0x%016lx memsz: 0x%016lx\n",
				(unsigned long)middle, (unsigned long)(end - middle));
			
			/* Zero the extra bytes */
			memset(middle, 0, end - middle);
		}
	}
	return 1;
}

int selfboot(struct lb_memory *mem, struct cbfs_payload *payload)
{
	u32 entry=0;
	struct segment head;
	unsigned long bounce_buffer;

	/* Find a bounce buffer so I can load to coreboot's current location */
	bounce_buffer = get_bounce_buffer(mem);
	if (!bounce_buffer) {
		printk_err("Could not find a bounce buffer...\n");
		goto out;
	}

	/* Preprocess the self segments */
	if (!build_self_segment_list(&head, bounce_buffer, mem, payload, &entry))
		goto out;

	/* Load the segments */
	if (!load_self_segments(&head, payload))
		goto out;

	printk_spew("Loaded segments\n");

	/* Reset to booting from this image as late as possible */
	boot_successful();

	printk_debug("Jumping to boot code at %x\n", entry);
	post_code(0xfe);

	/* Jump to kernel */
	jmp_to_elf_entry((void*)entry, bounce_buffer);
	return 1;

 out:
	return 0;
}

