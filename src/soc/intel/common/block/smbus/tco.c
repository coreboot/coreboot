/* SPDX-License-Identifier: GPL-2.0-only */

#define __SIMPLE_DEVICE__

#include <arch/io.h>
#include <device/pci_ops.h>
#include <device/device.h>
#include <device/pci.h>
#include <intelblocks/gpmr.h>
#include <intelblocks/pcr.h>
#include <intelblocks/pmclib.h>
#include <intelblocks/tco.h>
#include <soc/iomap.h>
#include <soc/pci_devs.h>
#include <soc/pcr_ids.h>
#include <soc/pm.h>
#include <soc/smbus.h>

/* SMBUS TCO base address. */
#define TCOBASE		0x50
#define TCOCTL		0x54
#define  TCO_BASE_EN		(1 << 8)
#define  TCO_BASE_LOCK		(1 << 0)

/* Get base address of TCO I/O registers. */
static uint16_t tco_get_bar(void)
{
	return TCO_BASE_ADDRESS;
}

uint16_t tco_read_reg(uint16_t tco_reg)
{
	uint16_t tcobase;

	tcobase = tco_get_bar();

	return inw(tcobase + tco_reg);
}

void tco_write_reg(uint16_t tco_reg, uint16_t value)
{
	uint16_t tcobase;

	tcobase = tco_get_bar();

	outw(value, tcobase + tco_reg);
}

void tco_lockdown(void)
{
	uint16_t tcocnt;
	const pci_devfn_t dev = PCH_DEV_SMBUS;

	/* TCO base address lockdown */
	pci_or_config32(dev, TCOCTL, TCO_BASE_LOCK);

	/* TCO Lock down */
	tcocnt = tco_read_reg(TCO1_CNT);
	tcocnt |= TCO_LOCK;
	tco_write_reg(TCO1_CNT, tcocnt);
}

uint32_t tco_reset_status(void)
{
	uint16_t tco1_sts;
	uint16_t tco2_sts;

	/* TCO Status 1 register */
	tco1_sts = tco_read_reg(TCO1_STS);
	tco_write_reg(TCO1_STS, tco1_sts);

	/* TCO Status 2 register */
	tco2_sts = tco_read_reg(TCO2_STS);
	tco_write_reg(TCO2_STS, tco2_sts | TCO2_STS_SECOND_TO);

	return (tco2_sts << 16) | tco1_sts;
}

/* Stop TCO timer */
static void tco_timer_disable(void)
{
	uint16_t tcocnt;

	/* Program TCO timer halt */
	tcocnt = tco_read_reg(TCO1_CNT);
	tcocnt |= TCO_TMR_HLT;
	tco_write_reg(TCO1_CNT, tcocnt);
}

/* Enable and initialize TCO intruder SMI */
static void tco_intruder_smi_enable(void)
{
	uint16_t tcocnt;

	/* Make TCO issue an SMI on INTRD_DET assertion */
	tcocnt = tco_read_reg(TCO2_CNT);
	tcocnt &= ~TCO_INTRD_SEL_MASK;
	tcocnt |= TCO_INTRD_SEL_SMI;
	tco_write_reg(TCO2_CNT, tcocnt);
}

/* Enable TCO BAR using SMBUS TCO base to access TCO related register */
static void tco_enable_bar(void)
{
	uint32_t reg32;
	uint16_t tcobase;
	const pci_devfn_t dev = PCH_DEV_SMBUS;

	/* Disable TCO in SMBUS Device first before changing Base Address */
	reg32 = pci_read_config32(dev, TCOCTL);
	reg32 &= ~TCO_BASE_EN;
	pci_write_config32(dev, TCOCTL, reg32);

	/* Program TCO Base */
	tcobase = tco_get_bar();
	pci_write_config32(dev, TCOBASE, tcobase);

	/* Enable TCO in SMBUS */
	pci_write_config32(dev, TCOCTL, reg32 | TCO_BASE_EN);

	/* Program TCO Base Address */
	gpmr_write32(GPMR_TCOBASE, tcobase | GPMR_TCOEN);
}

/*
 * Enable TCO BAR using SMBUS TCO base to access TCO related register
 * also disable the timer.
 */
void tco_configure(void)
{
	if (CONFIG(SOC_INTEL_COMMON_BLOCK_TCO_ENABLE_THROUGH_SMBUS))
		tco_enable_bar();

	tco_timer_disable();

	/* Enable intruder interrupt if TCO interrupts are enabled*/
	if (CONFIG(SOC_INTEL_COMMON_BLOCK_SMM_TCO_ENABLE))
		tco_intruder_smi_enable();
}
