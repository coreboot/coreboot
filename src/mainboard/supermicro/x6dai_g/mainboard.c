#include <console/console.h>
#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <device/pci_ops.h>
#include <cpu/x86/msr.h>
#include "chip.h"

struct chip_operations supermicro_x6dai_g_ops = {
	CHIP_NAME("Supermicro X6DAi-G mainboard")
};

