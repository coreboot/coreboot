#if USE_ELF_BOOT
#include <printk.h>
#include <part/fallback_boot.h>
#include <boot/elf.h>
#include <boot/elf_boot.h>
#include <boot/linuxbios_tables.h>
#include <rom/read_bytes.h>
#include <string.h>
#include <subr.h>
#include <stdint.h>
#include <stdlib.h>

/* Maximum physical address we can use for the linuxBIOS bounce buffer.
 */
#ifndef MAX_ADDR
#define MAX_ADDR -1UL
#endif

extern unsigned char _ram_seg;
extern unsigned char _eram_seg;

struct segment {
	struct segment *next;
	struct segment *prev;
	unsigned long s_addr;
	unsigned long s_memsz;
	unsigned long s_offset;
	unsigned long s_filesz;
};


/* The problem:  
 * Static executables all want to share the same addresses
 * in memory because only a few addresses are reliably present on
 * a machine, and implementing general relocation is hard.
 *
 * The solution:
 * - Allocate a buffer twice the size of the linuxBIOS image.
 * - Anything that would overwrite linuxBIOS copy into the lower half of
 *   the buffer. 
 * - After loading an ELF image copy linuxBIOS to the upper half of the
 *   buffer.
 * - Then jump to the loaded image.
 * 
 * Benefits:
 * - Nearly arbitrary standalone executables can be loaded.
 * - LinuxBIOS is preserved, so it can be returned to.
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
	/* Double linuxBIOS size so I have somewhere to place a copy to return to */
	lb_size = lb_size + lb_size;
	mem_entries = (mem->size - sizeof(*mem))/sizeof(mem->map[0]);
	buffer = 0;
	for(i = 0; i < mem_entries; i++) {
		unsigned long mstart, mend;
		unsigned long msize;
		unsigned long tbuffer;
		if (mem->map[i].type != LB_MEM_RAM)
			continue;
		if (mem->map[i].start > MAX_ADDR)
			continue;
		if (mem->map[i].size < lb_size)
			continue;
		mstart = mem->map[i].start;
		msize = MAX_ADDR - mstart +1;
		if (msize > mem->map[i].size)
			msize = mem->map[i].size;
		mend = mstart + msize;
		tbuffer = mend - lb_size;
		if (tbuffer < buffer) 
			continue;
		buffer = tbuffer;
	}
	return buffer;
}

static int safe_range(struct lb_memory *mem, unsigned long buffer,
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
		mstart = mem->map[i].start;
		mend = mstart + mem->map[i].size;
		if ((mtype == LB_MEM_RAM) && (start < mend) && (end > mstart)) {
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
			mstart = mem->map[i].start;
			mend = mstart + mem->map[i].size;
			printk_err("  [0x%016lx, 0x%016lx) %s\n",
				(unsigned long)mstart, 
				(unsigned long)mend, 
				(mtype == LB_MEM_RAM)?"RAM":"Reserved");
			
		}
		return 0;
	}
	return 1;
}

