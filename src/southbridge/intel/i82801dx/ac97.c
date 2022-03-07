/* SPDX-License-Identifier: GPL-2.0-only */

#include <console/console.h>
#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <arch/io.h>
#include <device/pci_ops.h>
#include <delay.h>
#include "i82801dx.h"

#define NAMBAR		0x10
#define   MASTER_VOL	0x02
#define   PAGING	0x24
#define   EXT_AUDIO	0x28
#define   FUNC_SEL	0x66
#define   INFO_IO	0x68
#define   CONNECTOR	0x6a
#define   VENDOR_ID1	0x7c
#define   VENDOR_ID2	0x7e
#define   SEC_VENDOR_ID1 0xfc
#define   SEC_VENDOR_ID2 0xfe

#define NABMBAR		0x14
#define   GLOB_CNT	0x2c
#define   GLOB_STA	0x30
#define   CAS		0x34

#define MMBAR		0x10
#define   EXT_MODEM_ID1	0x3c
#define   EXT_MODEM_ID2	0xbc

#define MBAR		0x14
#define   SEC_CODEC	0x40

/* FIXME. This table is probably mainboard specific */
static u16 ac97_function[16*2][4] = {
	{ (1 << 5), (2 << 11), (1 << 10), (3 << 13) },
	{ (1 << 5), (2 << 11), (1 << 10), (3 << 13) },
	{ (1 << 5), (2 << 11), (1 << 10), (3 << 13) },
	{ (1 << 5), (2 << 11), (1 << 10), (3 << 13) },
	{ (1 << 5), (2 << 11), (1 << 10), (3 << 13) },
	{ (1 << 5), (2 << 11), (1 << 10), (3 << 13) },
	{ (1 << 5), (2 << 11), (1 << 10), (3 << 13) },
	{ (1 << 5), (2 << 11), (1 << 10), (3 << 13) },
	{ (1 << 5), (2 << 11), (1 << 10), (3 << 13) },
	{ (1 << 5), (2 << 11), (1 << 10), (3 << 13) },
	{ (1 << 5), (2 << 11), (1 << 10), (3 << 13) },
	{ (1 << 5), (2 << 11), (1 << 10), (3 << 13) },
	{ (1 << 5), (2 << 11), (1 << 10), (3 << 13) },
	{ (1 << 5), (2 << 11), (1 << 10), (3 << 13) },
	{ (1 << 5), (2 << 11), (1 << 10), (3 << 13) },
	{ (1 << 5), (2 << 11), (1 << 10), (3 << 13) },
	{ (1 << 5), (2 << 11), (1 << 10), (3 << 13) },
	{ (1 << 5), (2 << 11), (1 << 10), (3 << 13) },
	{ (1 << 5), (2 << 11), (1 << 10), (3 << 13) },
	{ (1 << 5), (2 << 11), (1 << 10), (3 << 13) },
	{ (1 << 5), (2 << 11), (1 << 10), (3 << 13) },
	{ (1 << 5), (2 << 11), (1 << 10), (3 << 13) },
	{ (1 << 5), (2 << 11), (1 << 10), (3 << 13) },
	{ (1 << 5), (2 << 11), (1 << 10), (3 << 13) },
	{ (1 << 5), (2 << 11), (1 << 10), (3 << 13) },
	{ (1 << 5), (2 << 11), (1 << 10), (3 << 13) },
	{ (1 << 5), (2 << 11), (1 << 10), (3 << 13) },
	{ (1 << 5), (2 << 11), (1 << 10), (3 << 13) },
	{ (1 << 5), (2 << 11), (1 << 10), (3 << 13) },
	{ (1 << 5), (2 << 11), (1 << 10), (3 << 13) },
	{ (1 << 5), (2 << 11), (1 << 10), (3 << 13) },
	{ (1 << 5), (2 << 11), (1 << 10), (3 << 13) }
};

static u16 nabmbar;
static u16 nambar;

static int ac97_semaphore(void)
{
	int timeout;
	u8 reg8;

	timeout = 0xffff;
	do {
		reg8 = inb(nabmbar + CAS);
		timeout--;
	} while ((reg8 & 1) && timeout);
	if (! timeout) {
		printk(BIOS_DEBUG, "Timeout!\n");
	}

	return (!timeout);
}

static void init_cnr(void)
{
	// TODO
}

static void program_sigid(struct device *dev, u32 id)
{
	pci_write_config32(dev, 0x2c, id);
}

