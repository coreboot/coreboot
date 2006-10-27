#include <console/console.h>
#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <device/pci_ops.h>
#include <arch/io.h>
#include "chip.h"

struct chip_operations mainboard_advantech_som_gx533c_ops = {
	CHIP_NAME("Advantech SOM GX DB533-C mainboard")
};

