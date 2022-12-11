/* SPDX-License-Identifier: GPL-2.0-only */

#define __SIMPLE_DEVICE__

#include <stdint.h>
#include <acpi/acpi.h>
#include <acpi/acpi_pm.h>
#include <arch/io.h>
#include <bootmode.h>
#include <device/device.h>
#include <device/mmio.h>
#include <device/pci.h>
#include <device/pci_ops.h>
#include <console/console.h>

#include <soc/iomap.h>
#include <soc/lpc.h>
#include <soc/pci_devs.h>
#include <soc/pm.h>
#include <security/vboot/vbnv.h>

uint16_t get_pmbase(void)
{
	return pci_read_config16(PCI_DEV(0, PCU_DEV, 0), ABASE) & 0xfff8;
}

static void print_num_status_bits(int num_bits, uint32_t status, const char *const bit_names[])
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
	print_num_status_bits(30, smi_sts, smi_sts_bits);
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
	static const char *const pm1_sts_bits[] = {
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
	print_num_status_bits(16, pm1_sts, pm1_sts_bits);
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
	static const char *const tco_sts_bits[] = {
		[3] = "TIMEOUT",
		[17] = "SECOND_TO",
	};

	if (!tco_sts)
		return 0;

	printk(BIOS_DEBUG, "TCO_STS: ");
	print_num_status_bits(18, tco_sts, tco_sts_bits);
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
	static const char *const gpe_sts_bits[] = {
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
	print_num_status_bits(32, gpe_sts, gpe_sts_bits);
	printk(BIOS_DEBUG, "\n");

	return gpe_sts;
}

uint32_t clear_gpe_status(void)
{
	return print_gpe_sts(reset_gpe_status());
}

static uint32_t reset_alt_status(void)
{
	uint16_t pmbase = get_pmbase();
	uint32_t alt_gpio_smi = inl(pmbase + ALT_GPIO_SMI);
	outl(alt_gpio_smi, pmbase + ALT_GPIO_SMI);
	return alt_gpio_smi;
}

static uint32_t print_alt_sts(uint32_t alt_gpio_smi)
{
	uint32_t alt_gpio_sts;
	static const char *const alt_gpio_smi_sts_bits[] = {
		[0]  = "SUS_GPIO_0",
		[1]  = "SUS_GPIO_1",
		[2]  = "SUS_GPIO_2",
		[3]  = "SUS_GPIO_3",
		[4]  = "SUS_GPIO_4",
		[5]  = "SUS_GPIO_5",
		[6]  = "SUS_GPIO_6",
		[7]  = "SUS_GPIO_7",
		[8]  = "CORE_GPIO_0",
		[9]  = "CORE_GPIO_1",
		[10] = "CORE_GPIO_2",
		[11] = "CORE_GPIO_3",
		[12] = "CORE_GPIO_4",
		[13] = "CORE_GPIO_5",
		[14] = "CORE_GPIO_6",
		[15] = "CORE_GPIO_7",
	};

	/* Status bits are in the upper 16 bits. */
	alt_gpio_sts = alt_gpio_smi >> 16;
	if (!alt_gpio_sts)
		return alt_gpio_smi;

	printk(BIOS_DEBUG, "ALT_GPIO_SMI: ");
	print_num_status_bits(16, alt_gpio_sts, alt_gpio_smi_sts_bits);
	printk(BIOS_DEBUG, "\n");

	return alt_gpio_smi;
}

uint32_t clear_alt_status(void)
{
	return print_alt_sts(reset_alt_status());
}

void clear_pmc_status(void)
{
	uint32_t prsts;
	uint32_t gen_pmcon1;

	prsts = read32p(PMC_BASE_ADDRESS + PRSTS);
	gen_pmcon1 = read32p(PMC_BASE_ADDRESS + GEN_PMCON1);

	/* Clear the status bits. The RPS field is cleared on a 0 write. */
	write32p(PMC_BASE_ADDRESS + GEN_PMCON1, gen_pmcon1 & ~RPS);
	write32p(PMC_BASE_ADDRESS + PRSTS, prsts);
}

int rtc_failure(void)
{
	uint32_t gen_pmcon1;
	int rtc_fail;
	struct chipset_power_state *ps = acpi_get_pm_state();

	if (ps != NULL)
		gen_pmcon1 = ps->gen_pmcon1;
	else
		gen_pmcon1 = read32((u32 *)(PMC_BASE_ADDRESS + GEN_PMCON1));

	rtc_fail = !!(gen_pmcon1 & RPS);
	if (rtc_fail)
		printk(BIOS_DEBUG, "RTC failure.\n");

	return rtc_fail;
}

int vbnv_cmos_failed(void)
{
	return rtc_failure();
}

int platform_is_resuming(void)
{
	if (!(inw(ACPI_BASE_ADDRESS + PM1_STS) & WAK_STS))
		return 0;

	return acpi_sleep_from_pm1(inl(ACPI_BASE_ADDRESS + PM1_CNT)) == ACPI_S3;
}
