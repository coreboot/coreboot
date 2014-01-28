/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2013 Google Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
 */

#include <stdint.h>
#include <arch/io.h>
#include <console/console.h>

#include <baytrail/lpc.h>
#include <baytrail/pci_devs.h>
#include <baytrail/pmc.h>

#if defined(__SMM__)

static const device_t pcu_dev = PCI_DEV(0, PCU_DEV, 0);

static inline device_t get_pcu_dev(void)
{
	return pcu_dev;
}

#else /* !__SMM__ */
#include <device/device.h>
#include <device/pci.h>

static device_t pcu_dev;
static device_t get_pcu_dev(void)
{
	if (pcu_dev == NULL)
		pcu_dev = dev_find_slot(0, PCI_DEVFN(PCU_DEV, 0));
	return pcu_dev;
}
#endif

uint16_t get_pmbase(void)
{
	return pci_read_config16(get_pcu_dev(), ABASE) & 0xfff8;
}

static void print_status_bits(uint32_t status, const char *bit_names[])
{
	int i;

	if (!status)
		return;

	for (i = 31; i >= 0; i--) {
		if (status & (1 << i)) {
			if (bit_names[i])
				printk(BIOS_DEBUG, "%s ", bit_names[i]);
			else
				printk(BIOS_DEBUG, "BIT%d ", i);
		}
	}
}

static uint32_t print_smi_status(uint32_t smi_sts)
{
	static const char *smi_sts_bits[] = {
		[2] = "BIOS",
		[4] = "SLP_SMI",
		[5] = "APM",
		[6] = "SWSMI_TMR",
		[8] = "PM1",
		[9] = "GPE0",
		[12] = "DEVMON",
		[13] = "TCO",
		[14] = "PERIODIC",
		[15] = "ILB",
		[16] = "SMBUS_SMI",
		[17] = "LEGACY_USB2",
		[18] = "INTEL_USB2",
		[20] = "PCI_EXP_SMI",
		[26] = "SPI",
		[28] = "PUNIT",
		[29] = "GUNIT",
	};

	if (!smi_sts)
		return 0;

	printk(BIOS_DEBUG, "SMI_STS: ");
	print_status_bits(smi_sts, smi_sts_bits);
	printk(BIOS_DEBUG, "\n");

	return smi_sts;
}

static uint32_t reset_smi_status(void)
{
	uint16_t pmbase = get_pmbase();
	uint32_t smi_sts = inl(pmbase + SMI_STS);
	outl(smi_sts, pmbase + SMI_STS);
	return smi_sts;
}

uint32_t clear_smi_status(void)
{
	return print_smi_status(reset_smi_status());
}

void enable_smi(uint32_t mask)
{
	uint16_t pmbase = get_pmbase();
	uint32_t smi_en = inl(pmbase + SMI_EN);
	smi_en |= mask;
	outl(smi_en, pmbase + SMI_EN);
}

void disable_smi(uint32_t mask)
{
	uint16_t pmbase = get_pmbase();
	uint32_t smi_en = inl(pmbase + SMI_EN);
	smi_en &= ~mask;
	outl(smi_en, pmbase + SMI_EN);
}

void enable_pm1_control(uint32_t mask)
{
	uint16_t pmbase = get_pmbase();
	uint32_t pm1_cnt = inl(pmbase + PM1_CNT);
	pm1_cnt |= mask;
	outl(pm1_cnt, pmbase + PM1_CNT);
}

void disable_pm1_control(uint32_t mask)
{
	uint16_t pmbase = get_pmbase();
	uint32_t pm1_cnt = inl(pmbase + PM1_CNT);
	pm1_cnt &= ~mask;
	outl(pm1_cnt, pmbase + PM1_CNT);
}

static uint16_t reset_pm1_status(void)
{
	uint16_t pmbase = get_pmbase();
	uint16_t pm1_sts = inw(pmbase + PM1_STS);
	outw(pm1_sts, pmbase + PM1_STS);
	return pm1_sts;
}

