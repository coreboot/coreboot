/* SPDX-License-Identifier: GPL-2.0-only */

#include <amdblocks/acpimmio.h>

#include <console/console.h>
#include <device/device.h>
#include <device/mmio.h>
#include <device/pci_ops.h>
#include <device/pci_def.h>
#include <southbridge/amd/common/amd_pci_util.h>
#include <smbios.h>
#include <string.h>
#include <southbridge/amd/cimx/sb800/pci_devs.h>
#include <northbridge/amd/agesa/agesa_helper.h>
#include <northbridge/amd/agesa/family14/pci_devs.h>
#include <superio/nuvoton/nct5104d/nct5104d.h>
#include "gpio_ftns.h"
#include <AGESA.h>
#include <AMD.h>
#include <southbridge/amd/cimx/sb800/SBPLATFORM.h>

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
 *
 * The PCI INTA/B/C/D pins are connected to
 * FCH pins INTE/F/G/H on the schematic so these need
 * to be routed as well.
 */
static const u8 mainboard_picr_data[FCH_INT_TABLE_SIZE] = {
	/* INTA# - INTH# */
	[0x00] = 0x0A,0x0B,0x0A,0x0B,0x0A,0x0B,0x0A,0x0B,
	/* Misc-nil,0,1,2, INT from Serial irq */
	[0x08] = 0x00,0xF1,0x00,0x00,0x1F,0x1F,0x1F,0x1F,
	/* SCI, SMBUS0, ASF, HDA, FC, GEC, PerfMon */
	[0x10] = 0x1F,0x1F,0x1F,0x0A,0x1F,0x1F,0x1F,
	/* IMC INT0 - 5 */
	[0x20] = 0x1F,0x1F,0x1F,0x1F,0x1F,0x1F,
	/* USB Devs 18/19/20/22 INTA-C */
	[0x30] = 0x0A,0x0B,0x0A,0x0B,0x0A,0x0B,0x0A,
	/* IDE, SATA */
	[0x40] = 0x0B,0x0B,
	/* GPPInt0 - 3 */
	[0x50] = 0x0A,0x0B,0x0A,0x0B
};

