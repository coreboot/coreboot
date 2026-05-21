/* SPDX-License-Identifier: GPL-2.0-only */

#include <arch/barrier.h>
#include <cbfs.h>
#include <console/console.h>
#include <device/mmio.h>
#include <program_loading.h>
#include <soc/adsp.h>
#include <types.h>

/**
 * lpass_program_boot_addr() - Program ADSP boot address override
 * @addr: ADSP firmware entry address
 *
 * Program EVB address/select so Q6 boots from @addr.
 */
static void lpass_program_boot_addr(uintptr_t addr)
{
	write32(&lpass_efuse_q6ss->evb_addr, (addr>>4));
	write32(&lpass_efuse_q6ss->evb_sel, EVB_ENABLE);
	dsb();
}

/**
 * lpass_dtb_bring_up() - Configure ADSP Device Tree Boot parameters
 * @dtb_entry_addr: Physical address where Device Tree image is loaded
 * @dtb_size: Size of Device Tree image in bytes
 *
 * Configure the QDSP6SS boot parameter registers with:
 * - Device Tree blob location and size
 * - Chip identification (family and ID)
 * - Chip version information
 * - Platform type and version
 *
 * Boot Parameter Register Layout:
 * BOOT_PARAMS[0]: DTB address (lower 32 bits)
 * BOOT_PARAMS[1]: DTB address (upper 32 bits)
 * BOOT_PARAMS[2]: Chip family (bits 31:16) | Chip ID (bits 15:0)
 * BOOT_PARAMS[3]: Chip version - Major (bits 15:8) | Minor (bits 7:0)
 * BOOT_PARAMS[4]: Platform type (bits 31:24) | Subtype (bits 23:16) |
 *                 Version major (bits 15:8) | Version minor (bits 7:0) not programmed
 * BOOT_PARAMS[5]: DTB size
 *
 * Return: CB_SUCCESS on success, CB_ERR on failure
 */
static enum cb_err lpass_dtb_bring_up(uintptr_t dtb_entry_addr, size_t dtb_size)
{
	uint32_t chip_family, chip_id;
	uint32_t chip_major, chip_minor;
	uint32_t boot_params2, boot_params3;

	write32(&lpass_qdsp6ss->boot_params[0], (uint32_t)(dtb_entry_addr & 0xFFFFFFFF));
	write32(&lpass_qdsp6ss->boot_params[1], (uint32_t)(dtb_entry_addr >> 32));

	/* x1p42100 uses fixed SoC boot args; do not depend on SMEM init ordering. */
	if (platform_get_soc_id() == SOC_ID_HAMOA) {
		chip_family = HAMOA_FAMILY & CHIP_FAMILY_MASK;
		chip_id = HAMOA_ID_SCP & CHIP_ID_MASK;
	} else {
		chip_family = X1P42100_FAMILY & CHIP_FAMILY_MASK;
		chip_id = X1P42100_ID_SCP & CHIP_ID_MASK;
	}
	boot_params2 = (chip_family << CHIP_FAMILY_SHIFT) | chip_id;

	chip_major = CHIPINFO_CHIP_VERSION_MAJOR;
	chip_minor = CHIPINFO_CHIP_VERSION_MINOR;
	boot_params3 = ((chip_major & CHIP_VERSION_MASK) << CHIP_VERSION_MAJOR_SHIFT) |
		       (chip_minor & CHIP_VERSION_MASK);

	write32(&lpass_qdsp6ss->boot_params[2], boot_params2);
	write32(&lpass_qdsp6ss->boot_params[3], boot_params3);
	write32(&lpass_qdsp6ss->boot_params[5], (uint32_t)dtb_size);
	dsb();

	return CB_SUCCESS;
}

/**
 * adsp_fw_load() - Main entry point for ADSP firmware loading
 *
 * Orchestrate the complete ADSP firmware loading sequence:
 * 1. Load ADSP Device Tree Blob from CBFS
 * 2. Configure QDSP6SS boot parameters with DTB location and platform info
 * 3. Load ADSP firmware from CBFS
 * 4. Program ADSP boot address via EFUSE EVB registers
 */
void adsp_fw_load(void)
{
	struct prog adsp_dtbs_prog = PROG_INIT(PROG_PAYLOAD, ADSP_CBFS_DTBS);
	struct prog adsp_fw_prog = PROG_INIT(PROG_PAYLOAD, ADSP_CBFS_FIRMWARE);
	uintptr_t dtb_entry_addr;
	uintptr_t fw_entry_addr;
	size_t dtb_size;

	dtb_size = cbfs_get_size(ADSP_CBFS_DTBS);
	if (dtb_size == 0) {
		printk(BIOS_ERR, "ADSP: Failed to get DTBS size from CBFS\n");
		return;
	}

	if (!selfload(&adsp_dtbs_prog)) {
		printk(BIOS_ERR, "ADSP: DTBS load failed\n");
		return;
	}

	dtb_entry_addr = (uintptr_t)prog_entry(&adsp_dtbs_prog);

	if (lpass_dtb_bring_up(dtb_entry_addr, dtb_size) != CB_SUCCESS) {
		printk(BIOS_ERR, "ADSP: DTB bring up failed\n");
		return;
	}

	if (!selfload(&adsp_fw_prog)) {
		printk(BIOS_ERR, "ADSP: Firmware load failed\n");
		return;
	}

	fw_entry_addr = (uintptr_t)prog_entry(&adsp_fw_prog);

	lpass_program_boot_addr(fw_entry_addr);

	printk(BIOS_INFO, "SOC: LPASS/ADSP image loaded successfully\n");
}
