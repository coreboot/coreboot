/* SPDX-License-Identifier: GPL-2.0-only */

#include <bootstate.h>
#include <console/console.h>
#include <console/debug.h>
#include <cpu/x86/mp.h>
#include <cpu/x86/smm.h>
#include <device/pci.h>
#include <intelpch/lockdown.h>
#include <soc/msr.h>
#include <soc/pci_devs.h>
#include <soc/pm.h>
#include <soc/util.h>
#include <smp/spinlock.h>

#include "chip.h"

static void lock_pam0123(void)
{
	const struct device *dev;

	if (get_lockdown_config() != CHIPSET_LOCKDOWN_COREBOOT)
		return;

	dev = pcidev_path_on_bus(get_stack_busno(1), PCI_DEVFN(SAD_ALL_DEV, SAD_ALL_FUNC));
	pci_or_config32(dev, SAD_ALL_PAM0123_CSR, PAM_LOCK);
}

DECLARE_SPIN_LOCK(msr_ppin_lock);

static void lock_msr_ppin_ctl(void *unused)
{
	msr_t msr;

	msr = rdmsr(MSR_PLATFORM_INFO);
	if ((msr.lo & MSR_PPIN_CAP) == 0)
		return;

	spin_lock(&msr_ppin_lock);

	msr = rdmsr(MSR_PPIN_CTL);
	if (msr.lo & MSR_PPIN_CTL_LOCK) {
		spin_unlock(&msr_ppin_lock);
		return;
	}

	/* Clear enable and lock it */
	msr.lo &= ~MSR_PPIN_CTL_ENABLE;
	msr.lo |= MSR_PPIN_CTL_LOCK;
	wrmsr(MSR_PPIN_CTL, msr);

	spin_unlock(&msr_ppin_lock);
}

static void soc_finalize(void *unused)
{
	printk(BIOS_DEBUG, "Finalizing chipset.\n");

	/*
	 * Disable ACPI PM timer based on Kconfig
	 *
	 * Disabling ACPI PM timer is necessary for XTAL OSC shutdown.
	 * Disabling ACPI PM timer also switches off TCO.
	 *
	 * Note: In contrast to other platforms supporting PM timer emulation,
	 * disabling the PM timer must be done *after* FSP has run on Xeon-SP,
	 * because FSP makes use of the PM timer.
	 */
	if (!CONFIG(USE_PM_ACPI_TIMER))
		setbits8(pmc_mmio_regs() + PCH_PWRM_ACPI_TMR_CTL, ACPI_TIM_DIS);

	apm_control(APM_CNT_FINALIZE);
	lock_pam0123();

	if (CONFIG_MAX_SOCKET > 1) {
		/* This MSR is package scope but run for all cpus for code simplicity */
		if (mp_run_on_all_cpus(&lock_msr_ppin_ctl, NULL) != CB_SUCCESS)
			printk(BIOS_ERR, "Lock PPIN CTL MSR failed\n");
	} else {
		lock_msr_ppin_ctl(NULL);
	}

	post_code(POST_OS_BOOT);
}

BOOT_STATE_INIT_ENTRY(BS_PAYLOAD_LOAD, BS_ON_ENTRY, soc_finalize, NULL);
