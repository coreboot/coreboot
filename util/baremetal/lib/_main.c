#include <boot/linuxbios_tables.h>
#include <preboot.h>

#define DPRINTF( x... )

struct lb_memory *lbmem;

unsigned long find_ramtop( struct lb_memory *mem)
{
	int high_entry=0;
	unsigned long int high_start=0;
	int mem_entries;
	int i;


	mem_entries = (mem->size - sizeof(*mem))/sizeof(mem->map[0]);

	for(i=0; i<mem_entries; i++) {
		if(mem->map[i].type == LB_MEM_RAM) 
			if(mem->map[i].start > high_start) {
				high_entry=i;
				high_start = mem->map[i].start;
			}
	}

	return(high_start + mem->map[high_entry].size);

}

unsigned long int preboot_ramtop;
pre_boot_param_t *preboot_param;


int _main(pre_boot_param_t *pbp) {

	if(strcmp(pbp->sig, "$PBP$")) {
		lbmem = get_lbmem();

		DPRINTF("Got lbmem struct: %08x\n", (unsigned int) lbmem);

		preboot_ramtop = find_ramtop(lbmem);
		DPRINTF("Top of RAM = %08x\n", preboot_ramtop);
		preboot_param = (pre_boot_param_t *) preboot_ramtop - sizeof(pre_boot_param_t);
		if( strcmp(preboot_param->sig, "$PBP$")) {
			preboot_ramtop -= 4096;
			strcpy(preboot_param->sig, "$PBP$");
			preboot_param->lbmem = lbmem;
			preboot_param->stack = preboot_ramtop;

			__asm__ __volatile__(
				"movl %%ebp, %%eax\n"
				"subl %%esp, %%eax\n"
				"movl %0, %%esp\n" 
				"movl %0, %%ebp\n"
				"pushl %%ebp\n" 
				"subl %%eax, %%esp\n" :: "d" (preboot_ramtop));

			// reserve a stack
			preboot_param->free_ramtop = preboot_ramtop - 0x8000;
			preboot_param->stacksize = 0x8000;

			DPRINTF("ramtop = %08x, free_ramtop = %08x\n", preboot_ramtop, preboot_param->free_ramtop);
		}

		DPRINTF("Calling main\n");

		return(main());
	} else {
		DPRINTF("Pre boot environment already set up, calling main\n");
		preboot_param = pbp;
		return(main());
	}
}
