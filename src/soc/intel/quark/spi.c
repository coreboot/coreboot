/* SPDX-License-Identifier: GPL-2.0-only */

#include <device/mmio.h>
#include <device/pci_ops.h>
#include <bootstate.h>
#include <console/console.h>
#include <device/pci_ids.h>
#include <device/pci_def.h>
#include <delay.h>
#include <soc/pci_devs.h>
#include <soc/QuarkNcSocId.h>
#include <soc/spi.h>
#include <string.h>

struct spi_context spi_driver_context = {
	NULL,
	0,
	0
};

void spi_bios_base(uint32_t bios_base_address)
{
	uint32_t address;
	volatile struct flash_ctrlr *ctrlr = spi_driver_context.ctrlr;

	/* Prevent all SPI operations below this address */
	address = 0xff000000 | bios_base_address;
	ctrlr->bbar = address;
}

void spi_controller_lock(void)
{
	volatile struct flash_ctrlr *ctrlr = spi_driver_context.ctrlr;

	/* Prevent BIOS and system from changing the SPI controller setup */
	ctrlr->status |= SPISTS_CLD;
}

int spi_protection(uint32_t address, uint32_t length)
{
	uint32_t base;
	volatile struct flash_ctrlr *ctrlr = spi_driver_context.ctrlr;
	int index;
	uint32_t limit;
	uint32_t protect;
	uint32_t value;

	/* Determine the protection range */
	base = address;
	limit = address + length - 1;
	protect = SPIPBR_WPE | (limit & SPIPBR_PRL)
		| ((base >> SPIPBR_PRB_SHIFT) & SPIPBR_PRB);

	/* Walk the list of protected areas */
	for (index = 0; index < ARRAY_SIZE(ctrlr->pbr); index++) {
		value = read32(&ctrlr->pbr[index]);

		/* Don't duplicate if the range is already protected */
		if (value == protect)
			return 0;

		/* Use the first free register to protect this range */
		if ((value & SPIPBR_WPE) == 0) {
			write32(&ctrlr->pbr[index], protect);
			return 0;
		}
	}

	/* No free protection range registers */
	printk(BIOS_ERR,
		"Failed to set protection: 0x%08x - 0x%08x, PRRs full!\n",
				address, address + length - 1);
	return -1;
}

static int xfer(const struct spi_slave *slave, const void *dout,
	size_t bytesout, void *din, size_t bytesin)
{
	struct spi_context *context;
	uint16_t control;
	volatile struct flash_ctrlr *ctrlr;
	uint8_t *data;
	int index;
	uint8_t opcode;
	uint32_t status;
	int type;

	/* Locate the context structure */
	context = &spi_driver_context;
	ctrlr = context->ctrlr;

	/* Validate the buffer sizes */
	if (bytesin > sizeof(ctrlr->data)) {
		printk(BIOS_ERR, "bytesin > %zu\n", sizeof(ctrlr->data));
		goto error;
	}

	if (bytesin && (din == NULL)) {
		printk(BIOS_ERR, "din is NULL\n");
		goto error;
	}

	if (bytesout == 0) {
		/* Check for a read operation */
		if (bytesin == 0) {
			printk(BIOS_ERR, "bytesout and bytesin == 0\n");
			goto error;
		}

		/* Issue the read operation */
		control = context->control;
		control |= SPICTL_DC | ((bytesin - 1) << SPICTL_DBCNT_SHIFT);
		goto start_cycle;
	}

	/* Locate the opcode in the opcode table */
	data = (uint8_t *)dout;
	opcode = *data++;
	bytesout -= 1;
	for (index = 0; index < sizeof(ctrlr->opmenu); index++)
		if (opcode == ctrlr->opmenu[index])
			break;

	/* Check for a prefix byte */
	if (index == sizeof(ctrlr->opmenu)) {
		for (index = 0; index < sizeof(ctrlr->prefix); index++)
			if (opcode == ctrlr->prefix[index])
				break;

		/* Handle the unknown opcode error */
		if (index == sizeof(ctrlr->prefix)) {
			printk(BIOS_ERR, "Unknown SPI flash opcode\n");
			goto error;
		}

		/* Save the index for the next operation */
		context->prefix = index;
		return 0;
	}

	/* Get the opcode type */
	type = (ctrlr->type >> (index * 2))
		& (SPITYPE_ADDRESS | SPITYPE_PREFIX);

	/* Determine if the opcode has an address */
	if (type & SPITYPE_ADDRESS) {
		if (bytesout < 3) {
			printk(BIOS_ERR, "Missing address bytes\n");
			goto error;
		}

		/* Use chip select 0 */
		ctrlr->address = (data[0] << 16)
			       | (data[1] << 8)
			       |  data[2];

		/* read in order to flush the write buffer */
		status = ctrlr->address;

		data += 3;
		bytesout -= 3;
	}

	/* Build the control value */
	control = (index << SPICTL_COPTR_SHIFT)
		| (context->prefix << SPICTL_SOPTR_SHIFT)
		| SPICTL_CG | SPICTL_AR;
	if (bytesout) {
		memcpy((void *)&ctrlr->data[0], data, bytesout);
		control |= SPICTL_DC | ((bytesout - 1) << SPICTL_DBCNT_SHIFT);
	}

	/* Save the control value for the read operation request */
	if (!(type & SPITYPE_PREFIX)) {
		context->control = control;
		return 0;
	}

	/* Write operations require a prefix */
	control |= SPICTL_ACS;

start_cycle:
	/* Start the SPI cycle */
	ctrlr->control = control;
	status = ctrlr->control;
	context->prefix = 0;

	/* Wait for the access to complete */
	while ((status = ctrlr->status) & SPISTS_CIP)
		udelay(1);

	/* Clear any errors */
	ctrlr->status = status;

	/* Handle the blocked access error */
	if (status & SPISTS_BA) {
		printk(BIOS_ERR, "SPI access blocked!\n");
		return -1;
	}

	/* Check for done */
	if (status & SPISTS_CD) {
		/* Return any receive data */
		if (bytesin)
			memcpy(din, (void *)&ctrlr->data[0], bytesin);
		return 0;
	}

	/* Handle the timeout error */
	printk(BIOS_ERR, "SPI transaction timeout!\n");

error:
	context->prefix = 0;
	return -1;
}

