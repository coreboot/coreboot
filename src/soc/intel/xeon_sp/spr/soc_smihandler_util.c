/* SPDX-License-Identifier: GPL-2.0-only */

#include <cpu/x86/msr.h>
#include <device/pci.h>
#include <soc/msr.h>
#include <soc/pci_devs.h>

pci_devfn_t soc_get_ubox_pmon_dev(void)
{
	msr_t msr = rdmsr(MSR_CPU_BUSNO);
	u16 bus;

	if (msr.hi & BUSNO_VALID)
		bus = msr.lo & 0xff;
	else
		return 0;

	return PCI_DEV(bus, UBOX_DECS_DEV, UBOX_URACU_FUNC);
}
