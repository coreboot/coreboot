#include <console/console.h>
#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <device/pci_ops.h>
#include "../../../northbridge/amd/amdk8/northbridge.h"
#include "chip.h"


struct chip_operations mainboard_tyan_s4882_ops = {
	CHIP_NAME("Tyan s4882 mainboard")
};