static const u8 mainboard_intr_data[FCH_INT_TABLE_SIZE] = {
	/* INTA# - INTH# */
	[0x00] = 0x10,0x11,0x12,0x13,0x14,0x15,0x16,0x17,
	/* Misc-nil,0,1,2, INT from Serial irq */
	[0x08] = 0x00,0x00,0x00,0x00,0x1F,0x1F,0x1F,0x1F,
	/* SCI, SMBUS0, ASF, HDA, FC, GEC, PerMon */
	[0x10] = 0x09,0x1F,0x1F,0x10,0x1F,0x12,0x1F,
	/* IMC INT0 - 5 */
	[0x20] = 0x1F,0x1F,0x1F,0x1F,0x1F,0x1F,
	/* USB Devs 18/19/22/20 INTA-C */
	[0x30] = 0x12,0x11,0x12,0x11,0x12,0x11,0x12,
	/* IDE, SATA */
	[0x40] = 0x11,0x13,
	/* GPPInt0 - 3 */
	[0x50] = 0x10,0x11,0x12,0x13
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
/*
 * The PCI slot INTA/B/C/D connected to PIRQE/F/G/H
 * but because of PCI INT_PIN swizzle isn't implemented to match
 * the IDSEL (dev 3) of the slot, the table is adjusted for the
 * swizzle and INTA is connected to PIRQH so PINA/B/C/D on
 * off-chip devices should get mapped to PIRQH/E/F/G.
 */
static const struct pirq_struct mainboard_pirq_data[] = {
	/* {PCI_devfn,        {PIN A, PIN B, PIN C, PIN D}}, */
	{GFX_DEVFN,           {PIRQ_A, PIRQ_B, PIRQ_NC, PIRQ_NC}},      /* VGA:       01.0 */
	{NB_PCIE_PORT1_DEVFN, {PIRQ_A, PIRQ_B, PIRQ_C, PIRQ_D}},        /* NIC:       04.0 */
	{NB_PCIE_PORT2_DEVFN, {PIRQ_A, PIRQ_B, PIRQ_C, PIRQ_D}},        /* NIC:       05.0 */
	{NB_PCIE_PORT3_DEVFN, {PIRQ_A, PIRQ_B, PIRQ_C, PIRQ_D}},        /* NIC:       06.0 */
	{NB_PCIE_PORT4_DEVFN, {PIRQ_A, PIRQ_B, PIRQ_C, PIRQ_D}},        /* miniPCIe:  07.0 */
	{SATA_DEVFN,          {PIRQ_SATA, PIRQ_NC, PIRQ_NC, PIRQ_NC}},  /* SATA:      11.0 */
	{OHCI1_DEVFN,         {PIRQ_OHCI1, PIRQ_NC, PIRQ_NC, PIRQ_NC}}, /* OHCI1:     12.0 */
	{EHCI1_DEVFN,         {PIRQ_NC, PIRQ_EHCI1, PIRQ_NC, PIRQ_NC}}, /* EHCI1:     12.2 */
	{OHCI2_DEVFN,         {PIRQ_OHCI2, PIRQ_NC, PIRQ_NC, PIRQ_NC}}, /* OHCI2:     13.0 */
	{EHCI2_DEVFN,         {PIRQ_NC, PIRQ_EHCI2, PIRQ_NC, PIRQ_NC}}, /* EHCI2:     13.2 */
	{SMBUS_DEVFN,         {PIRQ_SMBUS, PIRQ_NC, PIRQ_NC, PIRQ_NC}}, /* SMBUS:     14.0 */
	{IDE_DEVFN,           {PIRQ_NC, PIRQ_IDE, PIRQ_NC, PIRQ_NC}},   /* IDE:       14.1 */
	{HDA_DEVFN,           {PIRQ_HDA, PIRQ_NC, PIRQ_NC, PIRQ_NC}},   /* HDA:       14.2 */
	{SB_PCI_PORT_DEVFN,   {PIRQ_H, PIRQ_E, PIRQ_F, PIRQ_G}},        /* PCI bdg:   14.4 */
	{OHCI4_DEVFN,         {PIRQ_NC, PIRQ_NC, PIRQ_OHCI4, PIRQ_NC}}, /* OHCI4:     14.5 */
	{SB_PCIE_PORT1_DEVFN, {PIRQ_A, PIRQ_B, PIRQ_C, PIRQ_D}},        /* miniPCIe:  15.0 */
	{OHCI3_DEVFN,         {PIRQ_OHCI3, PIRQ_NC, PIRQ_NC, PIRQ_NC}}, /* OHCI3:     16.0 */
	{EHCI3_DEVFN,         {PIRQ_NC, PIRQ_EHCI3, PIRQ_NC, PIRQ_NC}}, /* EHCI3:     16.2 */
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
		uart->enabled = CONFIG(APU1_PINMUX_UART_C);
	if (gpio)
		gpio->enabled = CONFIG(APU1_PINMUX_GPIO0);

	uart = dev_find_slot_pnp(SIO_PORT, NCT5104D_SP4);
	gpio = dev_find_slot_pnp(SIO_PORT, NCT5104D_GPIO1);
	if (uart)
		uart->enabled = CONFIG(APU1_PINMUX_UART_D);
	if (gpio)
		gpio->enabled = CONFIG(APU1_PINMUX_GPIO1);
}

static void pnp_raw_resource(struct device *dev, u8 reg, u8 val)
{
	struct resource *res;
	res = new_resource(dev, reg);
	res->base = val;
	res->size = 0;
	res->flags |= IORESOURCE_IRQ | IORESOURCE_ASSIGNED;
}

static void config_addon_uart(void)
{
	struct device *uart;

	uart = dev_find_slot_pnp(SIO_PORT, NCT5104D_SP3);
	if (uart && uart->enabled && CONFIG(UART_C_RS485))
		pnp_raw_resource(uart, 0xf2, 0x12);

	uart = dev_find_slot_pnp(SIO_PORT, NCT5104D_SP4);
	if (uart && uart->enabled && CONFIG(UART_D_RS485))
		pnp_raw_resource(uart, 0xf2, 0x12);
}

/**********************************************
 * Enable the dedicated functions of the board.
 **********************************************/
#if CONFIG(GENERATE_SMBIOS_TABLES)
static int mainboard_smbios_type16(DMI_INFO *agesa_dmi, int *handle, unsigned long *current)
{
	struct smbios_type16 *t;
	u32 max_capacity;
	int len;

	t = (struct smbios_type16 *)*current;
	len = sizeof(struct smbios_type16);
	memset(t, 0, len);
	max_capacity = get_spd_offset() ? 4 : 2; /* 4GB or 2GB variant */

	t->type = SMBIOS_PHYS_MEMORY_ARRAY;
	t->handle = *handle;
	t->length = len - 2;
	t->type = SMBIOS_PHYS_MEMORY_ARRAY;
	t->use = MEMORY_ARRAY_USE_SYSTEM;
	t->location = MEMORY_ARRAY_LOCATION_SYSTEM_BOARD;
	t->memory_error_correction = agesa_dmi->T16.MemoryErrorCorrection;
	t->maximum_capacity = max_capacity * 1024 * 1024;
	t->memory_error_information_handle = 0xfffe;
	t->number_of_memory_devices = 1;

	*current += len;

	return len;
}

static int mainboard_smbios_type17(DMI_INFO *agesa_dmi, int *handle, unsigned long *current)
{
	struct smbios_type17 *t;
	int len;

	t = (struct smbios_type17 *)*current;
	memset(t, 0, sizeof(struct smbios_type17));

	t->type = SMBIOS_MEMORY_DEVICE;
	t->length = sizeof(struct smbios_type17) - 2;
	t->handle = *handle + 1;
	t->phys_memory_array_handle = *handle;
	t->memory_error_information_handle = 0xfffe;
	t->total_width = agesa_dmi->T17[0][0][0].TotalWidth;
	t->data_width = agesa_dmi->T17[0][0][0].DataWidth;
	t->size = agesa_dmi->T17[0][0][0].MemorySize;
	/* AGESA DMI returns form factor = 0, override it with SPD value */
	t->form_factor = MEMORY_FORMFACTOR_SODIMM;
	t->device_set = agesa_dmi->T17[0][0][0].DeviceSet;
	t->device_locator = smbios_add_string(t->eos, agesa_dmi->T17[0][0][0].DeviceLocator);
	t->bank_locator = smbios_add_string(t->eos, agesa_dmi->T17[0][0][0].BankLocator);
	t->memory_type = agesa_dmi->T17[0][0][0].MemoryType;
	t->type_detail = *(u16 *)&agesa_dmi->T17[0][0][0].TypeDetail;
	t->speed = agesa_dmi->T17[0][0][0].Speed;
	t->manufacturer = agesa_dmi->T17[0][0][0].ManufacturerIdCode;
	t->serial_number = smbios_add_string(t->eos, agesa_dmi->T17[0][0][0].SerialNumber);
	t->part_number = smbios_add_string(t->eos, agesa_dmi->T17[0][0][0].PartNumber);
	t->attributes = agesa_dmi->T17[0][0][0].Attributes;
	t->extended_size = agesa_dmi->T17[0][0][0].ExtSize;
	t->clock_speed = agesa_dmi->T17[0][0][0].ConfigSpeed;
	t->minimum_voltage = 1500; /* From SPD: 1.5V */
	t->maximum_voltage = 1500;

	len = t->length + smbios_string_table_len(t->eos);
	*current += len;

	return len;
}

static int mainboard_smbios_data(struct device *dev, int *handle,
				 unsigned long *current)
{
	DMI_INFO *agesa_dmi;
	int len;

	agesa_dmi = agesawrapper_getlateinitptr(PICK_DMI);

	if (!agesa_dmi)
		return 0;

	len = mainboard_smbios_type16(agesa_dmi, handle, current);
	len += mainboard_smbios_type17(agesa_dmi, handle, current);

	*handle += 2;

	return len;
}
#endif

static void mainboard_enable(struct device *dev)
{
	printk(BIOS_INFO, "Mainboard " CONFIG_MAINBOARD_PART_NUMBER " Enable.\n");

	config_gpio_mux();
	config_addon_uart();

	/* Power off unused clock pins of GPP PCIe devices
	 * GPP CLK0-2 are connected to the 3 ethernet chips
	 * GPP CLK3-4 are connected to the miniPCIe slots
	 */
	misc_write8(0, 0x21);
	misc_write8(1, 0x43);
	/* GPP CLK5 is only connected to test pads -> disable */
	misc_write8(2, 0x05);
	/* disable unconnected GPP CLK6-8 and SLT_GFX_CLK */
	misc_write8(3, 0);
	misc_write8(4, 0);

	/* Initialize the PIRQ data structures for consumption */
	pirq_setup();
#if CONFIG(GENERATE_SMBIOS_TABLES)
	dev->ops->get_smbios_data = mainboard_smbios_data;
#endif
}

/*
 * We will stuff a modified version of the first NICs (BDF 1:0.0) MAC address
 * into the smbios serial number location.
 */
const char *smbios_mainboard_serial_number(void)
{
	static char serial[10];
	struct device *dev;
	uintptr_t bar18;
	u32 mac_addr = 0;
	int i;

	/* Already initialized. */
	if (serial[0] != 0)
		return serial;

	dev = pcidev_on_root(4, 0);
	if (dev)
		dev = pcidev_path_behind(dev->link_list, PCI_DEVFN(0, 0));
	if (!dev)
		return serial;

	/* Read in the last 3 bytes of NIC's MAC address. */
	bar18 = pci_read_config32(dev, PCI_BASE_ADDRESS_2);
	bar18 &= 0xFFFFFFF0;
	for (i = 3; i < 6; i++) {
		mac_addr <<= 8;
		mac_addr |= read8((u8 *)bar18 + i);
	}
	mac_addr &= 0x00FFFFFF;
	mac_addr /= 4;
	mac_addr -= 64;

	snprintf(serial, sizeof(serial), "%d", mac_addr);
	return serial;
}

/*
 * Set up "Over Current Control 1" (reg 0x58) on the first OHCI device.
 * The remaining ports on the second device are for mcpie2/sdcard and
 * can stay at the power-on default value.
 *
 * The schematic shows this transposed mapping for the first device:
 * chipset port 0 -> port 1 (j12 external 2, usboc0#)
 * chipset port 1 -> port 4 (j17 mpcie1)
 * chipset port 2 -> port 2 (j14 header row1, usboc1#)
 * chipset port 3 -> port 3 (j14 header row2, usboc1#)
 * chipset port 4 -> port 0 (j12 external 1. usboc0#)
 *
 * Register mapping:
 * bit0-3: Mapping for HS Port 0
 * bit4-7: Mapping for HS Port 1
 * bit8-11: Mapping for HS Port 2
 * bit12-15: Mapping for HS Port 3
 * bit16-19: Mapping for HS Port 4
 * bit20-31: Reserved (0)
 *
 * power-on default: 0xfffff
 * A value >7 will disable the overcurrent detection.
 */
static void usb_oc_setup(void)
{
	struct device *dev = pcidev_on_root(0x12, 0);

	pci_write_config32(dev, 0x58, 0x011f0);
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

static void mainboard_final(void *chip_info)
{
	printk(BIOS_INFO, "Mainboard " CONFIG_MAINBOARD_PART_NUMBER " Final.\n");

	/*
	 * LED1/D7/GPIO_189 should be 0
	 * LED2/D6/GPIO_190 should be 1
	 * LED3/D5/GPIO_191 should be 1
	 */
	configure_gpio(GPIO_189, GPIO_FTN_1, GPIO_OUTPUT | GPIO_DATA_LOW);
	configure_gpio(GPIO_190, GPIO_FTN_1, GPIO_OUTPUT | GPIO_DATA_HIGH);
	configure_gpio(GPIO_191, GPIO_FTN_1, GPIO_OUTPUT | GPIO_DATA_HIGH);
	usb_oc_setup();
}

struct chip_operations mainboard_ops = {
	.enable_dev = mainboard_enable,
	.final = mainboard_final,
};
