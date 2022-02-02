/* SPDX-License-Identifier: GPL-2.0-only */

#include "i210.h"
#include <device/device.h>
#include <console/console.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <device/pci_ops.h>
#include <device/pci_def.h>
#include <string.h>
#include <types.h>
#include <delay.h>

/* This is a private function to wait for a bit mask in a given register */
/* To avoid endless loops, a time-out is implemented here. */
static int wait_done(uint32_t *reg, uint32_t mask)
{
	uint32_t timeout = I210_POLL_TIMEOUT_US;

	while (!(*reg & mask)) {
		udelay(1);
		if (!--timeout)
			return I210_NOT_READY;
	}
	return I210_SUCCESS;
}

/** \brief This function can read the configuration space of the MACPHY
 *         For this purpose, the EEPROM interface is used. No direct access
 *         to the flash memory will be done.
 * @param *dev     Pointer to the PCI device of this MACPHY
 * @param address  Address inside the flash where reading will start
 * @param count    Number of words (16 bit values) to read
 * @param *buffer  Pointer to the buffer where to store read data
 * @return void    I210_NO_ERROR or an error code
 */
static uint32_t read_flash(struct device *dev, uint32_t address,
			   uint32_t count, uint16_t *buffer)
{
	uint32_t bar;
	uint32_t *eeprd;
	uint32_t i;

	/* Get the BAR to memory mapped space*/
	bar = pci_read_config32(dev, PCI_BASE_ADDRESS_0);
	if ((!bar) || ((address + count) > 0x40))
		return I210_INVALID_PARAM;
	eeprd = (uint32_t *)(bar + I210_REG_EEREAD);
	/* Prior to start ensure flash interface is ready by checking DONE-bit */
	if (wait_done(eeprd, I210_DONE))
		return I210_NOT_READY;

	/*OK, interface is ready, we can use it now */
	for (i = 0; i < count; i++) {
		/* To start a read cycle write desired address in bits 12..2 */
		*eeprd = ((address + i) << 2) & 0x1FFC;
		/* Wait until read is done */
		if (wait_done(eeprd, I210_DONE))
			return I210_READ_ERROR;
		/* Here, we can read back desired word in bits 31..16 */
		buffer[i] = (*eeprd & 0xffff0000) >> 16;
	}
	return I210_SUCCESS;
}

/** \brief This function computes the checksum for the configuration space.
 *         The address range for the checksum is 0x00..0x3e.
 * @param *dev      Pointer to the PCI device of this MACPHY
 * @param *checksum Pointer to the buffer where to store the checksum
 * @return void     I210_NO_ERROR or an error code
 */
static uint32_t compute_checksum(struct device *dev, uint16_t *checksum)
{
	uint16_t eep_data[0x40];
	uint32_t i;

	/* First read back data to compute the checksum for */
	if (read_flash(dev, 0, 0x3f, eep_data))
		return I210_READ_ERROR;
	/* The checksum is computed in that way that after summarize all the */
	/* data from word address 0 to 0x3f the result is 0xBABA. */
	*checksum = 0;
	for (i = 0; i < 0x3f; i++)
		*checksum += eep_data[i];
	*checksum = I210_TARGET_CHECKSUM - *checksum;
	return I210_SUCCESS;
}

/** \brief This function can write the configuration space of the MACPHY
 *         For this purpose, the EEPROM interface is used. No direct access
 *         to the flash memory will be done. This function will update
 *         the checksum after a value was changed.
 * @param *dev    Pointer to the PCI device of this MACPHY
 * @param address Address inside the flash where writing will start
 * @param count   Number of words (16 bit values) to write
 * @param *buffer Pointer to the buffer where data to write is stored in
 * @return void   I210_NO_ERROR or an error code
 */
