#include <device/device.h>
#include "chip.h"

#if CONFIG_CHIP_NAME == 1 
struct chip_operations cpu_amd_socket_940_ops = {
	CHIP_NAME("socket 940")
};
#endif