static void bounce_segments(unsigned long buffer, struct segment *head)
{
	/* Modify all segments that want to load onto linuxBIOS
	 * to load onto the bounce buffer instead.
	 */
	unsigned long lb_start = (unsigned long)&_ram_seg;
	unsigned long lb_end = (unsigned long)&_eram_seg;
	struct segment *ptr;

	printk_spew("lb: [0x%016lx, 0x%016lx)\n", 
		lb_start, lb_end);

	for(ptr = head->next; ptr != head; ptr = ptr->next) {
		unsigned long start, middle, end;
		start = ptr->s_addr;
		middle = start + ptr->s_filesz;
		end = start + ptr->s_memsz;
		/* I don't conflict with linuxBIOS so get out of here */
		if ((end <= lb_start) || (start >= lb_end))
			continue;

		printk_spew("segment: [0x%016lx, 0x%016lx, 0x%016lx)\n", 
			start, middle, end);

		/* Slice off a piece at the beginning
		 * that doesn't conflict with linuxBIOS.
		 */
		if (start < lb_start) {
			struct segment *new;
			unsigned long len = lb_start - start;
			new = malloc(sizeof(*new));
			*new = *ptr;
			new->s_memsz = len;
			ptr->s_memsz -= len;
			ptr->s_addr += len;
			ptr->s_offset += len;
			if (ptr->s_filesz > len) {
				new->s_filesz = len;
				ptr->s_filesz -= len;
			} else {
				ptr->s_filesz = 0;
			}
			new->next = ptr;
			ptr->prev = new;
			start = ptr->s_addr;

			printk_spew("   early: [0x%016lx, 0x%016lx, 0x%016lx)\n", 
				new->s_addr, 
				new->s_addr + new->s_filesz,
				new->s_addr + new->s_memsz);
		}

		/* Slice off a piece at the end 
		 * that doesn't conflict with linuxBIOS 
		 */
		if (end > lb_end) {
			struct segment *new;
			unsigned long len = end - lb_end;
			new = malloc(sizeof(*new));
			*new = *ptr;
			ptr->s_memsz = len;
			new->s_memsz -= len;
			new->s_addr += len;
			new->s_offset += len;
			if (ptr->s_filesz > len) {
				ptr->s_filesz = len;
				new->s_filesz -= len;
			} else {
				new->s_filesz = 0;
			}
			ptr->next = new;
			new->prev = ptr;
			end = start + len;

			printk_spew("   late: [0x%016lx, 0x%016lx, 0x%016lx)\n", 
				new->s_addr, 
				new->s_addr + new->s_filesz,
				new->s_addr + new->s_memsz);

		}
		/* Now retarget this segment onto the bounce buffer */
		ptr->s_addr = buffer + (ptr->s_addr - lb_start);

		printk_spew(" bounce: [0x%016lx, 0x%016lx, 0x%016lx)\n", 
			ptr->s_addr, 
			ptr->s_addr + ptr->s_filesz, 
			ptr->s_addr + ptr->s_memsz);
	}
}