static uint32_t write_flash(struct device *dev, uint32_t address,
			    uint32_t count, uint16_t *buffer)
{
	uint32_t bar;
	uint32_t *eepwr;
	uint32_t *eectrl;
	uint16_t checksum;
	uint32_t i;

	/* Get the BAR to memory mapped space */
	bar = pci_read_config32(dev, 0x10);
	if ((!bar) || ((address + count) > 0x40))
		return I210_INVALID_PARAM;
	eepwr = (uint32_t *)(bar + I210_REG_EEWRITE);
	eectrl = (uint32_t *)(bar + I210_REG_EECTRL);
	/* Prior to start ensure flash interface is ready by checking DONE-bit */
	if (wait_done(eepwr, I210_DONE))
		return I210_NOT_READY;

	/* OK, interface is ready, we can use it now */
	for (i = 0; i < count; i++) {
		/* To start a write cycle write desired address in bits 12..2 */
		/* and data to write in bits 31..16 into EEWRITE-register */
		*eepwr = ((((address + i) << 2) & 0x1FFC) | (buffer[i] << 16));
		/* Wait until write is done */
		if (wait_done(eepwr, I210_DONE))
			return I210_WRITE_ERROR;
	}
	/* Since we have modified data, we need to update the checksum */
	if (compute_checksum(dev, &checksum))
		return I210_CHECKSUM_ERROR;
	*eepwr = (0x3f << 2) | checksum << 16;
	if (wait_done(eepwr, I210_DONE))
		return I210_WRITE_ERROR;
	/* Up to now, desired data was written into shadowed RAM. We now need */
	/* to perform a flash cycle to bring the shadowed RAM into flash. */
	/* To start a flash cycle we need to set FLUPD and wait for FLDONE. */
	*eectrl = *eectrl | I210_FLUPD;
	if (wait_done(eectrl, I210_FLUDONE))
		return I210_FLASH_UPDATE_ERROR;
	return I210_SUCCESS;
}

/** \brief This function can read the MAC address out of the MACPHY
 * @param *dev    Pointer to the PCI device of this MACPHY
 * @param *MACAdr Pointer to the buffer where to store read MAC address
 * @return void   I210_NO_ERROR or an error code
 */
static uint32_t read_mac_adr(struct device *dev, uint8_t *mac_adr)
{
	uint16_t adr[3];
	if (!dev || !mac_adr)
		return I210_INVALID_PARAM;
	if (read_flash(dev, 0, 3, adr))
		return I210_READ_ERROR;
	/* Copy the address into destination. This is done because of possible */
	/* not matching alignment for destination to uint16_t boundary. */
	memcpy(mac_adr, (uint8_t *)adr, 6);
	return I210_SUCCESS;
}

/** \brief	This function can write the MAC address to the MACPHY
 * @param *dev    Pointer to the PCI device of this MACPHY
 * @param *MACAdr Pointer to the buffer where the desired MAC address is
 * @return void   I210_NO_ERROR or an error code
 */
static uint32_t write_mac_adr(struct device *dev, uint8_t *mac_adr)
{
	uint16_t adr[3];
	if (!dev || !mac_adr)
		return I210_INVALID_PARAM;
	/* Copy desired address into a local buffer to avoid alignment issues */
	memcpy((uint8_t *)adr, mac_adr, 6);
	return write_flash(dev, 0, 3, adr);
}

/** \brief This function is the driver entry point for the init phase
 *         of the PCI bus allocator. It will program a MAC address
 *         into the MACPHY.
 * @param  *dev  Pointer to the used PCI device
 * @return void  Nothing is given back
 */
static void init(struct device *dev)
{
	uint8_t cur_adr[6];
	uint8_t adr_to_set[6];
	enum cb_err status;

	/*Check first whether there is a valid MAC address available */
	status = mainboard_get_mac_address(dev, adr_to_set);
	if (status != CB_SUCCESS) {
		printk(BIOS_NOTICE, "I210: Mainboard has no address, keep the one in MAC.\n");
		return;
	}
	/* Before we will write a new address, check the existing one */
	if (read_mac_adr(dev, cur_adr)) {
		printk(BIOS_ERR, "I210: Not able to read MAC address.\n");
		return;
	}
	if (memcmp(cur_adr, adr_to_set, 6)) {
		if (write_mac_adr(dev, adr_to_set))
			printk(BIOS_ERR, "I210: Error setting MAC address\n");
		else
			printk(BIOS_INFO, "I210: MAC address changed.\n");
	} else {
		printk(BIOS_INFO, "I210: MAC address is up to date.\n");
	}
}

static void enable_bus_master(struct device *dev)
{
	if (CONFIG(PCI_ALLOW_BUS_MASTER_ANY_DEVICE))
		pci_or_config16(dev, PCI_COMMAND, PCI_COMMAND_MASTER);
}

static struct device_operations i210_ops  = {
	.read_resources   = pci_dev_read_resources,
	.set_resources    = pci_dev_set_resources,
	.enable_resources = pci_dev_enable_resources,
	.init             = init,
	.final            = enable_bus_master,
};

static const unsigned short i210_device_ids[] = { 0x1537, 0x1538, 0x1533, 0 };

static const struct pci_driver i210_driver __pci_driver = {
	.ops    = &i210_ops,
	.vendor = PCI_VID_INTEL,
	.devices = i210_device_ids,
};
