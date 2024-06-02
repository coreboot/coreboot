/* SPDX-License-Identifier: GPL-2.0-only */

#include <bootstate.h>
#include <console/console.h>
#include <cpu/cpu.h>
#include <device/mmio.h>
#include <device/pci_ops.h>
#include <fsp/util.h>
#include <intelblocks/systemagent.h>
#include <intelblocks/vtd.h>
#include <lib.h>
#include <soc/iomap.h>
#include <soc/pci_devs.h>

/* FSP 2.x VT-d HOB from edk2-platforms */
static const uint8_t vtd_pmr_info_data_hob_guid[16] = {
	0x45, 0x16, 0xb6, 0x6f, 0x68, 0xf1, 0xbe, 0x46,
	0x80, 0xec, 0xb5, 0x02, 0x38, 0x5e, 0xe7, 0xe7
};

struct vtd_pmr_info_hob {
	uint32_t protected_low_base;
	uint32_t protected_low_limit;
	uint64_t protected_high_base;
	uint64_t protected_high_limit;
} __packed;

static struct vtd_pmr_info_hob *pmr_hob;

static bool is_vtd_enabled(uintptr_t vtd_base)
{
	uint32_t version = vtd_read32(vtd_base, VER_REG);

	if (version == 0 || version == UINT32_MAX) {
		printk(BIOS_WARNING, "No VT-d @ 0x%08lx\n", vtd_base);
		return false;
	}

	printk(BIOS_DEBUG, "VT-d @ 0x%08lx, version %x.%x\n",
	       vtd_base, (version & 0xf0) >> 4, version & 0xf);

	return true;
}

static uint32_t vtd_get_pmr_alignment_lo(uintptr_t vtd_base)
{
	uint32_t value;

	vtd_write32(vtd_base, PLMLIMIT_REG, 0xffffffff);
	value = vtd_read32(vtd_base, PLMLIMIT_REG);
	value = ~value + 1;

	return value;
}

static uint64_t vtd_get_pmr_alignment_hi(uintptr_t vtd_base)
{
	uint64_t value;

	vtd_write64(vtd_base, PHMLIMIT_REG, 0xffffffffffffffffULL);
	value = vtd_read64(vtd_base, PHMLIMIT_REG);
	value = ~value + 1ULL;
	value = value & ((1ULL << (uint32_t)cpu_phys_address_size()) - 1ULL);

	/* The host address width can be different than the sizing of the register.
	 * Simply find the least significant bit set and use it as alignment;
	 */
	return __ffs64(value);
}

static void vtd_set_pmr_low(uintptr_t vtd_base)
{
	uint32_t pmr_lo_align;
	uint32_t pmr_lo_limit;
	/*
	 * Typical PMR alignment is 1MB so we should be good but check just in
	 * case.
	 */
	pmr_lo_align = vtd_get_pmr_alignment_lo(vtd_base);
	pmr_lo_limit = pmr_hob->protected_low_limit;

	if (!IS_ALIGNED(pmr_lo_limit, pmr_lo_align)) {
		pmr_lo_limit = ALIGN_DOWN(pmr_lo_limit, pmr_lo_align);
		printk(BIOS_WARNING, "PMR limit low not properly aligned, aligning down to %08x\n",
		       pmr_lo_limit);
	}

	printk(BIOS_INFO, "Setting DMA protection [0x0 - 0x%08x]\n", pmr_lo_limit);
	vtd_write32(vtd_base, PLMBASE_REG, 0);
	vtd_write32(vtd_base, PLMLIMIT_REG, pmr_lo_limit - 1);
}

static void vtd_set_pmr_high(uintptr_t vtd_base)
{
	uint64_t pmr_hi_align;
	uint64_t pmr_hi_limit;
	/*
	 * Typical PMR alignment is 1MB so we should be good with above 4G
	 * memory but check just in case.
	 */
	pmr_hi_align = vtd_get_pmr_alignment_hi(vtd_base);
	pmr_hi_limit = pmr_hob->protected_high_limit;

	/* No memory above 4G? Skip PMR high programming */
	if (pmr_hi_limit == 0 || pmr_hi_limit < 4ULL * GiB)
		return;

	if (!IS_ALIGNED(pmr_hi_limit, pmr_hi_align)) {
		pmr_hi_limit = ALIGN_DOWN(pmr_hi_limit, pmr_hi_align);
		printk(BIOS_WARNING, "PMR High limit not properly aligned, "
				     "aligning down to %016llx\n",
				     pmr_hi_limit);
	}

	printk(BIOS_INFO, "Setting DMA protection [0x100000000 - 0x%016llx]\n", pmr_hi_limit);
	vtd_write64(vtd_base, PHMBASE_REG, 4ULL * GiB);
	vtd_write64(vtd_base, PHMLIMIT_REG, pmr_hi_limit - 1ULL);
}

static bool disable_pmr_protection(uintptr_t vtd_base)
{
	if (vtd_read32(vtd_base, PMEN_REG) & PMEN_PRS) {
		vtd_write32(vtd_base, PMEN_REG, vtd_read32(vtd_base, PMEN_REG) & ~PMEN_EPM);
		if (vtd_read32(vtd_base, PMEN_REG) & PMEN_PRS) {
			printk(BIOS_ERR, "Failed to disable existing DMA protection\n");
			return false;
		}
	}

	return true;
}

