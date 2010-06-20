#include <device/device.h>
#include <boot/tables.h>
#include "chip.h"

int add_mainboard_resources(struct lb_memory *mem)
{
	return add_northbridge_resources(mem);
}

struct chip_operations mainboard_ops = {
	CHIP_NAME("ASUS MEW-VM Mainboard")
};
