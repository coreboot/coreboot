/* SPDX-License-Identifier: GPL-2.0-only */

#define __SIMPLE_DEVICE__

#include <stdint.h>
#include <arch/io.h>
#include <console/console.h>
#include <device/pci.h>
#include <intelblocks/pmclib.h>
#include <soc/iomap.h>
#include <soc/pci_devs.h>
#include <soc/pm.h>
#include <soc/soc_util.h>

static void print_num_status_bits(int num_bits, uint32_t status,
				  const char *const bit_names[])
{
	int i;

	if (!status)
		return;

	for (i = num_bits - 1; i >= 0; i--) {
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
	static const char *const smi_sts_bits[] = {
		[2] = "BIOS",
		[4] = "SLP_SMI",
		[5] = "APM",
		[6] = "SWSMI_TMR",
		[8] = "PM1",
		[9] = "GPE0",
		[10] = "GPE1",
		[11] = "MC_SMI",
		[12] = "DEVMON",
		[13] = "TCO",
		[14] = "PERIODIC",
		[15] = "SERIRQ",
		[16] = "SMBUS_SMI",
		[17] = "LEGACY_USB2",
		[18] = "INTEL_USB2",
		[19] = "PATCH",
		[20] = "PCI_EXP_SMI",
		[21] = "MONITOR",
		[26] = "SPI",
		[27] = "GPIO_UNLOCK",
		[31] = "LEGACY_USB3",
	};

	if (!smi_sts)
		return 0;

	printk(BIOS_DEBUG, "SMI_STS: ");
	print_num_status_bits(ARRAY_SIZE(smi_sts_bits), smi_sts, smi_sts_bits);
	printk(BIOS_DEBUG, "\n");

	return smi_sts;
}

static uint32_t reset_smi_status(void)
{
	uint16_t pmbase = get_pmbase();
	uint32_t smi_sts = inl((uint16_t)(pmbase + SMI_STS));
	outl(smi_sts, (uint16_t)(pmbase + SMI_STS));
	return smi_sts;
}

uint32_t clear_smi_status(void) { return print_smi_status(reset_smi_status()); }

void enable_smi(uint32_t mask)
{
	uint16_t pmbase = get_pmbase();
	uint32_t smi_en = inl((uint16_t)(pmbase + SMI_EN));
	smi_en |= mask;
	outl(smi_en, (uint16_t)(pmbase + SMI_EN));
}

uint8_t *pmc_mmio_regs(void)
{
	uint32_t reg32;

	reg32 = pci_read_config32(PCH_DEV_PMC, PMC_PWRM_BASE);

	/* 4KiB alignment. */
	reg32 &= ~0xfff;

	return (void *)(uintptr_t) reg32;
}

void disable_smi(uint32_t mask)
{
	uint16_t pmbase = get_pmbase();
	uint32_t smi_en = inl((uint16_t)(pmbase + SMI_EN));
	smi_en &= ~mask;
	outl(smi_en, (uint16_t)(pmbase + SMI_EN));
}

void enable_pm1_control(uint32_t mask)
{
	uint16_t pmbase = get_pmbase();
	uint32_t pm1_cnt = inl((uint16_t)(pmbase + PM1_CNT));
	pm1_cnt |= mask;
	outl(pm1_cnt, (uint16_t)(pmbase + PM1_CNT));
}

void disable_pm1_control(uint32_t mask)
{
	uint16_t pmbase = get_pmbase();
	uint32_t pm1_cnt = inl((uint16_t)(pmbase + PM1_CNT));
	pm1_cnt &= ~mask;
	outl(pm1_cnt, (uint16_t)(pmbase + PM1_CNT));
}

static uint16_t reset_pm1_status(void)
{
	uint16_t pmbase = get_pmbase();
	uint16_t pm1_sts = inw((uint16_t)(pmbase + PM1_STS));
	outw(pm1_sts, (uint16_t)(pmbase + PM1_STS));
	return pm1_sts;
}

static uint16_t print_pm1_status(uint16_t pm1_sts)
{
	static const char *const pm1_sts_bits[] = {
		[0] = "TMROF",  [4] = "BM",   [5] = "GBL",
		[8] = "PWRBTN", [10] = "RTC", [11] = "PRBTNOR",
		[15] = "WAK",
	};

	if (!pm1_sts)
		return 0;

	printk(BIOS_DEBUG, "PM1_STS: ");
	print_num_status_bits(ARRAY_SIZE(pm1_sts_bits), pm1_sts, pm1_sts_bits);
	printk(BIOS_DEBUG, "\n");

	return pm1_sts;
}

uint16_t clear_pm1_status(void) { return print_pm1_status(reset_pm1_status()); }

void enable_pm1(uint16_t events)
{
	uint16_t pmbase = get_pmbase();
	outw(events, (uint16_t)(pmbase + PM1_EN));
}

static uint32_t print_tco_status(uint32_t tco_sts)
{
	static const char *const tco_sts_bits[] = {
		[0] = "NMI2SMI",     [1] = "OS_TCO_SMI",
		[2] = "TCO_INIT",    [3] = "TIMEOUT",
		[7] = "NEWCENTURY ", [8] = "BIOSWR ",
		[9] = "CPUSCI ",     [10] = "CPUSMI ",
		[12] = "CPUSERR ",   [16] = "INTRD_DET ",
		[17] = "SECOND_TO",  [20] = "SMLINK_SLV_SMI",
	};

	if (!tco_sts)
		return 0;

	printk(BIOS_DEBUG, "TCO_STS: ");
	print_num_status_bits(ARRAY_SIZE(tco_sts_bits), tco_sts, tco_sts_bits);
	printk(BIOS_DEBUG, "\n");

	return tco_sts;
}

static uint32_t reset_tco_status(void)
{
	uint16_t tcobase = get_tcobase();
	uint32_t tco_sts = inl((uint16_t)(tcobase + TCO1_STS));
	uint32_t tco_en = inl((uint16_t)(tcobase + TCO1_CNT));

	outl(tco_sts, (uint16_t)(tcobase + TCO1_STS));
	return tco_sts & tco_en;
}

uint32_t clear_tco_status(void) { return print_tco_status(reset_tco_status()); }

void enable_gpe(uint32_t mask)
{
	uint16_t pmbase = get_pmbase();
	uint32_t gpe0_en = inl((uint16_t)(pmbase + GPE0_EN(GPE_STD)));
	gpe0_en |= mask;
	outl(gpe0_en, (uint16_t)(pmbase + GPE0_EN(GPE_STD)));
}

void disable_gpe(uint32_t mask)
{
	uint16_t pmbase = get_pmbase();
	uint32_t gpe0_en = inl((uint16_t)(pmbase + GPE0_EN(GPE_STD)));
	gpe0_en &= ~mask;
	outl(gpe0_en, (uint16_t)(pmbase + GPE0_EN(GPE_STD)));
}

void disable_all_gpe(void) { disable_gpe(~0); }

static uint32_t reset_gpe_status(void)
{
	uint16_t pmbase = get_pmbase();
	uint32_t gpe_sts = inl((uint16_t)(pmbase + GPE0_STS(GPE_STD)));
	outl(gpe_sts, (uint16_t)(pmbase + GPE0_STS(GPE_STD)));
	return gpe_sts;
}

static uint32_t print_gpe_sts(uint32_t gpe_sts)
{
	static const char *const gpe_sts_bits[] = {
		[0] = "GPIO_0", [1] = "GPIO_1",
		[2] = "GPIO_2", [3] = "GPIO_3",
		[4] = "GPIO_4", [5] = "GPIO_5",
		[6] = "GPIO_6", [7] = "GPIO_7",
		[8] = "GPIO_8", [9] = "GPIO_9",
		[10] = "GPIO_10", [11] = "GPIO_11",
		[12] = "GPIO_12", [13] = "GPIO_13",
		[14] = "GPIO_14", [15] = "GPIO_15",
		[16] = "GPIO_16", [17] = "GPIO_17",
		[18] = "GPIO_18", [19] = "GPIO_19",
		[20] = "GPIO_20", [21] = "GPIO_21",
		[22] = "GPIO_22", [23] = "GPIO_23",
		[24] = "GPIO_24", [25] = "GPIO_25",
		[26] = "GPIO_26", [27] = "GPIO_27",
		[28] = "GPIO_28", [29] = "GPIO_29",
		[30] = "GPIO_30", [31] = "GPIO_31",
	};

	if (!gpe_sts)
		return gpe_sts;

	printk(BIOS_DEBUG, "GPE0a_STS: ");
	print_num_status_bits(ARRAY_SIZE(gpe_sts_bits), gpe_sts, gpe_sts_bits);
	printk(BIOS_DEBUG, "\n");

	return gpe_sts;
}

uint32_t clear_gpe_status(void) { return print_gpe_sts(reset_gpe_status()); }

void clear_pmc_status(void) { /* TODO */ }
