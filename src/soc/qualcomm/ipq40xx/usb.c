/* SPDX-License-Identifier: GPL-2.0-only */

#include <device/mmio.h>
#include <console/console.h>
#include <delay.h>
#include <soc/clock.h>
#include <soc/iomap.h>
#include <soc/usb.h>

/**
 *  USB Hardware registers
 */
#define PHY_CTRL0_ADDR	0x000
#define PHY_CTRL1_ADDR	0x004
#define PHY_CTRL2_ADDR	0x008
#define PHY_CTRL3_ADDR	0x00C
#define PHY_CTRL4_ADDR	0x010
#define PHY_MISC_ADDR	0x024
#define PHY_IPG_ADDR	0x030

#define PHY_CTRL0_VAL	0xA4600015
#define PHY_CTRL1_VAL	0x09500000
#define PHY_CTRL2_VAL	0x00058180
#define PHY_CTRL3_VAL	0x6DB6DCD6
#define PHY_CTRL4_VAL	0x836DB6DB
#define PHY_MISC_VAL	0x3803FB0C
#define PHY_IPG_VAL	0x47323232

#define	USB_HOST3_PHY_BASE		((void *)0x8a00000)
#define	USB_HOST3_BALDUR_PHY_BASE	((void *)0xa6000)
#define	GCC_USB3_RST_CTRL		((void *)0x0181E038)

#define DWC3_GCTL			0xc110
#define DWC3_GUSB3PIPECTL(n)		(0xc2c0 + (n * 0x04))
#define DWC3_GUSB2PHYCFG(n)		(0xc200 + (n * 0x04))

/* Global USB3 PIPE Control Register */
#define DWC3_GUSB3PIPECTL_PHYSOFTRST	(1 << 31)
#define DWC3_GUSB3PIPECTL_SUSPHY	(1 << 17)
#define DWC3_GCTL_CORESOFTRESET		(1 << 11)
#define DWC3_GCTL_PRTCAPDIR(n)		((n) << 12)
#define DWC3_GCTL_PRTCAP_OTG		3
#define DWC3_DCTL_CSFTRST		(1 << 30)
#define DWC3_GSNPSID			0xc120
#define DWC3_DCTL			0xc704

/* Global USB2 PHY Configuration Register */
#define DWC3_GUSB2PHYCFG_PHYSOFTRST	(1 << 31)
#define DWC3_GUSB2PHYCFG_SUSPHY		(1 << 6)
#define DWC3_GSNPSID_MASK		0xffff0000
#define DWC3_GEVTEN			0xc114

#define DWC3_GCTL_SCALEDOWN(n)		((n) << 4)
#define DWC3_GCTL_SCALEDOWN_MASK	DWC3_GCTL_SCALEDOWN(3)
#define DWC3_GCTL_DISSCRAMBLE		(1 << 3)
#define DWC3_GCTL_DSBLCLKGTNG		(1 << 0)
#define DWC3_GCTL_U2RSTECN		(1 << 16)
#define DWC3_REVISION_190A		0x5533190a

#define USB30_HS_PHY_CTRL		0x00000010
#define SW_SESSVLD			(0x01 << 0x1C)
#define UTMI_OTG_VBUS_VALID		(0x01 << 0x14)

#define USB30_SS_PHY_CTRL		0x00000030
#define LANE0_PWR_PRESENT		(0x01 << 0x18)

static void setup_dwc3(void);

/**
 * Write register.
 *
 * @param base - PHY base virtual address.
 * @param offset - register offset.
 * @param val - value to write.
 */
static inline void qscratch_write(void *base, u32 offset, u32 val)
{
	write32(base + offset, val);
}

/**
 * Write register and read back masked value to confirm it is written
 *
 * @param base - base virtual address.
 * @param offset - register offset.
 * @param mask - register bitmask specifying what should be updated
 * @param val - value to write.
 */
static inline void qscratch_write_readback(void *base, u32 offset,
						const u32 mask, u32 val)
{
	u32 write_val, tmp = read32(base + offset);

	tmp &= ~mask;       /* retain other bits */
	write_val = tmp | val;

	write32(base + offset, write_val);

	/* Read back to see if val was written */
	tmp = read32(base + offset);
	tmp &= mask;        /* clear other bits */

	if (tmp != val) {
		printk(BIOS_INFO, "write: %x to QSCRATCH: %x FAILED\n",
			val, offset);
	}
}

