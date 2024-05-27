/* SPDX-License-Identifier: GPL-2.0-only */

#include <amdblocks/acpimmio.h>
#include <device/mmio.h>
#include <device/pci_ops.h>
#include <console/console.h>
#include <device/device.h>
#include <device/pci_def.h>
#include <gpio.h>
#include <identity.h>
#include <southbridge/amd/pi/hudson/hudson.h>
#include <southbridge/amd/pi/hudson/pci_devs.h>
#include <southbridge/amd/pi/hudson/amd_pci_int_defs.h>
#include <northbridge/amd/agesa/agesa_helper.h>
#include <northbridge/amd/pi/00730F01/pci_devs.h>
#include <southbridge/amd/common/amd_pci_util.h>
#include <superio/nuvoton/nct5104d/nct5104d.h>
#include <smbios.h>
#include <stdio.h>
#include <string.h>
#include <AGESA.h>
#include "gpio_ftns.h"

#define SPD_SIZE  128
#define PM_RTC_CONTROL	    0x56
#define PM_S_STATE_CONTROL  0xBA

/***********************************************************
 * These arrays set up the FCH PCI_INTR registers 0xC00/0xC01.
 * This table is responsible for physically routing the PIC and
 * IOAPIC IRQs to the different PCI devices on the system.  It
 * is read and written via registers 0xC00/0xC01 as an
 * Index/Data pair.  These values are chipset and mainboard
 * dependent and should be updated accordingly.
 *
 * These values are used by the PCI configuration space,
 * MP Tables.  TODO: Make ACPI use these values too.
 */
static const u8 mainboard_picr_data[FCH_INT_TABLE_SIZE] = {
	[0 ... FCH_INT_TABLE_SIZE-1] = 0x1F,
	/* INTA# - INTH# */
	[0x00] = 0x03,0x03,0x05,0x07,0x0B,0x0A,0x1F,0x1F,
	/* Misc-nil,0,1,2, INT from Serial irq */
	[0x08] = 0xFA,0xF1,0x00,0x00,0x1F,0x1F,0x1F,0x1F,
	/* SCI, SMBUS0, ASF, HDA, FC, RSVD, PerMon, SD */
	[0x10] = 0x1F,0x1F,0x1F,0x1F,0x1F,0x1F,0x1F,0x1F,
	[0x18] = 0x1F,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	/* IMC INT0 - 5 */
	[0x20] = 0x1F,0x1F,0x1F,0x1F,0x1F,0x1F,0x00,0x00,
	[0x28] = 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	/* USB Devs 18/19/22 INTA-C */
	[0x30] = 0x05,0x1F,0x05,0x1F,0x04,0x1F,0x1F,0x1F,
	[0x38] = 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	/* SATA */
	[0x40] = 0x1F,0x1F,0x00,0x00,0x00,0x00,0x00,0x00,
	[0x48] = 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	[0x50] = 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	[0x58] = 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	[0x60] = 0x00,0x00,0x1F
};

static const u8 mainboard_intr_data[FCH_INT_TABLE_SIZE] = {
	[0 ... FCH_INT_TABLE_SIZE-1] = 0x1F,
	/* INTA# - INTH# */
	[0x00] = 0x10,0x10,0x12,0x13,0x14,0x15,0x1F,0x1F,
	/* Misc-nil,0,1,2, INT from Serial irq */
	[0x08] = 0x00,0x00,0x00,0x00,0x1F,0x1F,0x1F,0x1F,
	/* SCI, SMBUS0, ASF, HDA, FC, RSVD, PerMon, SD */
	[0x10] = 0x09,0x1F,0x1F,0x1F,0x1F,0x1F,0x1F,0x10,
	[0x18] = 0x1F,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	/* IMC INT0 - 5 */
	[0x20] = 0x05,0x1F,0x1F,0x1F,0x1F,0x1F,0x00,0x00,
	[0x28] = 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	/* USB Devs 18/19/20/22 INTA-C */
	[0x30] = 0x12,0x1F,0x12,0x1F,0x12,0x1F,0x1F,0x00,
	[0x38] = 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	/* SATA */
	[0x40] = 0x1F,0x13,0x00,0x00,0x00,0x00,0x00,0x00,
	[0x48] = 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	[0x50] = 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	[0x58] = 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	[0x60] = 0x00,0x00,0x1F
};

