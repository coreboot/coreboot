#include <types.h>
#include <device/pci.h>
#include "mpc107.h"

void
wait_for_other_cpus(void)
{
}

unsigned long
this_processors_id(void)
{
	u32 pic1;          
	struct device *dev;

	if ((dev = dev_find_slot(0, 0)) == NULL)
		return 0;

	pic1 = pci_read_config32(dev, MPC107_PIC1);
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
