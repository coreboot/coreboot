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
 */

#include <console/console.h>
#include <cpu/cpu.h>
#include <endian.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <symbols.h>
#include <cbfs.h>
#include <lib.h>
#include <bootmem.h>
#include <program_loading.h>

static const unsigned long lb_start = (unsigned long)&_program;
static const unsigned long lb_end = (unsigned long)&_eprogram;

struct segment {
	struct segment *next;
	struct segment *prev;
	unsigned long s_dstaddr;
	unsigned long s_srcaddr;
	unsigned long s_memsz;
	unsigned long s_filesz;
	int compression;
};

/* The problem:
 * Static executables all want to share the same addresses
 * in memory because only a few addresses are reliably present on
 * a machine, and implementing general relocation is hard.
 *
 * The solution:
 * - Allocate a buffer the size of the coreboot image plus additional
 *   required space.
 * - Anything that would overwrite coreboot copy into the lower part of
 *   the buffer.
 * - After loading an ELF image copy coreboot to the top of the buffer.
 * - Then jump to the loaded image.
 *
 * Benefits:
 * - Nearly arbitrary standalone executables can be loaded.
 * - Coreboot is preserved, so it can be returned to.
 * - The implementation is still relatively simple,
 *   and much simpler than the general case implemented in kexec.
 */

static unsigned long bounce_size, bounce_buffer;

static void get_bounce_buffer(unsigned long req_size)
{
	unsigned long lb_size;
	void *buffer;

	/* When the ramstage is relocatable there is no need for a bounce
	 * buffer. All payloads should not overlap the ramstage.
	 */
	if (IS_ENABLED(CONFIG_RELOCATABLE_RAMSTAGE)) {
		bounce_buffer = ~0UL;
		bounce_size = 0;
		return;
	}

	lb_size = lb_end - lb_start;
	/* Plus coreboot size so I have somewhere
	 * to place a copy to return to.
	 */
	lb_size = req_size + lb_size;

	buffer = bootmem_allocate_buffer(lb_size);

	printk(BIOS_SPEW, "Bounce Buffer at %p, %lu bytes\n", buffer, lb_size);

	bounce_buffer = (uintptr_t)buffer;
	bounce_size = req_size;
}

static int overlaps_coreboot(struct segment *seg)
{
	unsigned long start, end;
	start = seg->s_dstaddr;
	end = start + seg->s_memsz;
	return !((end <= lb_start) || (start >= lb_end));
}