static void ac97_audio_init(struct device *dev)
{
	u16 reg16;
	u32 reg32;
	int i;

	printk(BIOS_DEBUG, "Initializing AC'97 Audio.\n");

	/* top 16 bits are zero, so don't read them */
	nabmbar = pci_read_config16(dev, NABMBAR) & 0xfffe;
	nambar = pci_read_config16(dev, NAMBAR) & 0xfffe;

	reg16 = inw(nabmbar + GLOB_CNT);
	reg16 |= (1 << 1); /* Remove AC_RESET# */
	outw(reg16, nabmbar + GLOB_CNT);

	/* Wait 600ms. Ouch. */
	udelay(600 * 1000);

	init_cnr();

	/* Detect Primary AC'97 Codec */
	reg32 = inl(nabmbar + GLOB_STA);
	if ((reg32 & ((1 << 28) | (1 << 9) | (1 << 8))) == 0) {
		/* Primary Codec not found */
		printk(BIOS_DEBUG, "No primary codec. Disabling AC'97 Audio.\n");
		return;
	}

	ac97_semaphore();

	/* Detect if codec is programmable */
	outw(0x8000, nambar + MASTER_VOL);
	ac97_semaphore();
	if (inw(nambar + MASTER_VOL) != 0x8000) {
		printk(BIOS_DEBUG, "Codec not programmable. Disabling AC'97 Audio.\n");
		return;
	}

	/* Program Vendor IDs */
	reg32 = inw(nambar + VENDOR_ID1);
	reg32 <<= 16;
	reg32 |= (u16)inw(nambar + VENDOR_ID2);

	program_sigid(dev, reg32);

	/* Is Codec AC'97 2.3 compliant? */
	reg16 = inw(nambar + EXT_AUDIO);
	/* [11:10] = 10b -> AC'97 2.3 */
	if ((reg16 & 0x0c00) != 0x0800) {
		/* No 2.3 Codec. We're done */
		return;
	}

	/* Select Page 1 */
	reg16 = inw(nambar + PAGING);
	reg16 &= 0xfff0;
	reg16 |= 0x0001;
	outw(reg16, nambar + PAGING);

	for (i = 0x0a * 2; i > 0; i--) {
		outw(i, nambar + FUNC_SEL);

		/* Function could not be selected. Next one */
		if (inw(nambar + FUNC_SEL) != i)
			continue;

		reg16 = inw(nambar + INFO_IO);

		/* Function Information present? */
		if (!(reg16 & (1 << 0)))
			continue;

		/* Function Information valid? */
		if (!(reg16 & (1 << 4)))
			continue;

		/* Program Buffer Delay [9:5] */
		reg16 &= 0x03e0;
		reg16 |= ac97_function[i][0];

		/* Program Gain [15:11] */
		reg16 |= ac97_function[i][1];

		/* Program Inversion [10] */
		reg16 |= ac97_function[i][2];

		outw(reg16, nambar + INFO_IO);

		/* Program Connector / Jack Location */
		reg16 = inw(nambar + CONNECTOR);
		reg16 &= 0x1fff;
		reg16 |= ac97_function[i][3];
		outw(reg16, nambar + CONNECTOR);
	}
}

static void ac97_modem_init(struct device *dev)
{
	u16 reg16;
	u32 reg32;
	u16 mmbar, mbar;

	mmbar = pci_read_config16(dev, MMBAR) & 0xfffe;
	mbar = pci_read_config16(dev, MBAR) & 0xfffe;

	reg16 = inw(mmbar + EXT_MODEM_ID1);
	if ((reg16 & 0xc000) != 0xc000) {
		if (reg16 & (1 << 0)) {
			reg32 = inw(mmbar + VENDOR_ID2);
			reg32 <<= 16;
			reg32 |= (u16)inw(mmbar + VENDOR_ID1);
			program_sigid(dev, reg32);
			return;
		}
	}

	/* Secondary codec? */
	reg16 = inw(mbar + SEC_CODEC);
	if ((reg16 & (1 << 9)) == 0)
		return;

	reg16 = inw(mmbar + EXT_MODEM_ID2);
	if ((reg16 & 0xc000) == 0x4000) {
		if (reg16 & (1 << 0)) {
			reg32 = inw(mmbar + SEC_VENDOR_ID2);
			reg32 <<= 16;
			reg32 |= (u16)inw(mmbar + SEC_VENDOR_ID1);
			program_sigid(dev, reg32);
			return;
		}
	}
}

static struct device_operations ac97_audio_ops  = {
	.read_resources   = pci_dev_read_resources,
	.set_resources    = pci_dev_set_resources,
	.enable_resources = pci_dev_enable_resources,
	.enable           = i82801dx_enable,
	.init             = ac97_audio_init,
};

static struct device_operations ac97_modem_ops  = {
	.read_resources   = pci_dev_read_resources,
	.set_resources    = pci_dev_set_resources,
	.enable_resources = pci_dev_enable_resources,
	.enable           = i82801dx_enable,
	.init             = ac97_modem_init,
};

/* 82801DB/DBL/DBM (ICH4/ICH4-L/ICH4-M) */
static const struct pci_driver i82801db_ac97_audio __pci_driver = {
	.ops	= &ac97_audio_ops,
	.vendor	= PCI_VID_INTEL,
	.device	= PCI_DID_INTEL_82801DB_AC97_AUDIO,
};

static const struct pci_driver i82801db_ac97_modem __pci_driver = {
	.ops	= &ac97_modem_ops,
	.vendor	= PCI_VID_INTEL,
	.device	= PCI_DID_INTEL_82801DB_AC97_MODEM,
};
