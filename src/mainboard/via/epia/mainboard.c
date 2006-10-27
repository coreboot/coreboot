#include <console/console.h>
#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <device/pci_ops.h>
#include <arch/io.h>
#include "chip.h"

struct chip_operations mainboard_via_epia_ops = {
	CHIP_NAME("VIA EPIA mainboard")
};

