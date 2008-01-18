#include <ip_checksum.h>
#include <boot/elf.h>
#include <boot/elf_boot.h>
#include <string.h>
#include <console/console.h>

extern void flush_dcache(void);

int elf_check_arch(Elf_ehdr *ehdr)
{
	return (
		(ehdr->e_machine == EM_PPC) &&
		(ehdr->e_ident[EI_CLASS] == ELFCLASS32) &&
		(ehdr->e_ident[EI_DATA] == ELFDATA2MSB) 
		);
	
}

void jmp_to_elf_entry(void *entry, unsigned long buffer)
{
	void (*kernel_entry)(void);       

	kernel_entry = entry;

	/*
	 * Kernel will invalidate and disable dcache immediately on
	 * entry. This is bad if we've been using it, which we
	 * have. Make sure it is flushed to memory.
	 */
	flush_dcache();

	/* On ppc we don't currently support loading over coreboot.
	 * So ignore the buffer.
	 */

	/* Jump to kernel */
	kernel_entry();
}