int elfboot(struct stream *stream, struct lb_memory *mem)
{
	static unsigned char header[ELF_HEAD_SIZE];
	unsigned long offset;
	Elf_ehdr *ehdr;
	Elf_phdr *phdr;
	int header_offset;
	unsigned long bounce_buffer;
	struct segment dummy;
	struct segment *head, *ptr;
	void *entry;
	int i;
	byte_offset_t amtread;

	printk_info("\n");
	printk_info("Welcome to %s, the open sourced starter.\n", BOOTLOADER);
	printk_info("January 2002, Eric Biederman.\n");
	printk_info("Version %s\n", BOOTLOADER_VERSION);
	printk_info("\n");
	if (stream->init() < 0) {
		printk_err("Could not initialize driver...\n");
		goto out;
	}

	/* Find a bounce buffer so I can load to linuxBIOS's current location */
	bounce_buffer = get_bounce_buffer(mem);
	if (!bounce_buffer) {
		printk_err("Could not find a bounce buffer...\n");
		goto out;
	}


	post_code(0xf8);
	/* Read in the initial ELF_HEAD_SIZE bytes */
	if (stream->read(header, ELF_HEAD_SIZE) != ELF_HEAD_SIZE) {
		printk_err("Read failed...\n");
		goto out;
	}
	/* Scan for an elf header */
	header_offset = -1;
	for(i = 0; i < ELF_HEAD_SIZE - (sizeof(Elf_ehdr) + sizeof(Elf_phdr)); i+=16) {
		ehdr = (Elf_ehdr *)(&header[i]);
		if (memcmp(ehdr->e_ident, ELFMAG, 4) != 0) {
			continue;
		}
		printk_debug("Found ELF candiate at offset %d\n", i);
		/* Sanity check the elf header */
		if ((ehdr->e_type == ET_EXEC) &&
			elf_check_arch(ehdr) &&
			(ehdr->e_ident[EI_VERSION] == EV_CURRENT) &&
			(ehdr->e_version == EV_CURRENT) &&
			(ehdr->e_ehsize == sizeof(Elf_ehdr)) &&
			(ehdr->e_phentsize = sizeof(Elf_phdr)) &&
			(ehdr->e_phoff < (ELF_HEAD_SIZE - i)) &&
			((ehdr->e_phoff + (ehdr->e_phentsize * ehdr->e_phnum)) <= 
				(ELF_HEAD_SIZE - i))) {
			header_offset = i;
			break;
		}
		ehdr = 0;
	}
	if (header_offset == -1) {
		goto out;
	}
	entry = (void *)(ehdr->e_entry);
	phdr = (Elf_phdr *)&header[ehdr->e_phoff + header_offset];

	/* Create an ordered table of the segments */
	memset(&dummy, 0, sizeof(dummy));
	head = &dummy;
	head->next = head->prev = head;
	for(i = 0; i < ehdr->e_phnum; i++) {
		struct segment *new;
		new = malloc(sizeof(*new));
		new->s_addr = phdr[i].p_paddr;
		new->s_memsz = phdr[i].p_memsz;
		new->s_offset = phdr[i].p_offset;
		new->s_filesz = phdr[i].p_filesz;
		/* Clean up the values */
		if (new->s_filesz > new->s_memsz)  {
			new->s_filesz = new->s_memsz;
		}
		for(ptr = head->next; ptr != head; ptr = ptr->next) {
			if (new->s_offset < ptr->s_offset)
				break;
		}
		new->next = ptr;
		new->prev = ptr->prev;
		ptr->prev->next = new;
		ptr->prev = new;
	}

	/* Sanity check the segments */
	for(ptr = head->next; ptr != head; ptr = ptr->next) {
		if (!safe_range(mem, bounce_buffer, ptr->s_addr, ptr->s_memsz)) {
			goto out;
		}
	}

	/* Modify all segments that want to load onto linuxBIOS
	 * to load onto the bounce buffer instead.
	 */
	bounce_segments(bounce_buffer, head);

	/* Load the segments */
	offset = 0;
	for(ptr = head->next; ptr != head; ptr = ptr->next) {
		unsigned long start_offset;
		unsigned long skip_bytes, read_bytes;
		unsigned char *dest, *middle, *end;
		byte_offset_t result;
		printk_debug("Loading Section: addr: 0x%016lx memsz: 0x%016lx filesz: 0x%016lx\n",
			ptr->s_addr, ptr->s_memsz, ptr->s_filesz);

		/* Compute the boundaries of the section */
		dest = (unsigned char *)(ptr->s_addr);
		end = dest + ptr->s_memsz;
		middle = dest + ptr->s_filesz;
		start_offset = ptr->s_offset;

		printk_spew("[ 0x%016lx, %016lx, 0x%016lx) <- %016lx\n",
			(unsigned long)dest,
			(unsigned long)middle,
			(unsigned long)end,
			(unsigned long)start_offset);

		/* Skip intial buffer unused bytes */
		if (offset < (ELF_HEAD_SIZE - header_offset)) {
			if (start_offset < (ELF_HEAD_SIZE - header_offset)) {
				offset = start_offset;
			} else {
				offset = (ELF_HEAD_SIZE - header_offset);
			}
		}

		/* Skip the unused bytes */
		skip_bytes = start_offset - offset;
		if (skip_bytes && 
			((result = stream->skip(skip_bytes)) != skip_bytes)) {
			printk_err("ERROR: Skip of %ld bytes skiped %ld bytes\n",
				skip_bytes, result);
			goto out;
		}
		offset = start_offset;

		/* Copy data from the initial buffer */
		if (offset < (ELF_HEAD_SIZE - header_offset)) {
			size_t len;
			if ((ptr->s_filesz + start_offset) > ELF_HEAD_SIZE) {
				len = ELF_HEAD_SIZE - start_offset;
			}
			else {
				len = ptr->s_filesz;
			}
			memcpy(dest, &header[header_offset + start_offset], len);
			dest += len;
		}
		
		/* Read the section into memory */
		read_bytes = middle - dest;
		if (read_bytes && 
			((result = stream->read(dest, read_bytes)) != read_bytes)) {
			printk_err("ERROR: Read of %ld bytes read %ld bytes...\n",
				read_bytes, result);
			goto out;
		}
		offset += ptr->s_filesz;

		/* Zero the extra bytes between middle & end */
		if (middle < end) {
			printk_debug("Clearing Section: addr: 0x%016lx memsz: 0x%016lx\n",
				(unsigned long)middle, end - middle);

			/* Zero the extra bytes */
			memset(middle, 0, end - middle);
		}
	}


	/* Reset to booting from this image as late as possible */
	stream->fini();
	boot_successful();

	printk_debug("Jumping to boot code\n");
	post_code(0xfe);

	/* Jump to kernel */
	jmp_to_elf_entry(entry, bounce_buffer);
	return 1;

 out:
	printk_err("Cannot Load ELF Image\n");

	/* Shutdown the stream device */
	stream->fini();

	return 0;
}
#endif /* USE_ELF_BOOT */
