#if USE_ELF_BOOT
#include <printk.h>
#include <part/fallback_boot.h>
#include <boot/elf.h>
#include <boot/elf_boot.h>
#include <rom/read_bytes.h>
#include <string.h>
#include <subr.h>
#include <stdint.h>


extern unsigned char _text;
extern unsigned char _etext;
extern unsigned char _rodata;
extern unsigned char _erodata;
extern unsigned char _data;
extern unsigned char _edata;
extern unsigned char _bss;
extern unsigned char _ebss;
extern unsigned char _heap;
extern unsigned char _eheap;
extern unsigned char _stack;
extern unsigned char _estack;
struct range {
	unsigned long start;
	unsigned long end;
};
#define RANGE(SEGMENT) { (unsigned long)&_ ## SEGMENT, (unsigned long)&_e ## SEGMENT }
static struct range bad_ranges[] = {
	RANGE(text),
	RANGE(rodata),
	RANGE(data),
	RANGE(bss),
	RANGE(heap),
	RANGE(stack),
};

static int safe_range(unsigned long start, unsigned long len)
{
	/* Check through all of the segments and see if the segment
	 * that was passed in overlaps with any of them.
	 */
	int i;
	unsigned long end = start + len;
	for(i = 0; i < sizeof(bad_ranges)/sizeof(bad_ranges[0]); i++) {
		if ((start < bad_ranges[i].end) &&
			(end > bad_ranges[i].start)) {
			printk_err(__FUNCTION__ " start 0x%x end 0x%x\n", 
					start, end);
			printk_err(__FUNCTION__ " Conflicts with range %d\n",
				i);
			printk_err("  which starts at 0x%x ends at 0x%x\n", 
				bad_ranges[i].start, bad_ranges[i].end);	
			return 0;
		}
	}
	return 1;
}

int elfboot(void)
{
	static unsigned char header[ELF_HEAD_SIZE];
	unsigned long offset;
	Elf_ehdr *ehdr;
	Elf_phdr *phdr;
	int header_offset;
	void *ptr, *entry;
	int i;

	printk_info("\n");
	printk_info("Welcome to %s, the open sourced starter.\n", BOOTLOADER);
	printk_info("January 2002, Eric Biederman.\n");
	printk_info("Version %s\n", BOOTLOADER_VERSION);
	printk_info("\n");
	if (streams->init() < 0) {
		printk_err("Could not initialize driver...\n");
		goto out;
	}

	post_code(0xf8);
	/* Read in the initial ELF_HEAD_SIZE bytes */
	if (streams->read(header, ELF_HEAD_SIZE) != ELF_HEAD_SIZE) {
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

	/* Sanity check the segments and zero the extra bytes */
	for(i = 0; i < ehdr->e_phnum; i++) {
		unsigned char *dest, *end;

		if (!safe_range(phdr[i].p_paddr, phdr[i].p_memsz)) {
			printk_err("Bad memory range: [0x%016lx, 0x%016lx)\n",
				phdr[i].p_paddr, phdr[i].p_memsz);
			goto out;
		}
		dest = (unsigned char *)(phdr[i].p_paddr);
		end = dest + phdr[i].p_memsz;
		dest += phdr[i].p_filesz;

		if (dest < end) {
			printk_debug("Clearing Section: addr: 0x%016lx memsz: 0x%016lx\n",
				(unsigned long)dest, end - dest);

			/* Zero the extra bytes */
			while(dest < end) {
				*(dest++) = 0;
			}
		}
	}
	
	offset = 0;
	while(1) {
		Elf_phdr *cur_phdr = 0;
		int i,len;
		unsigned long start_offset;
		unsigned char *dest, *middle, *end;
		/* Find the program header that descibes the current piece
		 * of the file.
		 */
		for(i = 0; i < ehdr->e_phnum; i++) {
			if (phdr[i].p_type != PT_LOAD) {
				continue;
			}
			if (phdr[i].p_filesz == 0) {
				continue;
			}
			if (phdr[i].p_filesz > phdr[i].p_memsz) {
				continue;
			}
			if (phdr[i].p_offset >= offset) {
				if (!cur_phdr ||
					(cur_phdr->p_offset > phdr[i].p_offset)) {
					cur_phdr = &phdr[i];
				}
			}
		}

		/* If we are out of sections we are done */
		if (!cur_phdr) {
			break;
		}
		printk_debug("Loading Section: addr: 0x%016lx memsz: 0x%016lx filesz: 0x%016lx\n",
			cur_phdr->p_paddr, cur_phdr->p_memsz, cur_phdr->p_filesz);

		/* Compute the boundaries of the section */
		dest = (unsigned char *)(cur_phdr->p_paddr);
		end = dest + cur_phdr->p_memsz;
		len = cur_phdr->p_filesz;
		if (len > cur_phdr->p_memsz) {
			len = cur_phdr->p_memsz;
		}
		middle = dest + len;
		start_offset = cur_phdr->p_offset;

		/* Skip intial buffer unused bytes */
		if (offset < (ELF_HEAD_SIZE - header_offset)) {
			if (start_offset < (ELF_HEAD_SIZE - header_offset)) {
				offset = start_offset;
			} else {
				offset = (ELF_HEAD_SIZE - header_offset);
			}
		}

		/* Skip the unused bytes */
		if (streams->skip(start_offset - offset) != (start_offset - offset)) {
			printk_err("skip failed\n");
			goto out;
		}
		offset = start_offset;

		/* Copy data from the initial buffer */
		if (offset < (ELF_HEAD_SIZE - header_offset)) {
			size_t len;
			if ((cur_phdr->p_filesz + start_offset) > ELF_HEAD_SIZE) {
				len = ELF_HEAD_SIZE - start_offset;
			}
			else {
				len = cur_phdr->p_filesz;
			}
			memcpy(dest, &header[header_offset + start_offset], len);
			dest += len;
		}
		
		/* Read the section into memory */
		if (streams->read(dest, middle - dest) != (middle - dest)) {
			printk_err("Read failed...\n");
			goto out;
		}
		offset += cur_phdr->p_filesz;
		/* The extra bytes between dest & end have been zeroed */
	}


	/* Reset to booting from this image as late as possible */
	streams->fini();
	boot_successful();

	printk_debug("Jumping to boot code\n");
	post_code(0xfe);

	/* Jump to kernel */
	jmp_to_elf_entry(entry);

 out:
	printk_err("Bad ELF Image\n");
	for(i = 0; i < sizeof(*ehdr); i++) {
		if ((i & 0xf) == 0) {
			printk_err("\n");
		}
		printk_err("%02x ", header[i]);
	}
	printk_err("\n");

	/* Reset to booting from this image as late as possible */
	streams->fini();
#if 0
	boot_successful();
#endif

	return 0;
}
#endif /* USE_ELF_BOOT */
