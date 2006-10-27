#include <console/console.h>
#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <device/pci_ops.h>
#include "chip.h"

struct chip_operations mainboard_densitron_dpx114_ops = {
	CHIP_NAME("Densitron DPX114 mainboard")
};