void spi_init(void)
{
	uint32_t bios_control;
	struct spi_context *context;
	volatile struct flash_ctrlr *ctrlr;
	struct device *dev;
	uint32_t rcba;

	/* Determine the base address of the SPI flash controller */
	context = &spi_driver_context;
	dev = dev_find_device(PCI_VID_INTEL, LPC_DEVID, NULL);
	rcba = pci_read_config32(dev, R_QNC_LPC_RCBA);
	if (!(rcba & B_QNC_LPC_RCBA_EN)) {
		printk(BIOS_ERR, "RBCA not enabled\n");
		return;
	}
	rcba &= B_QNC_LPC_RCBA_MASK;
	ctrlr = (volatile struct flash_ctrlr *)rcba;

	/* Enable writes to the SPI flash */
	bios_control = pci_read_config32(dev, R_QNC_LPC_BIOS_CNTL);
	bios_control |= B_QNC_LPC_BIOS_CNTL_BIOSWE;
	pci_write_config32(dev, R_QNC_LPC_BIOS_CNTL, bios_control);

	/* Setup the SPI flash controller */
	context->ctrlr = ctrlr;
	ctrlr->opmenu[0] = 0x03;	/* Read */
	ctrlr->opmenu[1] = 0x0b;	/* Read fast */
	ctrlr->opmenu[2] = 0x05;	/* Read status */
	ctrlr->opmenu[3] = 0x9f;	/* Read ID */
	ctrlr->opmenu[4] = 0x02;	/* Page program */
	ctrlr->opmenu[5] = 0x20;	/* Erase 4 KiB */
	ctrlr->opmenu[6] = 0xd8;	/* Erase 64 KiB */
	ctrlr->opmenu[7] = 0x01;	/* Write status */
	ctrlr->prefix[0] = 0x50;	/* Write status enable */
	ctrlr->prefix[1] = 0x06;	/* Write enable */
	ctrlr->type = SPITYPE_ADDRESS                        /* Read */
		| (SPITYPE_ADDRESS << 2)                     /* Read fast */
		| (0 << 4)                                   /* Read status */
		| (0 << 6)                                   /* Read ID */
		| ((SPITYPE_ADDRESS | SPITYPE_PREFIX) << 8)  /* Page program */
		| ((SPITYPE_ADDRESS | SPITYPE_PREFIX) << 10) /* Erase 4 KiB */
		| ((SPITYPE_ADDRESS | SPITYPE_PREFIX) << 12) /* Erase 64 KiB */
		| (SPITYPE_PREFIX << 14);                    /* Write status */
}

static void spi_init_cb(void *unused)
{
	struct spi_flash flash;

	spi_init();
	if (spi_flash_probe(0, 0, &flash)) {
		printk(BIOS_DEBUG, "SPI flash failed initialization!\n");
		return;
	}
	printk(BIOS_DEBUG, "SPI flash successfully initialized\n");
}

BOOT_STATE_INIT_ENTRY(BS_DEV_INIT, BS_ON_ENTRY, spi_init_cb, NULL);

const struct spi_ctrlr spi_driver = {
	.xfer = xfer,
	.max_xfer_size = 64,
};

const struct spi_ctrlr_buses spi_ctrlr_bus_map[] = {
	{
		.ctrlr = &spi_driver,
		.bus_start = 0,
		.bus_end = 0,
	},
};

const size_t spi_ctrlr_bus_map_count = ARRAY_SIZE(spi_ctrlr_bus_map);
