/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <console/console.h>
#include <device/pci_def.h>
#include <device/pci_mmio_cfg.h>
#include <device/pci_ops.h>
#include <northbridge/intel/haswell/haswell.h>
#include <northbridge/intel/haswell/vcu_mailbox.h>
#include <types.h>

#define PEG_DEV(func)		PCI_DEV(0, 1, func)

#define MAX_PEG_FUNC		3

static void peg_dmi_unset_and_set_mask_pcicfg(
	volatile union pci_bank *const bank,
	const uint32_t offset,
	const uint32_t unset_mask,
	const uint32_t set_mask,
	const uint32_t shift,
	const bool valid)
{
	if (!valid)
		return;

	volatile uint32_t *const addr = &bank->reg32[offset / sizeof(uint32_t)];
	clrsetbits32(addr, unset_mask << shift, set_mask << shift);
}

static void peg_dmi_unset_and_set_mask_common(
	const bool is_peg,
	const uint32_t offset,
	const uint32_t unset,
	const uint32_t set,
	const uint32_t shift,
	const bool valid)
{
	const uint32_t unset_mask = unset << shift;
	const uint32_t   set_mask =   set << shift;
	if (is_peg) {
		for (uint8_t i = 0; i < MAX_PEG_FUNC; i++)
			pci_update_config32(PEG_DEV(i), offset, ~unset_mask, set_mask);
	} else {
		dmibar_clrsetbits32(offset, unset_mask, set_mask);
	}
}

static void peg_dmi_unset_and_set_mask_vcu_mmio(
	const uint32_t addr,
	const uint32_t unset_mask,
	const uint32_t set_mask,
	const uint32_t shift,
	const bool valid)
{
	if (!valid)
		return;

	vcu_update_mmio(addr, ~(unset_mask << shift), set_mask << shift);
}

#define BUNDLE_STEP	0x20

static void *const dmibar = (void *)(uintptr_t)CONFIG_FIXED_DMIBAR_MMIO_BASE;

void peg_dmi_recipe(const bool is_peg, const pci_devfn_t dev)
{
	const bool always = true;
	const bool is_dmi = !is_peg;

	/* Treat DMIBAR and PEG devices the same way */
	volatile union pci_bank *const bank = is_peg ? pci_map_bus(dev) : dmibar;

	const size_t bundles = (is_peg ? 8 : 2) * BUNDLE_STEP;

	for (size_t i = 0; i < bundles; i += BUNDLE_STEP) {
		/* These are actually per-lane */
		peg_dmi_unset_and_set_mask_pcicfg(bank, 0xa00 + i, 0x1f, 0x0c,  0, always);
		peg_dmi_unset_and_set_mask_pcicfg(bank, 0xa10 + i, 0x1f, 0x0c,  0, always);
	}

	for (size_t i = 0; i < bundles; i += BUNDLE_STEP)
		peg_dmi_unset_and_set_mask_pcicfg(bank, 0x904 + i, 0x1f, 0x02,  0, is_peg);

	for (size_t i = 0; i < bundles; i += BUNDLE_STEP)
		peg_dmi_unset_and_set_mask_pcicfg(bank, 0x904 + i, 0x1f, 0x03,  5, is_peg);

	for (size_t i = 0; i < bundles; i += BUNDLE_STEP)
		peg_dmi_unset_and_set_mask_pcicfg(bank, 0x90c + i, 0x3f, 0x09,  5, always);

	for (size_t i = 0; i < bundles; i += BUNDLE_STEP)
		peg_dmi_unset_and_set_mask_pcicfg(bank, 0x90c + i, 0x0f, 0x05, 21, is_peg);

	for (size_t i = 0; i < bundles; i += BUNDLE_STEP)
		peg_dmi_unset_and_set_mask_pcicfg(bank, 0x910 + i, 0x0f, 0x08,  6, is_peg);

	for (size_t i = 0; i < bundles; i += BUNDLE_STEP)
		peg_dmi_unset_and_set_mask_pcicfg(bank, 0x910 + i, 0x0f, 0x00, 10, always);

	for (size_t i = 0; i < bundles; i += BUNDLE_STEP)
		peg_dmi_unset_and_set_mask_pcicfg(bank, 0x910 + i, 0x07, 0x00, 18, always);

	peg_dmi_unset_and_set_mask_vcu_mmio(0x0c008001, 0x1f, 0x03, 25, is_peg);
	peg_dmi_unset_and_set_mask_vcu_mmio(0x0c0c8001, 0x3f, 0x00, 23, is_dmi);

	peg_dmi_unset_and_set_mask_pcicfg(bank, 0xc28, 0x1f, 0x13, 18, always);

	peg_dmi_unset_and_set_mask_common(is_peg, 0xc38, 0x01, 0x00,  6, always);
	peg_dmi_unset_and_set_mask_common(is_peg, 0x260, 0x03, 0x02,  0, always);

	for (size_t i = 0; i < bundles; i += BUNDLE_STEP)
		peg_dmi_unset_and_set_mask_pcicfg(bank, 0x900 + i, 0x03, 0x00, 26, always);

	for (size_t i = 0; i < bundles; i += BUNDLE_STEP)
		peg_dmi_unset_and_set_mask_pcicfg(bank, 0x904 + i, 0x03, 0x03, 10, always);

	for (size_t i = 0; i < bundles; i += BUNDLE_STEP)
		peg_dmi_unset_and_set_mask_pcicfg(bank, 0x90c + i, 0x1f, 0x07, 25, is_peg);

	for (size_t i = 0; i < bundles; i += BUNDLE_STEP)
		peg_dmi_unset_and_set_mask_pcicfg(bank, 0x91c + i, 0x07, 0x05, 27, is_peg);
}
