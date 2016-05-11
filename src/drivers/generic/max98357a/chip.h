#include <arch/acpi_device.h>

struct drivers_generic_max98357a_config {
	/* SDMODE GPIO */
	struct acpi_gpio sdmode_gpio;

	/* SDMODE Delay */
	unsigned sdmode_delay;

	/* GPIO used to indicate if this device is present */
	unsigned device_present_gpio;
	unsigned device_present_gpio_invert;
};
