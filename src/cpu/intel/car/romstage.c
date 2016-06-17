#include <cpu/intel/romstage.h>

void * asmlinkage romstage_main(unsigned long bist)
{
	mainboard_romstage_entry(bist);
	return (void*)CONFIG_RAMTOP;
}