static uint16_t print_pm1_status(uint16_t pm1_sts)
{
	static const char *pm1_sts_bits[] = {
		[0] = "TMROF",
		[5] = "GBL",
		[8] = "PWRBTN",
		[10] = "RTC",
		[11] = "PRBTNOR",
		[13] = "USB",
		[14] = "PCIEXPWAK",
		[15] = "WAK",
	};

	if (!pm1_sts)
		return 0;

	printk(BIOS_SPEW, "PM1_STS: ");
	print_status_bits(pm1_sts, pm1_sts_bits);
	printk(BIOS_SPEW, "\n");

	return pm1_sts;
}

uint16_t clear_pm1_status(void)
{
	return print_pm1_status(reset_pm1_status());
}

void enable_pm1(uint16_t events)
{
	outw(events, get_pmbase() + PM1_EN);
}

static uint32_t print_tco_status(uint32_t tco_sts)
{
	static const char *tco_sts_bits[] = {
		[3] = "TIMEOUT",
		[17] = "SECOND_TO",
	};

	if (!tco_sts)
		return 0;

	printk(BIOS_DEBUG, "TCO_STS: ");
	print_status_bits(tco_sts, tco_sts_bits);
	printk(BIOS_DEBUG, "\n");

	return tco_sts;
}

static uint32_t reset_tco_status(void)
{
	uint16_t pmbase = get_pmbase();
	uint32_t tco_sts = inl(pmbase + TCO_STS);
	uint32_t tco_en = inl(pmbase + TCO1_CNT);

	outl(tco_sts, pmbase + TCO_STS);
	return tco_sts & tco_en;
}

uint32_t clear_tco_status(void)
{
	return print_tco_status(reset_tco_status());
}

void enable_gpe(uint32_t mask)
{
	uint16_t pmbase = get_pmbase();
	uint32_t gpe0_en = inl(pmbase + GPE0_EN);
	gpe0_en |= mask;
	outl(gpe0_en, pmbase + GPE0_EN);
}

void disable_gpe(uint32_t mask)
{
	uint16_t pmbase = get_pmbase();
	uint32_t gpe0_en = inl(pmbase + GPE0_EN);
	gpe0_en &= ~mask;
	outl(gpe0_en, pmbase + GPE0_EN);
}

void disable_all_gpe(void)
{
	disable_gpe(~0);
}


static uint32_t reset_gpe_status(void)
{
	uint16_t pmbase = get_pmbase();
	uint32_t gpe_sts = inl(pmbase + GPE0_STS);
	outl(gpe_sts, pmbase + GPE0_STS);
	return gpe_sts;
}

static uint32_t print_gpe_sts(uint32_t gpe_sts)
{
	static const char *gpe_sts_bits[] = {
		[1] = "HOTPLUG",
		[2] = "SWGPE",
		[3] = "PCIE_WAKE0",
		[4] = "PUNIT",
		[5] = "GUNIT",
		[6] = "PCIE_WAKE1",
		[7] = "PCIE_WAKE2",
		[8] = "PCIE_WAKE3",
		[9] = "PCI_EXP",
		[10] = "BATLOW",
		[13] = "PME_B0",
		[16] = "SUS_GPIO_0",
		[17] = "SUS_GPIO_1",
		[18] = "SUS_GPIO_2",
		[19] = "SUS_GPIO_3",
		[20] = "SUS_GPIO_4",
		[21] = "SUS_GPIO_5",
		[22] = "SUS_GPIO_6",
		[23] = "SUS_GPIO_7",
		[24] = "CORE_GPIO_0",
		[25] = "CORE_GPIO_1",
		[26] = "CORE_GPIO_2",
		[27] = "CORE_GPIO_3",
		[28] = "CORE_GPIO_4",
		[29] = "CORE_GPIO_5",
		[30] = "CORE_GPIO_6",
		[31] = "CORE_GPIO_7",
	};

	if (!gpe_sts)
		return gpe_sts;

	printk(BIOS_DEBUG, "GPE0a_STS: ");
	print_status_bits(gpe_sts, gpe_sts_bits);
	printk(BIOS_DEBUG, "\n");

	return gpe_sts;
}

uint32_t clear_gpe_status(void)
{
	return print_gpe_sts(reset_gpe_status());
}
