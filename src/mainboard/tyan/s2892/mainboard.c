#include <console/console.h>
#include <device/device.h>
#include <arch/acpi.h>
#include <cpu/amd/powernow.h>
#include <arch/acpi.h>
#include <arch/acpigen.h>
#include <cpu/amd/amdk8_sysconf.h>

static void mainboard_acpi_fill_ssdt_generator(device_t device) {
	amd_generate_powernow(0, 0, 0);
}

static void mainboard_enable(device_t dev)
{
	dev->ops->acpi_fill_ssdt_generator = mainboard_acpi_fill_ssdt_generator;
}

struct chip_operations mainboard_ops = {
	.enable_dev = mainboard_enable,
};