/*
 * This table defines the index into the picr/intr_data
 * tables for each device.  Any enabled device and slot
 * that uses hardware interrupts should have an entry
 * in this table to define its index into the FCH
 * PCI_INTR register 0xC00/0xC01.  This index will define
 * the interrupt that it should use.  Putting PIRQ_A into
 * the PIN A index for a device will tell that device to
 * use PIC IRQ 10 if it uses PIN A for its hardware INT.
 */
static const struct pirq_struct mainboard_pirq_data[] = {
	/* {PCI_devfn,	{PIN A, PIN B, PIN C, PIN D}}, */
	{GFX_DEVFN,	{PIRQ_A, PIRQ_NC, PIRQ_NC, PIRQ_NC}},		/* VGA:		01.0 */
	{ACTL_DEVFN,	{PIRQ_NC, PIRQ_B, PIRQ_NC, PIRQ_NC}},		/* Audio:	01.1 */
	{NB_PCIE_PORT1_DEVFN,	{PIRQ_A, PIRQ_B, PIRQ_C, PIRQ_D}},	/* mPCIe/NIC:	02.1 */
	{NB_PCIE_PORT2_DEVFN,	{PIRQ_B, PIRQ_C, PIRQ_D, PIRQ_A}},	/* NIC:		02.2 */
	{NB_PCIE_PORT3_DEVFN,	{PIRQ_C, PIRQ_D, PIRQ_A, PIRQ_B}},	/* NIC:		02.3 */
	{NB_PCIE_PORT4_DEVFN,	{PIRQ_D, PIRQ_A, PIRQ_B, PIRQ_C}},	/* NIC:		02.4 */
	{NB_PCIE_PORT5_DEVFN,	{PIRQ_A, PIRQ_B, PIRQ_C, PIRQ_D}},	/* mPCIe:	02.5 */
	{XHCI_DEVFN,	{PIRQ_C, PIRQ_NC, PIRQ_NC, PIRQ_NC}},		/* XHCI:	10.0 */
	{SATA_DEVFN,	{PIRQ_SATA, PIRQ_NC, PIRQ_NC, PIRQ_NC}},	/* SATA:	11.0 */
	{OHCI1_DEVFN,	{PIRQ_OHCI1, PIRQ_NC, PIRQ_NC, PIRQ_NC}},	/* OHCI1:	12.0 */
	{EHCI1_DEVFN,	{PIRQ_NC, PIRQ_EHCI1, PIRQ_NC, PIRQ_NC}},	/* EHCI1:	12.2 */
	{OHCI2_DEVFN,	{PIRQ_OHCI2, PIRQ_NC, PIRQ_NC, PIRQ_NC}},	/* OHCI2:	13.0 */
	{EHCI2_DEVFN,	{PIRQ_NC, PIRQ_EHCI2, PIRQ_NC, PIRQ_NC}},	/* EHCI2:	13.2 */
	{SMBUS_DEVFN,	{PIRQ_SMBUS, PIRQ_NC, PIRQ_NC, PIRQ_NC}},	/* SMBUS:	14.0 */
	{HDA_DEVFN,	{PIRQ_HDA, PIRQ_NC, PIRQ_NC, PIRQ_NC}},		/* HDA:		14.2 */
	{SD_DEVFN,	{PIRQ_SD, PIRQ_NC, PIRQ_NC, PIRQ_NC}},		/* SD:		14.7 */
	{OHCI3_DEVFN,	{PIRQ_OHCI3, PIRQ_NC, PIRQ_NC, PIRQ_NC}},	/* OHCI3:	16.0 (same device as xHCI 10.0) */
	{EHCI3_DEVFN,	{PIRQ_NC, PIRQ_EHCI3, PIRQ_NC, PIRQ_NC}},	/* EHCI3:	16.2 (same device as xHCI 10.1) */
};

