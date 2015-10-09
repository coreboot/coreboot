#include <stdint.h>
#include <string.h>
#include <lib.h>
#include <timestamp.h>
#include <arch/byteorder.h>
#include <arch/io.h>
#include <device/pci_def.h>
#include <device/pnp_def.h>
#include <cpu/x86/lapic.h>
#include <arch/acpi.h>
#include <console/console.h>
#include "northbridge/intel/sandybridge/sandybridge.h"
#include "northbridge/intel/sandybridge/raminit_native.h"
#include "southbridge/intel/bd82x6x/pch.h"
#include "southbridge/intel/bd82x6x/gpio.h"
#include <arch/cpu.h>
#include <cpu/x86/msr.h>
#include <cbfs.h>

void pch_enable_lpc(void)
{
	pci_write_config16(PCI_DEV(0, 0x1f, 0), 0x82, 0x3f0f);
	pci_write_config32(PCI_DEV(0, 0x1f, 0), 0x84, 0x000c0681);
	pci_write_config32(PCI_DEV(0, 0x1f, 0), 0x88, 0x000c1641);
	pci_write_config32(PCI_DEV(0, 0x1f, 0), 0x8c, 0x001c0301);
	pci_write_config32(PCI_DEV(0, 0x1f, 0), 0x90, 0x00fc0701);
	pci_write_config16(PCI_DEV(0, 0x1f, 0), 0x80, 0x0070);
	pci_write_config32(PCI_DEV(0, 0x1f, 0), 0xac, 0x80000000);
}

void rcba_config(void)
{
	/* Disable devices.  */
	RCBA32(0x3414) = 0x00000020;
	RCBA32(0x3418) = 0x1ffc0ee3;

}
const struct southbridge_usb_port mainboard_usb_ports[] = {
	{ 1, 0, -1 },
	{ 1, 0, -1 },
	{ 1, 0, -1 },
	{ 1, 0, -1 },
	{ 1, 0, -1 },
	{ 1, 0, -1 },
	{ 1, 0, -1 },
	{ 1, 0, -1 },
	{ 1, 0, -1 },
	{ 1, 0, -1 },
	{ 1, 0, -1 },
	{ 1, 0, -1 },
	{ 1, 0, -1 },
	{ 1, 0, -1 },
};

void mainboard_get_spd(spd_raw_data *spd)
{
	void *spd_file;
	size_t spd_file_len = 0;
	spd_file = cbfs_boot_map_with_leak("spd.bin", CBFS_TYPE_SPD,
						&spd_file_len);
	if (spd_file && spd_file_len >= 1024) {
		int i;
		for (i = 0; i < 4; i++)
			memcpy(&spd[i], spd_file + 256 * i, 128);
	}
}