static int relocate_segment(unsigned long buffer, struct segment *seg)
{
	/* Modify all segments that want to load onto coreboot
	 * to load onto the bounce buffer instead.
	 */
	/* ret:  1 : A new segment is inserted before the seg.
	 *       0 : A new segment is inserted after the seg, or no new one.
	 */
	unsigned long start, middle, end, ret = 0;

	printk(BIOS_SPEW, "lb: [0x%016lx, 0x%016lx)\n",
		lb_start, lb_end);

	/* I don't conflict with coreboot so get out of here */
	if (!overlaps_coreboot(seg))
		return 0;

	start = seg->s_dstaddr;
	middle = start + seg->s_filesz;
	end = start + seg->s_memsz;

	printk(BIOS_SPEW, "segment: [0x%016lx, 0x%016lx, 0x%016lx)\n",
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

			/* compute the new value of start */
			start = seg->s_dstaddr;

			printk(BIOS_SPEW, "   early: [0x%016lx, 0x%016lx, 0x%016lx)\n",
				new->s_dstaddr,
				new->s_dstaddr + new->s_filesz,
				new->s_dstaddr + new->s_memsz);

			ret = 1;
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

			printk(BIOS_SPEW, "   late: [0x%016lx, 0x%016lx, 0x%016lx)\n",
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

	printk(BIOS_SPEW, " bounce: [0x%016lx, 0x%016lx, 0x%016lx)\n",
		seg->s_dstaddr,
		seg->s_dstaddr + seg->s_filesz,
		seg->s_dstaddr + seg->s_memsz);

	return ret;
}


static int build_self_segment_list(
	struct segment *head,
	struct cbfs_payload *cbfs_payload, uintptr_t *entry)
{
	struct segment *new;
	struct segment *ptr;
	struct cbfs_payload_segment *segment, *first_segment;
	memset(head, 0, sizeof(*head));
	head->next = head->prev = head;
	first_segment = segment = &cbfs_payload->segments;

	while(1) {
		printk(BIOS_DEBUG, "Loading segment from rom address 0x%p\n", segment);
		switch(segment->type) {
		case PAYLOAD_SEGMENT_PARAMS:
			printk(BIOS_DEBUG, "  parameter section (skipped)\n");
			segment++;
			continue;

		case PAYLOAD_SEGMENT_CODE:
		case PAYLOAD_SEGMENT_DATA:
			printk(BIOS_DEBUG, "  %s (compression=%x)\n",
					segment->type == PAYLOAD_SEGMENT_CODE ?  "code" : "data",
					ntohl(segment->compression));
			new = malloc(sizeof(*new));
			new->s_dstaddr = ntohll(segment->load_addr);
			new->s_memsz = ntohl(segment->mem_len);
			new->compression = ntohl(segment->compression);

			new->s_srcaddr = (uintptr_t)
				((unsigned char *)first_segment)
				+ ntohl(segment->offset);
			new->s_filesz = ntohl(segment->len);
			printk(BIOS_DEBUG, "  New segment dstaddr 0x%lx memsize 0x%lx srcaddr 0x%lx filesize 0x%lx\n",
				new->s_dstaddr, new->s_memsz, new->s_srcaddr, new->s_filesz);
			/* Clean up the values */
			if (new->s_filesz > new->s_memsz)  {
				new->s_filesz = new->s_memsz;
				printk(BIOS_DEBUG,
				       "  cleaned up filesize 0x%lx\n",
				       new->s_filesz);
			}
			break;

		case PAYLOAD_SEGMENT_BSS:
			printk(BIOS_DEBUG, "  BSS 0x%p (%d byte)\n", (void *)
					(intptr_t)ntohll(segment->load_addr),
				 	ntohl(segment->mem_len));
			new = malloc(sizeof(*new));
			new->s_filesz = 0;
			new->s_srcaddr = (uintptr_t)
				((unsigned char *)first_segment)
				+ ntohl(segment->offset);
			new->s_dstaddr = ntohll(segment->load_addr);
			new->s_memsz = ntohl(segment->mem_len);
			break;

		case PAYLOAD_SEGMENT_ENTRY:
			printk(BIOS_DEBUG, "  Entry Point 0x%p\n",
			       (void *)(intptr_t)ntohll(segment->load_addr));
			*entry =  ntohll(segment->load_addr);
			/* Per definition, a payload always has the entry point
			 * as last segment. Thus, we use the occurrence of the
			 * entry point as break condition for the loop.
			 * Can we actually just look at the number of section?
			 */
			return 1;

		default:
			/* We found something that we don't know about. Throw
			 * hands into the sky and run away!
			 */
			printk(BIOS_EMERG, "Bad segment type %x\n", segment->type);
			return -1;
		}

		/* We have found another CODE, DATA or BSS segment */
		segment++;

		/* Find place where to insert our segment */
		for(ptr = head->next; ptr != head; ptr = ptr->next) {
			if (new->s_srcaddr < ntohll(segment->load_addr))
				break;
		}

		/* Order by stream offset */
		new->next = ptr;
		new->prev = ptr->prev;
		ptr->prev->next = new;
		ptr->prev = new;
	}

	return 1;
}

static int load_self_segments(
	struct segment *head,
	struct prog *payload)
{
	struct segment *ptr;
	struct segment *last_non_empty;
	const unsigned long one_meg = (1UL << 20);
	unsigned long bounce_high = lb_end;

	/* Determine last non-empty loaded segment. */
	last_non_empty = NULL;
	for(ptr = head->next; ptr != head; ptr = ptr->next)
		if (ptr->s_filesz != 0)
			last_non_empty = ptr;

	for(ptr = head->next; ptr != head; ptr = ptr->next) {
		if (bootmem_region_targets_usable_ram(ptr->s_dstaddr,
							ptr->s_memsz))
			continue;

		if (ptr->s_dstaddr < one_meg &&
		    (ptr->s_dstaddr + ptr->s_memsz) <= one_meg) {
			printk(BIOS_DEBUG,
				"Payload being loaded below 1MiB "
				"without region being marked as RAM usable.\n");
			continue;
		}

		/* Payload segment not targeting RAM. */
		printk(BIOS_ERR, "SELF Payload doesn't target RAM:\n");
		printk(BIOS_ERR, "Failed Segment: 0x%lx, %lu bytes\n",
			ptr->s_dstaddr, ptr->s_memsz);
		bootmem_dump_ranges();
		return 0;
	}

	for(ptr = head->next; ptr != head; ptr = ptr->next) {
		/*
		 * Add segments to bootmem memory map before a bounce buffer is
		 * allocated so that there aren't conflicts with the actual
		 * payload.
		 */
		bootmem_add_range(ptr->s_dstaddr, ptr->s_memsz,
					LB_MEM_UNUSABLE);

		if (!overlaps_coreboot(ptr))
			continue;
		if (ptr->s_dstaddr + ptr->s_memsz > bounce_high)
			bounce_high = ptr->s_dstaddr + ptr->s_memsz;
	}
	get_bounce_buffer(bounce_high - lb_start);
	if (!bounce_buffer) {
		printk(BIOS_ERR, "Could not find a bounce buffer...\n");
		return 0;
	}

	for(ptr = head->next; ptr != head; ptr = ptr->next) {
		unsigned char *dest, *src;
		printk(BIOS_DEBUG, "Loading Segment: addr: 0x%016lx memsz: 0x%016lx filesz: 0x%016lx\n",
			ptr->s_dstaddr, ptr->s_memsz, ptr->s_filesz);

		/* Modify the segment to load onto the bounce_buffer if necessary.
		 */
		if (relocate_segment(bounce_buffer, ptr)) {
			ptr = (ptr->prev)->prev;
			continue;
		}

		printk(BIOS_DEBUG, "Post relocation: addr: 0x%016lx memsz: 0x%016lx filesz: 0x%016lx\n",
			ptr->s_dstaddr, ptr->s_memsz, ptr->s_filesz);

		/* Compute the boundaries of the segment */
		dest = (unsigned char *)(ptr->s_dstaddr);
		src = (unsigned char *)(ptr->s_srcaddr);

		/* Copy data from the initial buffer */
		if (ptr->s_filesz) {
			unsigned char *middle, *end;
			size_t len;
			len = ptr->s_filesz;
			switch(ptr->compression) {
				case CBFS_COMPRESS_LZMA: {
					printk(BIOS_DEBUG, "using LZMA\n");
					len = ulzma(src, dest);
					if (!len) /* Decompression Error. */
						return 0;
					break;
				}
				case CBFS_COMPRESS_NONE: {
					printk(BIOS_DEBUG, "it's not compressed!\n");
					memcpy(dest, src, len);
					break;
				}
				default:
					printk(BIOS_INFO,  "CBFS:  Unknown compression type %d\n", ptr->compression);
					return -1;
			}
			end = dest + ptr->s_memsz;
			middle = dest + len;
			printk(BIOS_SPEW, "[ 0x%08lx, %08lx, 0x%08lx) <- %08lx\n",
				(unsigned long)dest,
				(unsigned long)middle,
				(unsigned long)end,
				(unsigned long)src);

			/* Zero the extra bytes between middle & end */
			if (middle < end) {
				printk(BIOS_DEBUG, "Clearing Segment: addr: 0x%016lx memsz: 0x%016lx\n",
					(unsigned long)middle, (unsigned long)(end - middle));

				/* Zero the extra bytes */
				memset(middle, 0, end - middle);
			}
			/* Copy the data that's outside the area that shadows ramstage */
			printk(BIOS_DEBUG, "dest %p, end %p, bouncebuffer %lx\n", dest, end, bounce_buffer);
			if ((unsigned long)end > bounce_buffer) {
				if ((unsigned long)dest < bounce_buffer) {
					unsigned char *from = dest;
					unsigned char *to = (unsigned char*)(lb_start-(bounce_buffer-(unsigned long)dest));
					unsigned long amount = bounce_buffer-(unsigned long)dest;
					printk(BIOS_DEBUG, "move prefix around: from %p, to %p, amount: %lx\n", from, to, amount);
					memcpy(to, from, amount);
				}
				if ((unsigned long)end > bounce_buffer + (lb_end - lb_start)) {
					unsigned long from = bounce_buffer + (lb_end - lb_start);
					unsigned long to = lb_end;
					unsigned long amount = (unsigned long)end - from;
					printk(BIOS_DEBUG, "move suffix around: from %lx, to %lx, amount: %lx\n", from, to, amount);
					memcpy((char*)to, (char*)from, amount);
				}
			}

			/*
			 * Each architecture can perform additonal operations
			 * on the loaded segment
			 */
			arch_segment_loaded((uintptr_t)dest, ptr->s_memsz,
					last_non_empty == ptr ? SEG_FINAL : 0);
		}
	}

	return 1;
}

void *selfload(struct prog *payload)
{
	uintptr_t entry = 0;
	struct segment head;
	void *data;

	data = rdev_mmap_full(prog_rdev(payload));

	if (data == NULL)
		return NULL;

	/* Preprocess the self segments */
	if (!build_self_segment_list(&head, data, &entry))
		goto out;

	/* Load the segments */
	if (!load_self_segments(&head, payload))
		goto out;

	printk(BIOS_SPEW, "Loaded segments\n");

	rdev_munmap(prog_rdev(payload), data);

	/* Update the payload's area with the bounce buffer information. */
	prog_set_area(payload, (void *)(uintptr_t)bounce_buffer, bounce_size);

	/* Update the payload's area with the bounce buffer information. */
	prog_set_area(payload, (void *)(uintptr_t)bounce_buffer, bounce_size);

	return (void *)entry;

out:
	rdev_munmap(prog_rdev(payload), data);
	return NULL;
}
