#include <device/device.h>

static void mainboard_enable(device_t dev)
{
}

struct chip_operations mainboard_ops = {
	.enable_dev = mainboard_enable,
};
