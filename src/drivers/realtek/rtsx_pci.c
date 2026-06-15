/* SPDX-License-Identifier: GPL-2.0-only */

/*
 * Driver for the Realtek RTS5227/RTS5229 PCIe SD card reader.
 *
 * coreboot does not run the init sequence the Lenovo vendor UEFI firmware
 * applies after PCIe enumeration, so on boards such as the ThinkPad T440p the
 * in-kernel Linux rtsx_pci driver times out on every SD command
 * ("rtsx_pci_send_cmd error -110"). This driver replicates that sequence
 * (reverse engineered from the vendor firmware) from the device's .init phase:
 *
 *   - PCI config-space vendor settings the rtsx_pci driver reads back but never
 *     writes (PCR_SETTING_REG1/REG2). The values depend on the chip revision
 *     (DUMMY_REG_RESET_0 bits 3:0): the vendor firmware uses one set for IC
 *     version > 2 and another for older parts. The T440p reader (version 4)
 *     needs the newer set; the older values break its card detect. Bit 14 of
 *     REG2 is the "reverse socket" flag the kernel later reads back
 *     (PCR_REVERSE_SOCKET in rtsx_pci.h).
 *   - internal registers via the HAIMR window (BAR0 + 0x10): clock/ASPM force
 *     controls, two PCIe PHY tunings and the card power path (PWD_SUSPEND_EN,
 *     PWR_GATE_CTRL), which the kernel driver assumes the firmware set up.
 *
 * Register names follow the Linux rtsx_pci driver.
 */

#include <console/console.h>
#include <device/device.h>
#include <device/mmio.h>
#include <device/pci.h>
#include <device/pci_def.h>
#include <device/pci_ids.h>
#include <device/pci_ops.h>
#include <types.h>

/*
 * HAIMR: window into the chip's internal register file (BAR0 + 0x10).
 * Bit 31 starts a transfer and is cleared by the chip when done (busy);
 * bit 30 selects the direction (1 = write).
 */
#define RTSX_HAIMR		0x10
#define  HAIMR_WRITE_START	0xc0000000	/* bit31 start | bit30 write */
#define  HAIMR_READ_START	0x80000000	/* bit31 start (read)        */
#define  HAIMR_BUSY		0x80000000	/* bit31: set while busy      */
#define  HAIMR_POLL_MAX		1024

/* Internal registers written by the vendor init sequence */
#define ASPM_FORCE_CTL		0xfe57
#define PM_CLK_FORCE_CTL	0xfe58
#define PHYRWCTL		0xfe3c
#define  PHY_RWCTL_WRITE	0x81		/* start | write */
#define  PHY_RWCTL_READ		0x80		/* start | read  */
#define PHYDATA0		0xfe3d
#define PHYDATA1		0xfe3e
#define PHYADDR			0xfe3f
#define PWR_GATE_CTRL		0xfe75
#define PWD_SUSPEND_EN		0xfe76	/* PWD = power down */
#define L1SUB_CONFIG1		0xfe8d
#define DUMMY_REG_RESET_0	0xfe90	/* bits 3:0 = IC version */
#define PETXCFG			0xff03	/* PCIe transmit config */

/* PCIe-PHY registers the Lenovo firmware tunes */
#define PHY_FLD4			0x1e
#define  PHY_FLD4_FLDEN_SEL		0x4000
#define  PHY_FLD4_REQ_REF		0x2000
#define  PHY_FLD4_RXAMP_OFF		0x1000
#define  PHY_FLD4_REQ_ADDA		0x0800
#define  PHY_FLD4_BER_COUNT		0x00e0
#define  PHY_FLD4_BER_TIMER		0x000a
#define  PHY_FLD4_BER_CHK_EN		0x0001
#define PHY_REV				0x19
#define  PHY_REV_RESV			0xe000
#define  PHY_REV_RXIDLE_LATCHED		0x1000
#define  PHY_REV_P1_EN			0x0800
#define  PHY_REV_RXIDLE_EN		0x0400
#define  PHY_REV_CLKREQ_TX_EN		0x0200
#define  PHY_REV_CLKREQ_DT_1_0		0x0040
#define  PHY_REV_STOP_CLKRD		0x0020
#define  PHY_REV_RX_PWST		0x0008
#define  PHY_REV_STOP_CLKWR		0x0004

static int rtsx_wait_haimr(uintptr_t bar)
{
	for (unsigned int i = 0; i < HAIMR_POLL_MAX; i++) {
		if (!(read32p(bar + RTSX_HAIMR) & HAIMR_BUSY))
			return 0;
	}
	return -1;
}

static int rtsx_write_masked(uintptr_t bar, u16 addr, u8 mask, u8 data)
{
	const u32 val = (addr & 0x3fff) << 16 | mask << 8 | data;
	write32p(bar + RTSX_HAIMR, HAIMR_WRITE_START | val);
	return rtsx_wait_haimr(bar);
}

static int rtsx_write(uintptr_t bar, u16 addr, u8 data)
{
	return rtsx_write_masked(bar, addr, 0xff, data);
}

static u8 rtsx_read(uintptr_t bar, u16 addr)
{
	write32p(bar + RTSX_HAIMR, HAIMR_READ_START | (addr & 0x3fff) << 16);
	if (rtsx_wait_haimr(bar))
		return 0;
	return read32p(bar + RTSX_HAIMR) & 0xff;
}

static void rtsx_wait_phy(uintptr_t bar)
{
	for (unsigned int i = 0; i < HAIMR_POLL_MAX; i++) {
		if (!(rtsx_read(bar, PHYRWCTL) & 0x80))
			return;
	}
}

