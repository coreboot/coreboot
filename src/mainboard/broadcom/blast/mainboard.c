#include <device/device.h>
#include "chip.h"

#if CONFIG_CHIP_NAME == 1
struct chip_operations mainboard_broadcom_blast_ops = {
	CHIP_NAME("Broadcom Blast mainboard")
};
#endif

