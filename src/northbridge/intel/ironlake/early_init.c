/* SPDX-License-Identifier: GPL-2.0-only */

#include <stdint.h>
#include <console/console.h>
#include <arch/io.h>
#include <device/pci_ops.h>
#include <device/pci_def.h>
#include <elog.h>
#include <cpu/x86/msr.h>
#include <cpu/intel/speedstep.h>
#include <cpu/intel/turbo.h>
#include <arch/cpu.h>

#include "ironlake.h"

static void ironlake_setup_bars(void)
{
	printk(BIOS_DEBUG, "Setting up static northbridge registers...");
	/* Set up all hardcoded northbridge BARs */
	pci_write_config32(PCI_DEV(0, 0, 0), EPBAR,  CONFIG_FIXED_EPBAR_MMIO_BASE  | 1);
	pci_write_config32(PCI_DEV(0, 0, 0), EPBAR  + 4, 0);
	pci_write_config32(PCI_DEV(0, 0, 0), MCHBAR, CONFIG_FIXED_MCHBAR_MMIO_BASE | 1);
	pci_write_config32(PCI_DEV(0, 0, 0), MCHBAR + 4, 0);
	pci_write_config32(PCI_DEV(0, 0, 0), DMIBAR, CONFIG_FIXED_DMIBAR_MMIO_BASE | 1);
	pci_write_config32(PCI_DEV(0, 0, 0), DMIBAR + 4, 0);

	/* Set C0000-FFFFF to access RAM on both reads and writes */
	pci_write_config8(QPI_SAD, QPD0F1_PAM(0), 0x30);
	pci_write_config8(QPI_SAD, QPD0F1_PAM(1), 0x33);
	pci_write_config8(QPI_SAD, QPD0F1_PAM(2), 0x33);
	pci_write_config8(QPI_SAD, QPD0F1_PAM(3), 0x33);
	pci_write_config8(QPI_SAD, QPD0F1_PAM(4), 0x33);
	pci_write_config8(QPI_SAD, QPD0F1_PAM(5), 0x33);
	pci_write_config8(QPI_SAD, QPD0F1_PAM(6), 0x33);

	printk(BIOS_DEBUG, " done.\n");
}

static void early_cpu_init(void)
{
	msr_t m;

	/* bit 0 = disable multicore,
	   bit 1 = disable quadcore,
	   bit 8 = disable hyperthreading.  */
	pci_update_config32(QPI_NON_CORE, DESIRED_CORES, 0xfffffefc, 0x10000);

	u8 reg8;
	struct cpuid_result result;
	result = cpuid_ext(0x6, 0x8b);
	if (!(result.eax & 0x2)) {
		m = rdmsr(MSR_FSB_CLOCK_VCC);
		reg8 = ((m.lo & 0xff00) >> 8) + 1;
		m = rdmsr(IA32_PERF_CTL);
		m.lo = (m.lo & ~0xff) | reg8;
		wrmsr(IA32_PERF_CTL, m);

		m = rdmsr(IA32_MISC_ENABLE);
		m.hi &= ~0x00000040;
		m.lo |= 0x10000;

		wrmsr(IA32_MISC_ENABLE, m);
	}

	m = rdmsr(MSR_FSB_CLOCK_VCC);
	reg8 = ((m.lo & 0xff00) >> 8) + 1;

	m = rdmsr(IA32_PERF_CTL);
	m.lo = (m.lo & ~0xff) | reg8;
	wrmsr(IA32_PERF_CTL, m);

	m = rdmsr(IA32_MISC_ENABLE);
	m.lo |= 0x10000;
	wrmsr(IA32_MISC_ENABLE, m);
}

void ironlake_early_initialization(int chipset_type)
{
	u32 capid0_a;
	u8 reg8;
	int s3_resume;

	/* Device ID Override Enable should be done very early */
	capid0_a = pci_read_config32(PCI_DEV(0, 0, 0), 0xe4);
	if (capid0_a & (1 << 10)) {
		reg8 = pci_read_config8(PCI_DEV(0, 0, 0), 0xf3);
		reg8 &= ~7;	/* Clear 2:0 */

		if (chipset_type == IRONLAKE_MOBILE)
			reg8 |= 1;	/* Set bit 0 */

		pci_write_config8(PCI_DEV(0, 0, 0), 0xf3, reg8);
	}

	/* Setup all BARs required for early PCIe and raminit */
	ibexpeak_setup_bars();
	ironlake_setup_bars();

	s3_resume = (inw(DEFAULT_PMBASE + PM1_STS) & WAK_STS) &&
	    (((inl(DEFAULT_PMBASE + PM1_CNT) >> 10) & 7) == SLP_TYP_S3);

	elog_boot_notify(s3_resume);

	/* Device Enable */
	pci_write_config32(PCI_DEV(0, 0, 0), DEVEN, DEVEN_IGD | DEVEN_PEG10 | DEVEN_HOST);

	early_cpu_init();

	pci_write_config32(PCI_DEV(0, 0x16, 0), 0x10, (uintptr_t)DEFAULT_HECIBAR);
	pci_write_config32(PCI_DEV(0, 0x16, 0), PCI_COMMAND,
			   PCI_COMMAND_MASTER | PCI_COMMAND_MEMORY);

	/* Magic for S3 resume. Must be done early.  */
	if (s3_resume) {
		mchbar_clrsetbits32(0x1e8, 1, 6);
		mchbar_clrsetbits32(0x1e8, 3, 4);
	}
}