static void rtsx_write_phy(uintptr_t bar, u8 addr, u16 val)
{
	rtsx_write(bar, PHYDATA0, val & 0xff);
	rtsx_write(bar, PHYDATA1, val >> 8);
	rtsx_write(bar, PHYADDR, addr);
	rtsx_write(bar, PHYRWCTL, PHY_RWCTL_WRITE);
	rtsx_wait_phy(bar);
}

static u16 rtsx_read_phy(uintptr_t bar, u8 addr)
{
	rtsx_write(bar, PHYADDR, addr);
	rtsx_write(bar, PHYRWCTL, PHY_RWCTL_READ);
	rtsx_wait_phy(bar);

	u16 val = 0;
	val |= rtsx_read(bar, PHYDATA0) << 0;
	val |= rtsx_read(bar, PHYDATA1) << 8;
	return val;
}

/* Clear Common Clock Configuration (LNKCTL bit 6), as the vendor firmware does. */
static void disable_pcie_ccc(struct device *dev)
{
	const u16 cap = pci_find_capability(dev, PCI_CAP_ID_PCIE);
	if (cap)
		pci_and_config8(dev, cap + PCI_EXP_LNKCTL, ~PCI_EXP_LNKCTL_CCC);
}

/*
 * The rtsx_pci driver reads these "vendor" settings, but does not write them.
 * The PCR_SETTING_REG* registers are read-only unless the PCR_UNLOCK bit is set
 * in the PCR_SETTING_LOCK register.
 *
 * It is not known whether these "vendor" settings are board-specific or generic
 * to the chip; they are applied as found in the vendor init sequence.
 */
#define PCR_SETTING_LOCK	0x817
#define  PCR_UNLOCK		BIT(7)
#define PCR_SETTING_REG1	0x724
#define PCR_SETTING_REG2	0x814

static void rts5227_vendor_settings(struct device *dev, bool new_revision)
{
	pci_write_config8(dev, PCR_SETTING_LOCK, PCR_UNLOCK);
	pci_write_config32(dev, PCR_SETTING_REG1, new_revision ? 0xbeff9fea : 0xaeff9fea);
	disable_pcie_ccc(dev);
	if (dev->upstream && dev->upstream->dev)
		disable_pcie_ccc(dev->upstream->dev);
	/*
	 * REG2 bit 14 is the reverse-socket flag the kernel reads back,
	 * cf. rtsx_reg_check_reverse_socket() in rtsx_pcr.h from Linux.
	 */
	const u16 reverse_socket = new_revision ? BIT(14) : 0;
	pci_write_config16(dev, PCR_SETTING_REG2, 0x60 | reverse_socket);
	pci_write_config8(dev, PCR_SETTING_LOCK, 0x00);
}

static void rts5227_init(struct device *dev)
{
	struct resource *res = probe_resource(dev, PCI_BASE_ADDRESS_0);
	if (!res) {
		printk(BIOS_WARNING, "RTS5227: BAR0 unassigned, skipping init\n");
		return;
	}
	const uintptr_t bar = (uintptr_t)res->base;

	/* The vendor firmware programs a different value set for IC version > 2. */
	const u8 ic_version = rtsx_read(bar, DUMMY_REG_RESET_0) & 0x0f;
	printk(BIOS_INFO, "RTS5227: IC version %u\n", ic_version);

	rts5227_vendor_settings(dev, ic_version > 2);

	/* Clock/ASPM force controls. */
	rtsx_write_masked(bar, ASPM_FORCE_CTL, 0x13, 0x10);
	rtsx_write_masked(bar, PETXCFG, 0x80, 0x80);
	rtsx_write(bar, L1SUB_CONFIG1, 0x00);
	rtsx_write(bar, PM_CLK_FORCE_CTL, 0x11);

	/* PCIe PHY tunings the kernel driver cannot apply. */
	rtsx_write_phy(bar, PHY_FLD4,
		PHY_FLD4_FLDEN_SEL | PHY_FLD4_REQ_REF |
		PHY_FLD4_RXAMP_OFF | PHY_FLD4_REQ_ADDA |
		PHY_FLD4_BER_COUNT | PHY_FLD4_BER_TIMER |
		PHY_FLD4_BER_CHK_EN);

	rtsx_write_phy(bar, PHY_REV,
		PHY_REV_RESV | PHY_REV_RXIDLE_LATCHED |
		PHY_REV_P1_EN | PHY_REV_RXIDLE_EN |
		PHY_REV_CLKREQ_TX_EN | PHY_REV_RX_PWST |
		PHY_REV_CLKREQ_DT_1_0 | PHY_REV_STOP_CLKRD |
		PHY_REV_STOP_CLKWR);

	/* Decisive for the dead-card symptom: power the card path. */
	rtsx_write(bar, PWD_SUSPEND_EN, 0xff);
	rtsx_write(bar, PWR_GATE_CTRL, 0xf1);

	printk(BIOS_DEBUG, "RTS5227: PHY_FLD4[0x%02x] = 0x%04x\n",
	       PHY_FLD4, rtsx_read_phy(bar, PHY_FLD4));
}

static struct device_operations rts5227_ops = {
	.read_resources		= pci_dev_read_resources,
	.set_resources		= pci_dev_set_resources,
	.enable_resources	= pci_dev_enable_resources,
	.ops_pci		= &pci_dev_ops_pci,
	.init			= rts5227_init,
};

static const unsigned short pci_device_ids[] = {
	0x5227, /* RTS5227 PCI Express Card Reader */
	0x5229, /* RTS5229 PCI Express Card Reader (same family) */
	0
};

static const struct pci_driver realtek_cardreader __pci_driver = {
	.ops		= &rts5227_ops,
	.vendor		= PCI_VID_REALTEK,
	.devices	= pci_device_ids,
};
