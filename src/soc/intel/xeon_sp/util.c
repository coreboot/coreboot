/* SPDX-License-Identifier: GPL-2.0-only */

#include <console/console.h>
#include <device/pci.h>
#include <soc/pci_devs.h>
#include <soc/msr.h>
#include <soc/util.h>

void get_stack_busnos(uint32_t *bus)
{
	uint32_t reg1, reg2;

	reg1 = pci_mmio_read_config32(PCI_DEV(UBOX_DECS_BUS, UBOX_DECS_DEV, UBOX_DECS_FUNC),
		0xcc);
	reg2 = pci_mmio_read_config32(PCI_DEV(UBOX_DECS_BUS, UBOX_DECS_DEV, UBOX_DECS_FUNC),
		0xd0);

	for (int i = 0; i < 4; ++i)
		bus[i] = ((reg1 >> (i * 8)) & 0xff);
	for (int i = 0; i < 2; ++i)
		bus[4+i] = ((reg2 >> (i * 8)) & 0xff);
}

void unlock_pam_regions(void)
{
	uint32_t bus1 = 0;
	uint32_t pam0123_unlock_dram = 0x33333330;
	uint32_t pam456_unlock_dram = 0x00333333;

	get_cpubusnos(NULL, &bus1, NULL, NULL);
	pci_io_write_config32(PCI_DEV(bus1, SAD_ALL_DEV, SAD_ALL_FUNC),
		SAD_ALL_PAM0123_CSR, pam0123_unlock_dram);
	pci_io_write_config32(PCI_DEV(bus1, SAD_ALL_DEV, SAD_ALL_FUNC),
		SAD_ALL_PAM456_CSR, pam456_unlock_dram);

	uint32_t reg1 = pci_io_read_config32(PCI_DEV(bus1, SAD_ALL_DEV,
		SAD_ALL_FUNC), SAD_ALL_PAM0123_CSR);
	uint32_t reg2 = pci_io_read_config32(PCI_DEV(bus1, SAD_ALL_DEV,
		SAD_ALL_FUNC), SAD_ALL_PAM456_CSR);
	printk(BIOS_DEBUG, "%s:%s pam0123_csr: 0x%x, pam456_csr: 0x%x\n",
		__FILE__, __func__, reg1, reg2);
}

void get_cpubusnos(uint32_t *bus0, uint32_t *bus1, uint32_t *bus2, uint32_t *bus3)
{
	uint32_t bus = pci_io_read_config32(PCI_DEV(UBOX_DECS_BUS, UBOX_DECS_DEV,
		UBOX_DECS_FUNC), UBOX_DECS_CPUBUSNO_CSR);
	if (bus0)
		*bus0 = (bus & 0xff);
	if (bus1)
		*bus1 = (bus >> 8) & 0xff;
	if (bus2)
		*bus2 = (bus >> 16) & 0xff;
	if (bus3)
		*bus3 = (bus >> 24) & 0xff;
}

msr_t read_msr_ppin(void)
{
	msr_t ppin = {0};
	msr_t msr;

	/* If MSR_PLATFORM_INFO PPIN_CAP is 0, PPIN capability is not supported */
	msr = rdmsr(MSR_PLATFORM_INFO);
	if ((msr.lo & MSR_PPIN_CAP) == 0) {
		printk(BIOS_ERR, "MSR_PPIN_CAP is 0, PPIN is not supported\n");
		return ppin;
	}

	/* Access to MSR_PPIN is permitted only if MSR_PPIN_CTL LOCK is 0 and ENABLE is 1 */
	msr = rdmsr(MSR_PPIN_CTL);
	if (msr.lo & MSR_PPIN_CTL_LOCK) {
		printk(BIOS_ERR, "MSR_PPIN_CTL_LOCK is 1, PPIN access is not allowed\n");
		return ppin;
	}

	if ((msr.lo & MSR_PPIN_CTL_ENABLE) == 0) {
		/* Set MSR_PPIN_CTL ENABLE to 1 */
		msr.lo |= MSR_PPIN_CTL_ENABLE;
		wrmsr(MSR_PPIN_CTL, msr);
	}
	ppin = rdmsr(MSR_PPIN);
	/* Set enable to 0 after reading MSR_PPIN */
	msr.lo &= ~MSR_PPIN_CTL_ENABLE;
	wrmsr(MSR_PPIN_CTL, msr);
	return ppin;
}
