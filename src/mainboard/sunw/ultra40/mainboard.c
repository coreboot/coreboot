#include <device/device.h>
#include "chip.h"

#if CONFIG_CHIP_NAME == 1
struct chip_operations mainboard_sunw_ultra40_ops = {
	CHIP_NAME("Sun Ultra 40 mainboard")
};
#endif