/* PIRQ Setup */
static void pirq_setup(void)
{
	pirq_data_ptr = mainboard_pirq_data;
	pirq_data_size = ARRAY_SIZE(mainboard_pirq_data);
	intr_data_ptr = mainboard_intr_data;
	picr_data_ptr = mainboard_picr_data;
}

/* Wrapper to enable GPIO/UART devices under menuconfig. Revisit
 * once configuration file format for SPI flash storage is complete.
 */
#define SIO_PORT 0x2e

static void config_gpio_mux(void)
{
	struct device *uart, *gpio;

	uart = dev_find_slot_pnp(SIO_PORT, NCT5104D_SP3);
	gpio = dev_find_slot_pnp(SIO_PORT, NCT5104D_GPIO0);
	if (uart)
		uart->enabled = CONFIG(APU2_PINMUX_UART_C);
	if (gpio)
		gpio->enabled = CONFIG(APU2_PINMUX_GPIO0);

	uart = dev_find_slot_pnp(SIO_PORT, NCT5104D_SP4);
	gpio = dev_find_slot_pnp(SIO_PORT, NCT5104D_GPIO1);
	if (uart)
		uart->enabled = CONFIG(APU2_PINMUX_UART_D);
	if (gpio)
		gpio->enabled = CONFIG(APU2_PINMUX_GPIO1);
}

/**********************************************
 * enable the dedicated function in mainboard.
 **********************************************/
#if CONFIG(GENERATE_SMBIOS_TABLES)
static int mainboard_smbios_type16(DMI_INFO *agesa_dmi, int *handle,
				 unsigned long *current)
{
	const u32 max_capacity = get_spd_offset() ? 4 : 2; /* 4GB or 2GB variant */

	struct smbios_type16 *t = smbios_carve_table(*current, SMBIOS_PHYS_MEMORY_ARRAY,
						     sizeof(*t), *handle);

	t->use = MEMORY_ARRAY_USE_SYSTEM;
	t->location = MEMORY_ARRAY_LOCATION_SYSTEM_BOARD;
	t->memory_error_correction = agesa_dmi->T16.MemoryErrorCorrection;
	t->maximum_capacity = max_capacity * 1024 * 1024;
	t->memory_error_information_handle = 0xfffe;
	t->number_of_memory_devices = 1;

	const int len = smbios_full_table_len(&t->header, t->eos);
	*current += len;
	return len;
}

static int mainboard_smbios_type17(DMI_INFO *agesa_dmi, int *handle,
				 unsigned long *current)
{
	struct smbios_type17 *t = smbios_carve_table(*current, SMBIOS_MEMORY_DEVICE,
						     sizeof(*t), *handle + 1);

	t->phys_memory_array_handle = *handle;
	t->memory_error_information_handle = 0xfffe;
	t->total_width = agesa_dmi->T17[0][0][0].TotalWidth;
	t->data_width = agesa_dmi->T17[0][0][0].DataWidth;
	t->size = agesa_dmi->T17[0][0][0].MemorySize;
	t->form_factor = agesa_dmi->T17[0][0][0].FormFactor;
	t->device_set = agesa_dmi->T17[0][0][0].DeviceSet;
	t->device_locator = smbios_add_string(t->eos,
				agesa_dmi->T17[0][0][0].DeviceLocator);
	t->bank_locator = smbios_add_string(t->eos,
				agesa_dmi->T17[0][0][0].BankLocator);
	t->memory_type = agesa_dmi->T17[0][0][0].MemoryType;
	t->type_detail = *(u16 *)&agesa_dmi->T17[0][0][0].TypeDetail;
	t->speed = agesa_dmi->T17[0][0][0].Speed;
	t->manufacturer = agesa_dmi->T17[0][0][0].ManufacturerIdCode;
	t->serial_number = smbios_add_string(t->eos,
				agesa_dmi->T17[0][0][0].SerialNumber);
	t->part_number = smbios_add_string(t->eos,
				agesa_dmi->T17[0][0][0].PartNumber);
	t->attributes = agesa_dmi->T17[0][0][0].Attributes;
	t->extended_size = agesa_dmi->T17[0][0][0].ExtSize;
	t->clock_speed = agesa_dmi->T17[0][0][0].ConfigSpeed;
	t->minimum_voltage = 1500; /* From SPD: 1.5V */
	t->maximum_voltage = 1500;

	const int len = smbios_full_table_len(&t->header, t->eos);
	*current += len;
	return len;
}

