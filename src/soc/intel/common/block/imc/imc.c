/* SPDX-License-Identifier: GPL-2.0-only */

#include <commonlib/bsd/cb_err.h>
#include <console/console.h>
#include <delay.h>
#include <device/mmio.h>
#include <device/pci_ops.h>
#include <intelblocks/imc.h>
#include <soc/pci_devs.h>
#include <stdbool.h>
#include <timer.h>

#include "imclib.h"

#define IMC_SMBUS_TIMEOUT_MS 100

#define SMB_CMD_CFG           0x80
#define SMB_CKOVRD            BIT(29)
#define SMB_DIS_WRT           BIT(28)
#define SMB_SOFT_RST          BIT(24)
#define SMB_TSOD_POLL_EN      BIT(20)
#define SMB_CMD_TRIGGER       BIT(19)
#define SMB_WORD_ACCESS       BIT(17)
#define SMB_WRT_READ          (0 << 15)
#define SMB_WRT_WRITE         BIT(15)
#define SMB_DTI_MASK          (7 << 11)
#define SMB_CMD_SA_SHIFT      8
#define SMB_CMD_BA_SHIFT      0
#define SMB_CMD_DTI_SHIFT     11
#define SMB_STATUS_CFG        0x84
#define SMB_SBE               BIT(1)
#define SMB_BUSY              BIT(0)
#define SMB_DATA_CFG          0x88
#define SMB_PERIOD_CFG        0x90
#define SMB_CLOCK_PERIOD_400K 250 /* Clock period for 400K. */
#define SMB_CLOCK_OFFSET_400K 35  /* Clock offset for 400K. */

static void imc_spd_smbus_reset(pci_devfn_t dev)
{
	uint32_t cmd;

	cmd = pci_read_config32(dev, SMB_CMD_CFG);
	cmd &= ~SMB_CKOVRD;
	cmd |= SMB_SOFT_RST;
	pci_write_config32(dev, SMB_CMD_CFG, cmd);

	mdelay(35); /* See description of `SMB_CKOVRD` field. */

	cmd = pci_read_config32(dev, SMB_CMD_CFG);
	cmd |= SMB_CKOVRD;
	cmd &= ~SMB_SOFT_RST;
	pci_write_config32(dev, SMB_CMD_CFG, cmd);
}

void imc_spd_smbus_init(pci_devfn_t dev)
{
	uint32_t status, cmd;
	if (pci_read_config16(dev, 0) == 0xffff) {
		printk(BIOS_ERR,
		       "IMC SMBUS controller PCI: %02x:%02x:%02x.%01x isn't present!\n",
		       PCI_DEV2SEG(dev), PCI_DEV2BUS(dev), PCI_SLOT(PCI_DEV2DEVFN(dev)),
		       PCI_FUNC(PCI_DEV2DEVFN(dev)));
		return;
	}

	/* Set SMB CLOCK to 400K to detect DIMM SPDs. */
	pci_write_config32(dev, SMB_PERIOD_CFG,
			   (SMB_CLOCK_OFFSET_400K << 16) | SMB_CLOCK_PERIOD_400K);

	/* Reset the bus if the first access is busy. */
	status = pci_read_config32(dev, SMB_STATUS_CFG);
	if (status & SMB_BUSY)
		imc_spd_smbus_reset(dev);

	/* Disable TSOD polling. */
	cmd = pci_read_config32(dev, SMB_CMD_CFG);
	cmd &= ~SMB_TSOD_POLL_EN;
	pci_write_config32(dev, SMB_CMD_CFG, cmd);
}

static bool poll_ready(pci_devfn_t dev, uint32_t *status)
{
	struct stopwatch sw;

	stopwatch_init_msecs_expire(&sw, IMC_SMBUS_TIMEOUT_MS);

	do {
		*status = pci_read_config32(dev, SMB_STATUS_CFG);
		if ((*status & SMB_BUSY) == 0)
			return true;
	} while (!stopwatch_expired(&sw));

	return false;
}

static bool claim_controller(pci_devfn_t dev)
{
	uint32_t cmd, status;

	cmd = pci_read_config32(dev, SMB_CMD_CFG);
	cmd &= ~SMB_TSOD_POLL_EN;
	cmd &= ~SMB_DIS_WRT;
	pci_write_config32(dev, SMB_CMD_CFG, cmd);

	return poll_ready(dev, &status);
}

static bool release_controller(pci_devfn_t dev)
{
	uint32_t cmd, status;

	cmd = pci_read_config32(dev, SMB_CMD_CFG);
	cmd |= SMB_TSOD_POLL_EN;
	pci_write_config32(dev, SMB_CMD_CFG, cmd);

	return poll_ready(dev, &status);
}

int imc_smbus_spd_xfer(pci_devfn_t dev, uint8_t slave_addr, uint8_t bus_addr,
		       enum device_type_id dti, enum access_width width,
		       enum memory_controller_id mcid, enum smbus_command cmd, void *data)
{
	int ret = CB_ERR;
	uint32_t cmdbits, stat, databits, data_mask;
	uint16_t wdata = 0, rdata = 0;

	/* Slaves addresses are 3 bits length, and bus address is 8 bits length. */
	if (slave_addr > (1 << 7) - 1) {
		printk(BIOS_ERR, "Invalid SMBus slave 0x%02x\n", slave_addr);
		return CB_ERR;
	}

	if (!claim_controller(dev)) {
		printk(BIOS_ERR, "Claim controller failed!\n");
		return CB_ERR;
	}

	cmdbits = slave_addr << SMB_CMD_SA_SHIFT;
	cmdbits |= bus_addr << SMB_CMD_BA_SHIFT;

	if (cmd == IMC_WRITE) {
		databits = pci_read_config32(dev, SMB_DATA_CFG);
		wdata = (width == IMC_DATA_BYTE ? read8(data) : cpu_to_be16(read16(data)));
		databits |= (wdata << 16);
		pci_write_config32(dev, SMB_DATA_CFG, databits);

		cmdbits |= SMB_WRT_WRITE;
		cmdbits &= ~SMB_DIS_WRT;
	} else {
		cmdbits |= SMB_WRT_READ;
	}

	if (width == IMC_DATA_WORD) {
		cmdbits |= SMB_WORD_ACCESS;
		data_mask = 0xffff;
	} else {
		data_mask = 0xff;
	}

	cmdbits &= ~SMB_DTI_MASK;
	cmdbits |= dti << SMB_CMD_DTI_SHIFT;
	cmdbits |= SMB_CKOVRD;

	/* Pull the trigger */
	cmdbits |= SMB_CMD_TRIGGER;
	pci_write_config32(dev, SMB_CMD_CFG, cmdbits);

	if (!poll_ready(dev, &stat)) {
		printk(BIOS_ERR, "IMC transfer didn't finished for slave 0x%02x\n", slave_addr);
		ret = CB_ERR;
		goto cleanup;
	}

	if (stat & SMB_SBE) {
		printk(BIOS_ERR, "IMC SMBUS SBE for slave 0x%02x\n", slave_addr);
		ret = CB_ERR;
		goto cleanup;
	}

	if (cmd == IMC_READ) {
		databits = pci_read_config32(dev, SMB_DATA_CFG);
		rdata = databits & data_mask;
		if (width == IMC_DATA_WORD)
			write16(data, be16_to_cpu(rdata));
		else
			write8(data, rdata);
	}

	ret = CB_SUCCESS;

cleanup:
	if (!release_controller(dev)) {
		printk(BIOS_ERR, "Release controller failed!\n");
		return CB_ERR;
	}

	return ret;
}
