#include <device/device.h>
#include "chip.h"

#if CONFIG_CHIP_NAME == 1
struct chip_operations mainboard_amd_serengeti_leopard_ops = {
	CHIP_NAME("AMD Serengeti Leopard Mainboard")
};
#endif

