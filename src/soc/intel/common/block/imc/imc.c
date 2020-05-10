/* SPDX-License-Identifier: GPL-2.0-only */

/*
 * Please note: the driver uses MMIO PCIe register access. IO based access will
 * not work.
 */

#include <console/console.h>
#include <delay.h>
#include <device/pci_ops.h>
#include <intelblocks/imc.h>
#include <soc/pci_devs.h>
#include <timer.h>

#define IMC_SMBUS_TIMEOUT_MS           100
#define IMC_SMBCNTL_DTI_TSOD           0x3
#define IMC_SMBCNTL_DTI_EEPROM         0xa
#define IMC_SMBCNTL_DTI_WP_EEPROM      0x6

#define SMBSTAT(i)                     (0x180 + 0x10 * i)
#define SMBCMD(i)                      (0x184 + 0x10 * i)
#define SMBCNTL(i)                     (0x188 + 0x10 * i)

#define SMBSTAT_RDO                    (1u << 31)  /* Read Data Valid */
#define SMBSTAT_WOD                    (1u << 30)  /* Write Operation Done */
#define SMBSTAT_SBE                    (1u << 29)  /* SMBus Error */
#define SMBSTAT_SMB_BUSY               (1u << 28)  /* SMBus Busy State */

#define SMBCMD_TRIGGER                 (1u << 31)  /* CMD Trigger */
#define SMBCMD_PNTR_SEL                (1u << 30)  /* HW polls TSOD with pointer */
#define SMBCMD_WORD_ACCESS             (1u << 29)  /* word (vs byte) access */
#define SMBCMD_TYPE_MASK               (3u << 27)  /* Mask for access type */
#define SMBCMD_TYPE_READ               (0u << 27)  /* Read */
#define SMBCMD_TYPE_WRITE              (1u << 27)  /* Write */
#define SMBCMD_TYPE_PNTR_WRITE         (3u << 27)  /* Write to pointer */
#define SMBCMD_SA_MASK                 (7u << 24)  /* Slave Address high bits */
#define SMBCMD_SA_SHIFT                24
#define SMBCMD_BA_MASK                 0xff0000    /* Bus Txn address */
#define SMBCMD_BA_SHIFT                16
#define SMBCMD_WDATA_MASK              0xffff      /* data to write */

#define SMBCNTL_DTI_MASK               0xf0000000  /* Slave Address low bits */
#define SMBCNTL_DTI_SHIFT              28          /* Slave Address low bits */
#define SMBCNTL_CKOVRD                 (1u << 27)  /* # Clock Override */
#define SMBCNTL_DIS_WRT                (1u << 26)  /* Disable Write (sadly) */
#define SMBCNTL_SOFT_RST               (1u << 10)  /* Soft Reset */
#define SMBCNTL_TSOD_POLL_EN           (1u << 8)   /* TSOD Polling Enable */

static bool poll_ready(pci_devfn_t dev, enum memory_controller_id mcid, uint32_t *status)
{
	struct stopwatch sw;

	stopwatch_init_msecs_expire(&sw, IMC_SMBUS_TIMEOUT_MS);

	do {
		*status = pci_mmio_read_config32(dev, SMBSTAT(mcid));
		if (!(*status & SMBSTAT_SMB_BUSY))
			break;
	} while (!stopwatch_expired(&sw));

	return (!(*status & SMBSTAT_SMB_BUSY));
}

static bool claim_controller(pci_devfn_t dev, enum memory_controller_id mcid)
{
	uint32_t cntl, status;

	cntl = pci_mmio_read_config32(dev, SMBCNTL(mcid));
	cntl &= ~SMBCNTL_TSOD_POLL_EN;
	cntl &= ~SMBCNTL_DIS_WRT;
	pci_mmio_write_config32(dev, SMBCNTL(mcid), cntl);

	return poll_ready(dev, mcid, &status);
}


static void release_controller(pci_devfn_t dev, enum memory_controller_id mcid)
{
	uint32_t cntl, status;

	cntl = pci_mmio_read_config32(dev, SMBCNTL(mcid));
	cntl |= SMBCNTL_TSOD_POLL_EN;
	pci_mmio_write_config32(dev, SMBCNTL(mcid), cntl);

	poll_ready(dev, mcid, &status);
}

int imc_smbus_spd_xfer(pci_devfn_t dev, uint8_t slave_addr, uint8_t bus_addr,
		       enum device_type_id dti, enum access_width width,
		       enum memory_controller_id mcid, enum smbus_command cmd, void *data)
{
	int ret = -1;
	uint32_t cmdbits = 0, stat = 0, cntlbits = 0, data_mask = 0;
	uint16_t wdata = 0, rdata = 0;

	/* slaves addresses are 7 bits length */
	if (slave_addr > (1 << 7) - 1) {
		printk(BIOS_ERR, "invalid SMBus address, aborting xfer\n");
		return -1;
	}

	if (!claim_controller(dev, mcid)) {
		printk(BIOS_ERR, "ayee! couldn't claim controller, giving up xfer\n");
		return -1;
	}

	cmdbits = (slave_addr << SMBCMD_SA_SHIFT);
	cmdbits |= (bus_addr << SMBCMD_BA_SHIFT);

	if (cmd == IMC_WRITE) {
		wdata = (width == IMC_DATA_BYTE ? read8(data) : cpu_to_be16(read16(data)));
		cmdbits |= (SMBCMD_TYPE_WRITE | wdata);
	} else {
		cmdbits |= SMBCMD_TYPE_READ;
	}

	if (width == IMC_DATA_WORD) {
		cmdbits |= SMBCMD_WORD_ACCESS;
		data_mask = 0xffff;
	} else {
		data_mask = 0xff;
	}

	cntlbits = pci_mmio_read_config32(dev, SMBCNTL(mcid));
	cntlbits &= ~SMBCNTL_DTI_MASK;
	cntlbits |= (dti << SMBCNTL_DTI_SHIFT);

	pci_mmio_write_config32(dev, SMBCNTL(mcid), cntlbits);

	/* Pull the trigger */
	cmdbits |= SMBCMD_TRIGGER;
	pci_mmio_write_config32(dev, SMBCMD(mcid), cmdbits);

	if (!poll_ready(dev, mcid, &stat)) {
		printk(BIOS_ERR, "IMC xfer failed for slave %x", slave_addr);
		ret = -1;
		goto cleanup;
	}

	if (stat & SMBSTAT_SBE) {
		ret = -1;
		goto cleanup;
	}

	if (cmd == IMC_READ) {
		rdata = stat & data_mask;
		if (width == IMC_DATA_WORD)
			write16(data, cpu_to_be16(rdata));
		else
			write8(data, rdata);
	}

	ret = 0;
cleanup:
	release_controller(dev, SMBSTAT(mcid));

	return ret;
}
