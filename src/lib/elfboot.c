#if USE_ELF_BOOT
#include <printk.h>
#include <boot/elf.h>
#include <boot/uniform_boot.h>
#include <rom/fill_inbuf.h>
#include <string.h>
#include <subr.h>

int elfboot(size_t totalram)
{
	static unsigned char header[ELF_HEAD_SIZE];
	unsigned long offset;
	Elf_ehdr *ehdr;
	Elf_phdr *phdr;
	void *ptr, *entry;
	int i;

	printk("\n");
	printk("Welcome to elfboot, the open sourced starter.\n");
	printk("Febuary 2001, Eric Biederman.\n");
	printk("Version 0.99\n");
	printk("\n");
	ptr = get_ube_pointer(totalram);

	post_code(0xf8);
	/* Read in the initial 512 bytes */
	for(offset = 0; offset < 512; offset++) {
		header[offset] = get_byte();
	}
	ehdr = (Elf_ehdr *)(&header[0]);
	entry = (void *)(ehdr->e_entry);
	
	/* Sanity check the elf header */
	if ((memcmp(ehdr->e_ident, ELFMAG, 4) != 0) ||
		(ehdr->e_type != ET_EXEC) ||
		(!elf_check_arch(ehdr)) ||
		(ehdr->e_ident[EI_VERSION] != EV_CURRENT) ||
		(ehdr->e_version != EV_CURRENT) ||
		(ehdr->e_phoff > ELF_HEAD_SIZE) ||
		(ehdr->e_phentsize != sizeof(Elf_phdr)) ||
		((ehdr->e_phoff + (ehdr->e_phentsize * ehdr->e_phnum)) > 
			ELF_HEAD_SIZE)) {
		goto out;
	}

	phdr = (Elf_phdr *)&header[ehdr->e_phoff];
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
		printk("Loading Section: addr: 0x%08x memsz: 0x%08x filesz: 0x%08x\n",
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
		if (offset < ELF_HEAD_SIZE) {
			if (start_offset < ELF_HEAD_SIZE) {
				offset = start_offset;
			} else {
				offset = ELF_HEAD_SIZE;
			}
		}

		/* Skip the unused bytes */
		while(offset < start_offset) {
			offset++;
			get_byte();
		}

		/* Copy data from the initial buffer */
		if (offset < ELF_HEAD_SIZE) {
			size_t len;
			if ((cur_phdr->p_filesz + start_offset) > ELF_HEAD_SIZE) {
				len = ELF_HEAD_SIZE - start_offset;
			}
			else {
				len = cur_phdr->p_filesz;
			}
			memcpy(dest, &header[start_offset], len);
			dest += len;
		}
		
		/* Read the section into memory */
		while(dest < middle) {
			*(dest++) = get_byte();
		}
		offset += cur_phdr->p_filesz;
		/* Zero the extra bytes */
		while(dest < end) {
			*(dest++) = 0;
		}
	}

	DBG("Jumping to boot code\n");
	post_code(0xfe);

	/* Jump to kernel */
	jmp_to_elf_entry(entry, ptr);

 out:
	printk("Bad ELF Image\n");
	for(i = 0; i < sizeof(*ehdr); i++) {
		if ((i & 0xf) == 0) {
			printk("\n");
		}
		printk("%02x ", header[i]);
	}
	printk("\n");

	return 0;
}
#endif /* USE_ELF_BOOT */
