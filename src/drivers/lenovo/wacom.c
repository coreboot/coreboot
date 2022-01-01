/* SPDX-License-Identifier: GPL-2.0-only */

#include <types.h>
#include <console/console.h>
#include <acpi/acpigen.h>
#include <device/device.h>
#include <device/pnp.h>
#include <string.h>
#include "lenovo.h"
#include "drivers/i2c/at24rf08c/lenovo.h"

static const char tablet_numbers[][5] = {
	/* X60t. */
	"6363", "6364", "6365", "6366",
	"6367", "6368", "7762", "7763",
	"7764", "7767", "7768", "7769",
	/* X200t. */
	"7448", "7449", "7450", "7453",
	/* X201t. */
	"0053", "0831", "2985", "3093",
	"3113", "3144", "3239", "4184",
	"2263", "2266",
};

int
drivers_lenovo_is_wacom_present(void)
{
	const char *pn;
	int i;
	static int result = -1;
	struct device *superio;
	u8 sioid;

	if (result != -1)
		return result;

	if (CONFIG(DIGITIZER_PRESENT)) {
		printk (BIOS_INFO, "Digitizer state forced as present\n");
		return (result = 1);
	}

	if (CONFIG(DIGITIZER_ABSENT)) {
		printk (BIOS_INFO, "Digitizer state forced as absent\n");
		return (result = 0);
	}

	superio = dev_find_slot_pnp (0x164e, 3);
	if (!superio) {
		printk (BIOS_INFO, "No Super I/O, skipping wacom\n");
		return (result = 0);
	}

	/* Probe ID. */
	sioid = pnp_read_config(superio, 0x20);
	if (sioid == 0xff) {
		printk (BIOS_INFO, "Super I/O probe failed, skipping wacom\n");
		return (result = 0);
	}

	pn = lenovo_mainboard_partnumber();
	if (!pn)
		return (result = 0);
	printk (BIOS_DEBUG, "Lenovo P/N is %s\n", pn);
	for (i = 0; i < ARRAY_SIZE (tablet_numbers); i++)
		if (memcmp (tablet_numbers[i], pn, 4) == 0) {
			printk (BIOS_DEBUG, "Lenovo P/N %s is a tablet\n", pn);
			return (result = 1);
		}
	printk (BIOS_DEBUG, "Lenovo P/N %s is not a tablet\n", pn);
	return (result = 0);
}

void
drivers_lenovo_serial_ports_ssdt_generate(const char *scope,
					  int have_dock_serial)
{
	acpigen_write_scope(scope);

	if (drivers_lenovo_is_wacom_present()) {
		acpigen_write_device("DTR");

		acpigen_write_name("_HID");
		acpigen_emit_eisaid("WACF004");

		acpigen_write_name("_CRS");

		acpigen_write_resourcetemplate_header();
		acpigen_write_io16(0x200, 0x200, 1, 8, 1);
		acpigen_write_irq((1 << 5));

		acpigen_write_resourcetemplate_footer();

		acpigen_write_STA(0xf);

		acpigen_pop_len();
	}

	if (have_dock_serial) {
		acpigen_write_device("COMA");

		acpigen_write_name("_HID");
		acpigen_emit_eisaid("PNP0501");
		acpigen_write_name("_UID");
		/* Byte */
		acpigen_write_byte(0x2);

		acpigen_write_name("_CRS");

		acpigen_write_resourcetemplate_header();
		acpigen_write_io16(0x3f8, 0x3f8, 1, 8, 1);
		acpigen_write_irq(1 << 4);

		acpigen_write_resourcetemplate_footer();

		acpigen_write_STA(0xf);

		acpigen_pop_len();
	}

	acpigen_pop_len();
}
