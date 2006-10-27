#include <console/console.h>
#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <device/pci_ops.h>
#include <cpu/x86/msr.h>
#include <part/hard_reset.h>
#include <device/smbus.h>
#include <delay.h>
#include "chip.h"


struct chip_operations mainboard_arima_hdama_ops = {
	CHIP_NAME("Arima HDAMA mainboard")
};

