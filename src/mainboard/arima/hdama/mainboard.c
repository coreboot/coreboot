#include <console/console.h>
#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <device/pci_ops.h>

#include <arch/io.h>
#include <device/chip.h>
#include "chip.h"


unsigned long initial_apicid[CONFIG_MAX_CPUS] =
{
	0, 1,
};

static void
enable(struct chip *chip, enum chip_pass pass)
{

        struct mainboard_arima_hdama_config *conf =
                (struct mainboard_arima_hdama_config *)chip->chip_info;

        switch (pass) {
	default: break;
	case CONF_PASS_PRE_BOOT:
		break;
        }

}
struct chip_control mainboard_arima_hdama_control = {
                enable: enable,
                name:   "Arima HDAMA mainboard "
};

