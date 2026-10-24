/* SPDX-License-Identifier: GPL-2.0-only */

#include <amdblocks/psp.h>
#include <amdblocks/root_complex.h>
#include <amdblocks/smn.h>
#include <console/console.h>
#include <device/mmio.h>
#include <types.h>
#include "psp_def.h"

#define IOHC_MISC_PSP_MMIO_REG		0x2e0

static uint64_t get_psp_mmio_mask(void)
{
	const struct non_pci_mmio_reg *mmio_regs;
	size_t reg_count;
	mmio_regs = get_iohc_non_pci_mmio_regs(&reg_count);

	for (size_t i = 0; i < reg_count; i++) {
		if (mmio_regs[i].iohc_misc_offset == IOHC_MISC_PSP_MMIO_REG)
			return mmio_regs[i].mask;
	}

	printk(BIOS_ERR, "No PSP MMIO register description found.\n");
	return 0;
}

#define PSP_MMIO_LOCK	BIT(8)

/* Getting the PSP MMIO base from the domain resources only works in ramstage, but not in SMM,
   so we have to read this from the hardware registers */
static uintptr_t get_psp_mmio_base(void)
{
	static uintptr_t psp_mmio_base;
	const struct domain_iohc_info *iohc;
	size_t iohc_count;

	if (psp_mmio_base)
		return psp_mmio_base;

	iohc = get_iohc_info(&iohc_count);
	const uint64_t psp_mmio_mask = get_psp_mmio_mask();

	if (!psp_mmio_mask)
		return 0;

	for (size_t i = 0; i < iohc_count; i++) {
		/* Both ramstage and SMM use the PSP mailbox on the primary socket. */
		if (!iohc[i].misc_smn_base || iohc[i].socket != 0)
			continue;
		uint64_t reg64 = smn_read64_at(iohc[i].smn_dev,
					     iohc[i].misc_smn_base | IOHC_MISC_PSP_MMIO_REG);

		if (!(reg64 & IOHC_MMIO_EN))
			continue;

		const uint64_t base = reg64 & psp_mmio_mask;

		if (ENV_X86_32 && base >= 4ull * GiB) {
			printk(BIOS_WARNING, "PSP MMIO base above 4GB.\n");
			continue;
		}

		/* If the PSP MMIO base is enabled but the register isn't locked, set the lock
		   bit. This shouldn't happen, but better be a bit too careful here */
		if (!(reg64 & PSP_MMIO_LOCK)) {
			printk(BIOS_WARNING, "Enabled PSP MMIO in domain %zu isn't locked. "
					     "Locking it.\n", i);
			reg64 |= PSP_MMIO_LOCK;
			/* Since the lock bit lives in the lower one of the two 32 bit SMN
			   registers, we only need to write that one to lock it */
			smn_write32_at(iohc[i].smn_dev,
				       iohc[i].misc_smn_base | IOHC_MISC_PSP_MMIO_REG,
				       reg64 & 0xffffffff);
		}

		psp_mmio_base = base;
	}

	if (!psp_mmio_base)
		printk(BIOS_ERR, "No usable PSP MMIO found.\n");

	return psp_mmio_base;
}

uint64_t psp_get_base(void)
{
	return get_psp_mmio_base();
}

uint32_t psp_read32(uint64_t base, uint32_t offset)
{
	return read32p((uintptr_t)(base | offset));
}

void psp_write32(uint64_t base, uint32_t offset, uint32_t data)
{
	write32p((uintptr_t)(base | offset), data);
}

void psp_write64(uint64_t base, uint32_t offset, uint64_t data)
{
	write64p((uintptr_t)(base | offset), data);
}