static void dwc3_ipq40xx_enable_vbus_valid(void)
{
	/* Enable VBUS valid for HS PHY*/
	qscratch_write_readback((void *)0x8af8800, USB30_HS_PHY_CTRL,
				SW_SESSVLD, SW_SESSVLD);
	qscratch_write_readback((void *)0x8af8800, USB30_HS_PHY_CTRL,
				UTMI_OTG_VBUS_VALID, UTMI_OTG_VBUS_VALID);

	/* Enable VBUS valid for SS PHY*/
	qscratch_write_readback((void *)0x8af8800, USB30_SS_PHY_CTRL,
				LANE0_PWR_PRESENT, LANE0_PWR_PRESENT);
}

static void qcom_baldur_hs_phy_init(void)
{
	u32 reg;

	/* assert HS PHY POR reset */
	reg = read32(GCC_USB3_RST_CTRL);
	reg = reg | 0x10;
	write32(GCC_USB3_RST_CTRL, reg);
	mdelay(10);

	/* assert HS PHY SRIF reset */
	reg = read32(GCC_USB3_RST_CTRL);
	reg = reg | 0x4;
	write32(GCC_USB3_RST_CTRL, reg);
	mdelay(10);

	/* deassert HS PHY SRIF reset and program HS PHY registers */
	reg = read32(GCC_USB3_RST_CTRL);
	reg = reg & ~0x4;
	write32(GCC_USB3_RST_CTRL, reg);

	mdelay(10);

	/* perform PHY register writes */
	write32(USB_HOST3_BALDUR_PHY_BASE + PHY_CTRL0_ADDR, PHY_CTRL0_VAL);
	write32(USB_HOST3_BALDUR_PHY_BASE + PHY_CTRL1_ADDR, PHY_CTRL1_VAL);
	write32(USB_HOST3_BALDUR_PHY_BASE + PHY_CTRL2_ADDR, PHY_CTRL2_VAL);
	write32(USB_HOST3_BALDUR_PHY_BASE + PHY_CTRL3_ADDR, PHY_CTRL3_VAL);
	write32(USB_HOST3_BALDUR_PHY_BASE + PHY_CTRL4_ADDR, PHY_CTRL4_VAL);
	write32(USB_HOST3_BALDUR_PHY_BASE + PHY_MISC_ADDR, PHY_MISC_VAL);
	write32(USB_HOST3_BALDUR_PHY_BASE + PHY_IPG_ADDR, PHY_IPG_VAL);

	mdelay(10);

	/* de-assert USB3 HS PHY POR reset */
	reg = read32(GCC_USB3_RST_CTRL);
	reg = reg & ~0x10;
	write32(GCC_USB3_RST_CTRL, reg);
}

static void qcom_uni_ss_phy_init(void)
{
	u32 reg;

	/* assert SS PHY POR reset */
	reg = read32(GCC_USB3_RST_CTRL);
	reg = reg | 0x20;
	write32(GCC_USB3_RST_CTRL, reg);

	mdelay(100);

	/* deassert SS PHY POR reset */
	reg = read32(GCC_USB3_RST_CTRL);
	reg = reg  & ~0x20;
	write32(GCC_USB3_RST_CTRL, reg);
}

