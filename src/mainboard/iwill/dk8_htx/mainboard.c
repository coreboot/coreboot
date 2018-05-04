#include <console/console.h>
#include <device/device.h>
#include <arch/acpi.h>
#include <cpu/amd/powernow.h>
#include <arch/acpi.h>
#include <arch/acpigen.h>
#include <cpu/amd/amdk8_sysconf.h>
#include "mainboard.h"

static void mainboard_enable(struct device *dev)
{
	dev->ops->write_acpi_tables = mainboard_write_acpi_tables;
}

struct chip_operations mainboard_ops = {
	.enable_dev = mainboard_enable,
};
