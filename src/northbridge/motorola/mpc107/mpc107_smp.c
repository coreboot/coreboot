#include <pci.h>
#include "mpc107.h"

void
wait_for_other_cpus(void)
{
}

unsigned long
this_processors_id(void)
{
	u32 pic1;          

	pcibios_read_config_dword(0, 0, MPC107_PIC1, &pic1);
	return (pic1 & MPC107_PIC1_CF_MP_ID) >> 14;
}

unsigned long
processor_index(unsigned long id)
{
	return id;
}

void
startup_other_cpus(unsigned long *map)
{
}
