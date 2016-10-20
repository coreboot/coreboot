#include <arch/acpi_device.h>
#include <device/i2c.h>

struct drivers_i2c_generic_config {
	const char *hid;	/* ACPI _HID (required) */
	const char *name;	/* ACPI Device Name */
	const char *desc;	/* Device Description */
	unsigned uid;		/* ACPI _UID */
	enum i2c_speed speed;	/* Bus speed in Hz, default is I2C_SPEED_FAST */
	unsigned wake;		/* Wake GPE */
	struct acpi_irq irq;	/* Interrupt */

	/*
	 * This flag will add a device propery which will indicate
	 * to the OS that it should probe this device before adding it.
	 *
	 * This can be used to declare a device that may not exist on
	 * the board, for example to support multiple trackpad vendors.
	 */
	int probed;

	/* GPIO used to indicate if this device is present */
	unsigned device_present_gpio;
	unsigned device_present_gpio_invert;

	/* GPIO used to take device out of reset or to put it into reset. */
        unsigned reset_gpio;
	/* Delay to be inserted after device is taken out of reset. */
	unsigned reset_delay_ms;
	/* GPIO used to enable device. */
	unsigned enable_gpio;
	/* Delay to be inserted after device is enabled. */
	unsigned enable_delay_ms;
};
