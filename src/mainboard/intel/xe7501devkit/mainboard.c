#include <console/console.h>
#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <device/pci_ops.h>
#include "chip.h"

#if CONFIG_CHIP_NAME == 1
struct chip_operations mainboard_intel_xe7501devkit_ops = {
	CHIP_NAME("Intel Xeon E7501 DevKit mainboard")
};
#endif