static int mainboard_smbios_data(struct device *dev, int *handle,
				 unsigned long *current)
{
	DMI_INFO *agesa_dmi;
	int len = 0;

	agesa_dmi = agesawrapper_getlateinitptr(PICK_DMI);

	if (!agesa_dmi)
		return len;

	len += mainboard_smbios_type16(agesa_dmi, handle, current);
	len += mainboard_smbios_type17(agesa_dmi, handle, current);

	*handle += 2;

	return len;
}
#endif

static void mainboard_enable(struct device *dev)
{
	/* Maintain this text unchanged for manufacture process. */
	printk(BIOS_INFO, "Mainboard %s Enable.\n", mainboard_part_number);

	config_gpio_mux();

	//
	// Enable the RTC output
	//
	pm_write16(PM_RTC_CONTROL, pm_read16(PM_RTC_CONTROL) | (1 << 11));

	//
	// Enable power on from WAKE#
	//
	pm_write16(PM_S_STATE_CONTROL, pm_read16(PM_S_STATE_CONTROL) | (1 << 14));

	/* Initialize the PIRQ data structures for consumption */
	pirq_setup();
#if CONFIG(GENERATE_SMBIOS_TABLES)
	dev->ops->get_smbios_data = mainboard_smbios_data;
#endif
}

static void mainboard_final(void *chip_info)
{
	//
	// Turn off LED 2 and LED 3
	//
	gpio_set(GPIO_58, 1);
	gpio_set(GPIO_59, 1);
}

/*
 * We will stuff a modified version of the first NICs (BDF 1:0.0) MAC address
 * into the smbios serial number location.
 */
const char *smbios_mainboard_serial_number(void)
{
	static char serial[10];
	struct device *dev;
	uintptr_t bar10;
	u32 mac_addr = 0;
	int i;

	/* Already initialized. */
	if (serial[0] != 0)
		return serial;

	dev = pcidev_on_root(2, 2);
	if (dev)
		dev = pcidev_path_behind(dev->downstream, PCI_DEVFN(0, 0));
	if (!dev)
		return serial;

	/* Read in the last 3 bytes of NIC's MAC address. */
	bar10 = pci_read_config32(dev, PCI_BASE_ADDRESS_0);
	bar10 &= 0xFFFE0000;
	bar10 += 0x5400;
	for (i = 3; i < 6; i++) {
		mac_addr <<= 8;
		mac_addr |= read8((u8 *)bar10 + i);
	}
	mac_addr &= 0x00FFFFFF;
	mac_addr /= 4;
	mac_addr -= 64;

	snprintf(serial, sizeof(serial), "%d", mac_addr);
	return serial;
}

/*
 * We will stuff the memory size into the smbios sku location.
 */
const char *smbios_system_sku(void)
{
	static char sku[5];
	if (sku[0] != 0)
		return sku;

	if (!get_spd_offset())
		snprintf(sku, sizeof(sku), "2 GB");
	else
		snprintf(sku, sizeof(sku), "4 GB");
	return sku;
}

struct chip_operations mainboard_ops = {
	.enable_dev = mainboard_enable,
	.final = mainboard_final,
};
