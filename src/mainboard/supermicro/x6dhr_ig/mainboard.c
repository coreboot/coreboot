#include <console/console.h>
#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <device/pci_ops.h>
#include <cpu/x86/msr.h>
#include "chip.h"

struct chip_operations mainboard_supermicro_x6dhr_ig_ops = {
	CHIP_NAME("Supermicro X6DHR-iG mainboard")
};

