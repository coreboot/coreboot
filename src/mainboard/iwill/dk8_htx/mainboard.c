#include <console/console.h>
#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <device/pci_ops.h>
#include "chip.h"

#if CONFIG_CHIP_NAME == 1
struct chip_operations mainboard_iwill_dk8_htx_ops = {
	CHIP_NAME("Iwill DK8-HTX mainboard")
};
#endif
