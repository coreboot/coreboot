/*
 * elfboot -- boot elf images
 * 
 * (C)opyright 2002 Eric Biederman, Linux NetworX
 * (C)opyright 2007 Ronald G. Minnich
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
 *
 */

/* This code is modified from the LinuxBIOS V2 version as follows:
 * great simplified
 * checksum removed -- lar can do that
 * can run from read-only FLASH
 * no calls to malloc
 */

#include <arch/types.h>
#include <string.h>

#include <console.h>
#include <tables.h>
#include <elf.h>
#include <elf_boot.h>

static int valid_area(struct lb_memory *mem, 
	unsigned long start, unsigned long len)
{
	/* Check through all of the memory segments and ensure
	 * the segment that was passed in is completely contained
	 * in RAM.
	 */
	int i;
	unsigned long end = start + len;
	unsigned long mem_entries = (mem->size - sizeof(*mem))/sizeof(mem->map[0]);

	/* Walk through the table of valid memory ranges and see if I
	 * have a match.
	 */
	for(i = 0; i < mem_entries; i++) {
		u64 mstart, mend;
		u32 mtype;
		mtype = mem->map[i].type;
		mstart = unpack_lb64(mem->map[i].start);
		mend = mstart + unpack_lb64(mem->map[i].size);
		if ((mtype == LB_MEM_RAM) && (start < mend) && (end > mstart)) {
			break;
		}
	}
	if (i == mem_entries) {
		printk(BIOS_ERR, "No matching RAM area found for range:\n");
		printk(BIOS_ERR, "  [0x%016lx, 0x%016lx)\n", start, end);
		printk(BIOS_ERR, "RAM areas\n");
		for(i = 0; i < mem_entries; i++) {
			u64 mstart, mend;
			u32 mtype;
			mtype = mem->map[i].type;
			mstart = unpack_lb64(mem->map[i].start);
			mend = mstart + unpack_lb64(mem->map[i].size);
			printk(BIOS_ERR, "  [0x%016lx, 0x%016lx) %s\n",
				(unsigned long)mstart, 
				(unsigned long)mend, 
				(mtype == LB_MEM_RAM)?"RAM":"Reserved");
			
		}
		return 0;
	}
	return 1;
}

static int load_elf_segments(struct lb_memory *mem,unsigned char *header, int headers)
{
        Elf_ehdr *ehdr;
        Elf_phdr *phdr;

        ehdr = (Elf_ehdr *)header;
        phdr = (Elf_phdr *)(&header[ehdr->e_phoff]);

	printk(BIOS_DEBUG, "%s: header %p #headers %d\n", header, headers);
	int i;
	int size;
	for(i = 0; i < headers; i++) {
		/* Ignore data that I don't need to handle */
		if (phdr[i].p_type != PT_LOAD) {
			printk(BIOS_DEBUG, "Dropping non PT_LOAD segment\n");
			continue;
		}
		if (phdr[i].p_memsz == 0) {
			printk(BIOS_DEBUG, "Dropping empty segment\n");
			continue;
		}
		printk(BIOS_DEBUG, "New segment addr 0x%lx size 0x%lx offset 0x%lx filesize 0x%lx\n",
			phdr[i].p_paddr, phdr[i].p_memsz, phdr[i].p_offset, phdr[i].p_filesz);
		/* Clean up the values */
		size = phdr[i].p_filesz;
		if (phdr[i].p_filesz > phdr[i].p_memsz)  {
			size = phdr[i].p_memsz;
		}
		printk(BIOS_DEBUG, "(cleaned up) New segment addr 0x%lx size 0x%lx offset 0x%lx\n",
			phdr[i].p_paddr, size, phdr[i].p_offset);

		/* Verify the memory addresses in the segment are valid */
		if (!valid_area(mem, phdr[i].p_paddr, size)) 
			goto out;
		/* let's just be stupid about this. Bzero the whole area we are copying to, 
		  * then copy out the data, which may be a subset of the total area. 
		  * the cache, after all, is your friend.
		  */
		printk(BIOS_INFO, "Set %p to 0 for %d bytes\n", (unsigned char *)phdr[i].p_paddr, phdr[i].p_memsz);
		memset((unsigned char *)phdr[i].p_paddr, 0, phdr[i].p_memsz);
		/* ok, copy it out */
		printk(BIOS_INFO, "Copy to %p from %p for %d bytes\n", (unsigned char *)phdr[i].p_paddr, &header[phdr[i].p_offset], size);
		memcpy((unsigned char *)phdr[i].p_paddr, &header[phdr[i].p_offset], size);
		
	}
	return 1;
 out:
	return 0;
}



int elfload(struct lb_memory *mem, unsigned char *header, unsigned long header_size)
{
	Elf_ehdr *ehdr;
	void *entry;

	ehdr = (Elf_ehdr *)header;
	entry = (void *)(ehdr->e_entry);

	/* Load the segments */
	if (!load_elf_segments(mem, header, header_size))
		goto out;

	printk(BIOS_SPEW, "Loaded segments\n");
	
	/* Reset to booting from this image as late as possible */
	/* what the hell is boot_successful? */
	//boot_successful();

	printk(BIOS_DEBUG, "Jumping to boot code at 0x%x\n", entry);
	post_code(0xfe);

	/* Jump to kernel */
	/* most of the time, jmp_to_elf_entry is just a call. But this hook gives us 
	  * a handy way to get architecture-dependent operations done, if needed 
	  * jmp_to_elf_entry is in arch/<architecture>/archelfboot.c
	  */
	jmp_to_elf_entry(entry);
	return 1;

 out:
	return 0;
}

int elfboot_mem(struct lb_memory *mem, void *where, int size)
{
	Elf_ehdr *ehdr;
	unsigned char *header = where;
	int header_offset;
	int i, result;

	result = 0;
	printk(BIOS_INFO, "ELF loader started.\n");
	post_code(0xf8);

	/* Scan for an elf header */
	header_offset = -1;
	for(i = 0; i < ELF_HEAD_SIZE - (sizeof(Elf_ehdr) + sizeof(Elf_phdr)); i+=16) {
		ehdr = (Elf_ehdr *)(&header[i]);
		if (memcmp(ehdr->e_ident, ELFMAG, 4) != 0) {
			printk(BIOS_SPEW, "No header at %d\n", i);
			continue;
		}
		printk(BIOS_DEBUG, "Found ELF candidate at offset %d\n", i);
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
	printk(BIOS_SPEW, "header_offset is %d\n", header_offset);
	if (header_offset == -1) {
		goto out;
	}

	printk(BIOS_SPEW, "Try to load at offset 0x%x %d phdr\n", header_offset, ehdr->e_phnum);
	result = elfload(mem, header,  ehdr->e_phnum);
 out:
	if (!result) {

		printk(BIOS_ERR, "Cannot load ELF image\n");

		post_code(0xff);
	}
	return 0;
}	
