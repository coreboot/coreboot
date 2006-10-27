#include <console/console.h>
#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <device/pci_ops.h>
#include <cpu/x86/msr.h>
#include <arch/io.h>
#include "chip.h"

struct chip_operations mainboard_intel_e7520_ops = {
	CHIP_NAME("Intel Jarell mainboard")
};

