#include <console/console.h>
#include <arch/io.h>
#include <stdint.h>
#include <device/device.h>
#include <device/pci.h>
#include <stdlib.h>
#include <string.h>
#include <bitops.h>
#include "chip.h"

#if CONFIG_CHIP_NAME
struct chip_operations northbridge_intel_e7501_ops = {
	CHIP_NAME("Intel E7501 northbridge")
};
#endif
