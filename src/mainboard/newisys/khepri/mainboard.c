#include <console/console.h>
#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <device/pci_ops.h>

#include <arch/io.h>
#include "../../../northbridge/amd/amdk8/northbridge.h"
#include "chip.h"

struct chip_operations mainboard_newisys_khepri_control = {
	.name      = "Newisys Khepri mainboard ",
};