static bool enable_pmr_protection(uintptr_t vtd_base)
{
	vtd_write32(vtd_base, PMEN_REG, vtd_read32(vtd_base, PMEN_REG) | PMEN_EPM);
	if (vtd_read32(vtd_base, PMEN_REG) & PMEN_PRS)
		return true;

	return false;
}

static const void *locate_pmr_info_hob(void)
{
	size_t size;
	const void *hob;

	if (pmr_hob)
		return (void *)pmr_hob;

	hob = fsp_find_extension_hob_by_guid(vtd_pmr_info_data_hob_guid, &size);

	if (hob) {
		pmr_hob = (struct vtd_pmr_info_hob *)hob;
		printk(BIOS_SPEW, "PMR info HOB:\n"
				  "  protected_low_base: %08x\n"
				  "  protected_low_limit: %08x\n"
				  "  protected_high_base: %016llx\n"
				  "  protected_high_limit: %016llx\n",
				  pmr_hob->protected_low_base, pmr_hob->protected_low_limit,
				  pmr_hob->protected_high_base, pmr_hob->protected_high_limit);
	}

	return hob;
}

static void vtd_engine_enable_dma_protection(uintptr_t vtd_base)
{
	if (!is_vtd_enabled(vtd_base)) {
		printk(BIOS_ERR, "Not enabling DMA protection, VT-d not found\n");
		return;
	}

	/* At minimum PMR Low must be supported, coreboot executes in 32bit space (for now) */
	if (!(vtd_read32(vtd_base, CAP_REG) & CAP_PMR_LO)) {
		printk(BIOS_ERR, "Not enabling DMA protection, PMR registers not supported\n");
		return;
	}

	if (!locate_pmr_info_hob()) {
		printk(BIOS_ERR, "VT-d PMR HOB not found, not enabling DMA protection\n");
		return;
	}

	/* If protection is enabled, disable it first */
	if (!disable_pmr_protection(vtd_base)) {
		printk(BIOS_ERR, "Not setting DMA protection\n");
		return;
	}

	vtd_set_pmr_low(vtd_base);

	if (vtd_read32(vtd_base, CAP_REG) & CAP_PMR_HI)
		vtd_set_pmr_high(vtd_base);

	if (enable_pmr_protection(vtd_base))
		printk(BIOS_INFO, "Successfully enabled VT-d PMR DMA protection\n");
	else
		printk(BIOS_ERR, "Enabling VT-d PMR DMA protection failed\n");
}

static const struct hob_resource *find_resource_hob_by_addr(const uint64_t addr)
{
	const struct hob_header *hob_iterator;
	const struct hob_resource *res;

	if (fsp_hob_iterator_init(&hob_iterator) != CB_SUCCESS) {
		printk(BIOS_ERR, "Failed to find HOB list\n");
		return NULL;
	}

	while (fsp_hob_iterator_get_next_resource(&hob_iterator, &res) == CB_SUCCESS) {
		if ((res->type == EFI_RESOURCE_MEMORY_RESERVED) && (res->addr == addr))
			return res;
	}

	return NULL;
}

void *vtd_get_dma_buffer(size_t *size)
{
	const struct hob_resource *res;

	if (!CONFIG(ENABLE_EARLY_DMA_PROTECTION))
		goto no_dma_buffer;

	if (!locate_pmr_info_hob()) {
		printk(BIOS_ERR, "FSP PMR info HOB not found\n");
		goto no_dma_buffer;
	}

	/* PMR low limit will be the DMA buffer base reserved by FSP */
	res = find_resource_hob_by_addr((uint64_t)pmr_hob->protected_low_limit);
	if (!res) {
		printk(BIOS_ERR, "FSP PMR resource HOB not found\n");
		goto no_dma_buffer;
	}

	if (size)
		*size = res->length;

	return (void *)(uintptr_t)res->addr;

no_dma_buffer:
	if (size)
		*size = 0;
	return NULL;
}

void vtd_enable_dma_protection(void)
{
	if (!CONFIG(ENABLE_EARLY_DMA_PROTECTION))
		return;

	vtd_engine_enable_dma_protection(VTVC0_BASE_ADDRESS);
	/*
	 * FIXME: GFX VT-d will fail to set PMR (tested on ADL-S).
	 * Should we program PMRs on all VT-d engines?
	 * vtd_engine_enable_dma_protection(GFXVT_BASE_ADDRESS);
	 * vtd_engine_enable_dma_protection(IPUVT_BASE_ADDRESS);
	 */
}

static void vtd_disable_pmr_on_resume(void *unused)
{
	/* At minimum PMR Low must be supported */
	if (!(vtd_read32(VTVC0_BASE_ADDRESS, CAP_REG) & CAP_PMR_LO))
		return;

	if (disable_pmr_protection(VTVC0_BASE_ADDRESS)) {
		vtd_write32(VTVC0_BASE_ADDRESS, PLMBASE_REG, 0);
		vtd_write32(VTVC0_BASE_ADDRESS, PLMLIMIT_REG, 0);
		if (vtd_read32(VTVC0_BASE_ADDRESS, CAP_REG) & CAP_PMR_HI) {
			vtd_write64(VTVC0_BASE_ADDRESS, PHMBASE_REG, 0);
			vtd_write64(VTVC0_BASE_ADDRESS, PHMLIMIT_REG, 0);
		}
	}
}

BOOT_STATE_INIT_ENTRY(BS_OS_RESUME, BS_ON_ENTRY, vtd_disable_pmr_on_resume, NULL);
