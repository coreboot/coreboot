#include <arch/acpi_device.h>
#include <device/i2c.h>

struct drivers_i2c_tpm_config {
	const char *hid;	/* ACPI _HID (required) */
	const char *desc;	/* Device Description */
	unsigned int uid;	/* ACPI _UID */
	enum i2c_speed speed;	/* Bus speed in Hz, default is I2C_SPEED_FAST */
	struct acpi_irq irq;	/* Interrupt */
};