void setup_dwc3(void)
{
	u32 reg;
	u32 revision;

	revision = read32(USB_HOST3_PHY_BASE + DWC3_GSNPSID);
	/* This should read as U3 followed by revision number */
	if ((revision & DWC3_GSNPSID_MASK) != 0x55330000)
		printk(BIOS_INFO, "Error in reading Version\n");

	printk(BIOS_INFO, "Version = %x\n", revision);

	/* issue device SoftReset too */
	write32(USB_HOST3_PHY_BASE + DWC3_DCTL, DWC3_DCTL_CSFTRST);
	do {
		reg = read32(USB_HOST3_PHY_BASE + DWC3_DCTL);
		if (!(reg & DWC3_DCTL_CSFTRST))
			break;

		udelay(10);
	} while (true);
	printk(BIOS_INFO, "software reset done\n");

	/* Before Resetting PHY, put Core in Reset */
	reg = read32(USB_HOST3_PHY_BASE + DWC3_GCTL);
	reg |= DWC3_GCTL_CORESOFTRESET;
	write32(USB_HOST3_PHY_BASE + DWC3_GCTL, reg);

	/* Assert USB3 PHY reset */
	reg = read32(USB_HOST3_PHY_BASE + DWC3_GUSB3PIPECTL(0));
	reg |= DWC3_GUSB3PIPECTL_PHYSOFTRST;
	write32(USB_HOST3_PHY_BASE + DWC3_GUSB3PIPECTL(0), reg);

	/* Assert USB2 PHY reset */
	reg = read32(USB_HOST3_PHY_BASE + DWC3_GUSB2PHYCFG(0));
	reg |= DWC3_GUSB2PHYCFG_PHYSOFTRST;
	write32(USB_HOST3_PHY_BASE + DWC3_GUSB2PHYCFG(0), reg);

	qcom_baldur_hs_phy_init();
	qcom_uni_ss_phy_init();
	mdelay(100);

	/* Clear USB3 PHY reset */
	reg = read32(USB_HOST3_PHY_BASE + DWC3_GUSB3PIPECTL(0));
	reg &= ~DWC3_GUSB3PIPECTL_PHYSOFTRST;
	write32(USB_HOST3_PHY_BASE + DWC3_GUSB3PIPECTL(0), reg);

	/* Clear USB2 PHY reset */
	reg = read32(USB_HOST3_PHY_BASE + DWC3_GUSB2PHYCFG(0));
	reg &= ~DWC3_GUSB2PHYCFG_PHYSOFTRST;
	write32(USB_HOST3_PHY_BASE + DWC3_GUSB2PHYCFG(0), reg);

	mdelay(100);

	/* After PHYs are stable we can take Core out of reset state */
	reg = read32(USB_HOST3_PHY_BASE + DWC3_GCTL);
	reg &= ~DWC3_GCTL_CORESOFTRESET;
	write32(USB_HOST3_PHY_BASE + DWC3_GCTL, reg);

#if 0
	/* Enable Suspend USB2.0 HS/FS/LS PHY (SusPHY) */
	reg = read32(USB_HOST3_PHY_BASE + DWC3_GUSB2PHYCFG(0));
	reg |= DWC3_GUSB2PHYCFG_SUSPHY;
	write32(USB_HOST3_PHY_BASE + DWC3_GUSB2PHYCFG(0), reg);

	/* Enable Suspend USB3.0 SS PHY (Suspend_en) */
	reg = read32(USB_HOST3_PHY_BASE + DWC3_GUSB3PIPECTL(0));
	reg |= DWC3_GUSB3PIPECTL_SUSPHY;
	write32(USB_HOST3_PHY_BASE + DWC3_GUSB3PIPECTL(0), reg);
#endif

	/* configure controller in Host mode */
	reg = read32(USB_HOST3_PHY_BASE + DWC3_GCTL);
	reg &= ~(DWC3_GCTL_PRTCAPDIR(DWC3_GCTL_PRTCAP_OTG));
	reg |= DWC3_GCTL_PRTCAPDIR(0x1); /* host mode */
	write32(USB_HOST3_PHY_BASE + DWC3_GCTL, reg);
	printk(BIOS_INFO, "USB Host mode reg = %x\n", reg);

	reg = read32(USB_HOST3_PHY_BASE + DWC3_GCTL);
	reg &= ~DWC3_GCTL_SCALEDOWN_MASK;
	reg &= ~DWC3_GCTL_DISSCRAMBLE;

	reg &= ~DWC3_GCTL_DSBLCLKGTNG;
	/*
	 * WORKAROUND: DWC3 revisions <1.90a have a bug
	 * where the device can fail to connect at SuperSpeed
	 * and falls back to high-speed mode which causes
	 * the device to enter a Connect/Disconnect loop
	 */
	if (revision < DWC3_REVISION_190A)
		reg |= DWC3_GCTL_U2RSTECN;

	write32(USB_HOST3_PHY_BASE + DWC3_GCTL, reg);
}

void setup_usb_host1(void)
{
	printk(BIOS_INFO, "Setting up USB HOST1 controller.\n");
	setup_dwc3();
	dwc3_ipq40xx_enable_vbus_valid();
}
