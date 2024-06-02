/* SPDX-License-Identifier: GPL-2.0-only */

#include <stddef.h>
#include <stdint.h>
#include <commonlib/bsd/helpers.h>
#include <device/pci_ops.h>
#include <static_devices.h>
#include <romstage_common.h>
#include <halt.h>

static void tune_fsb(void)
{
	if (!CONFIG(CPU_VIA_C7)) {
		printk(BIOS_WARNING,
		       "FSB settings are known for VIA C7 CPUs, P4 compat. is unknown.\n");
	}
	static const struct {
		uint8_t reg;
		uint8_t val;
	} fsb_settings[] = {
		{ 0x70, 0x33 },
		{ 0x71, 0x11 },
		{ 0x72, 0x33 },
		{ 0x73, 0x11 },
		{ 0x74, 0x20 },
		{ 0x75, 0x2e },
		{ 0x76, 0x64 },
		{ 0x77, 0x00 },
		{ 0x78, 0x44 },
		{ 0x79, 0xaa },
		{ 0x7a, 0x33 },
		{ 0x7b, 0xaa },
		{ 0x7c, 0x00 },
		{ 0x7e, 0x33 },
		{ 0x7f, 0x33 },
		{ 0x80, 0x44 },
		{ 0x81, 0x44 },
		{ 0x82, 0x44 },
	};
	for (size_t i = 0; i < ARRAY_SIZE(fsb_settings); ++i)
		pci_write_config8(_sdev_host_if, fsb_settings[i].reg, fsb_settings[i].val);
}

void __noreturn romstage_main(void)
{
	/* Allows access to all northbridge PCI devfn's */
	pci_write_config8(_sdev_host_ctrl, 0x4f, 0x01);

	tune_fsb();

	/* Needed for __noreturn */
	halt();
}
