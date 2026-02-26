/* SPDX-License-Identifier: GPL-2.0-only */

#include <amdblocks/psp.h>
#include <bootstate.h>
#include <cpu/x86/smm.h>
#include <smm_call.h>
#include "psp_rom_armor_apmc.h"

/*
 * Ramstage wrappers that trigger SMI to execute ROM Armor operations in SMM.
 * These functions communicate with SMM via CPU registers (call_smm)
 */

/*
 * Read data from the SPI flash via PSP ROM Armor.
 * ROM Armor 2 only.
 *
 * On ROM Armor 3, reads can be done directly through ROM2/ROM3 MMIO,
 * so this function is not used.
 */
static ssize_t rom_armor_ramstage_readat(const struct region_device *rd, void *buf,
					 size_t offset, size_t len)
{
	struct rom_armor_params_read params = {
		.buf = buf,
		.offset = offset,
		.size = len,
	};
	u32 ret;

	printk(BIOS_DEBUG, "PSP RomArmor (ramstage): Read offset=0x%zx, len=0x%zx\n",
	       offset, len);

	if (!buf) {
		printk(BIOS_ERR, "PSP RomArmor (ramstage): Invalid read parameters\n");
		return -1;
	}

	ret = call_smm(APM_CNT_ROM_ARMOR, ROM_ARMOR_APM_CMD_READ, &params);

	if (ret != ROM_ARMOR_RET_SUCCESS) {
		printk(BIOS_ERR, "PSP RomArmor (ramstage): Read failed, ret=%u\n", ret);
		return -1;
	}

	return len;
}

static ssize_t rom_armor_ramstage_writeat(const struct region_device *rd, const void *buf,
					  size_t offset, size_t len)
{
	struct rom_armor_params_write params = {
		.buf = buf,
		.offset = offset,
		.size = len,
	};
	u32 ret;

	printk(BIOS_DEBUG, "PSP ROM Armor (ramstage): Write offset=0x%zx, len=0x%zx\n",
	       offset, len);

	if (!buf || len == 0) {
		printk(BIOS_ERR, "PSP ROM Armor (ramstage): Invalid write parameters\n");
		return -1;
	}

	ret = call_smm(APM_CNT_ROM_ARMOR, ROM_ARMOR_APM_CMD_WRITE, &params);

	if (ret != ROM_ARMOR_RET_SUCCESS) {
		printk(BIOS_ERR, "PSP ROM Armor (ramstage): Write failed, ret=%u\n", ret);
		return -1;
	}

	return len;
}

static ssize_t rom_armor_ramstage_eraseat(const struct region_device *rd,
					   size_t offset, size_t len)
{
	struct rom_armor_params_erase params = {
		.offset = offset,
		.size = len,
	};
	u32 ret;

	printk(BIOS_DEBUG, "PSP ROM Armor (ramstage): Erase offset=0x%zx, len=0x%zx\n",
	       offset, len);

	ret = call_smm(APM_CNT_ROM_ARMOR, ROM_ARMOR_APM_CMD_ERASE, &params);

	if (ret != ROM_ARMOR_RET_SUCCESS) {
		printk(BIOS_ERR, "PSP ROM Armor (ramstage): Erase failed, ret=%u\n", ret);
		return -1;
	}

	return len;
}


/* Region device operations for ramstage - uses APM calls to SMM */
const struct region_device_ops rom_armor_apm_ops = {
	.mmap = NULL,
	.munmap = NULL,
	.readat = rom_armor_ramstage_readat,	/* Only used by ROM Armor 2 */
	.writeat = rom_armor_ramstage_writeat,
	.eraseat = rom_armor_ramstage_eraseat,
};

const struct region_device rom_armor_apm_call_rw =
	REGION_DEV_INIT(&rom_armor_apm_ops, 0, CONFIG_ROM_SIZE);

void psp_rom_armor_init(bool allow_capsule_update)
{
	enum rom_armor_apm_result ret;
	struct rom_armor_params_init params = {
		.capsule_update = allow_capsule_update,
	};
	ret = call_smm(APM_CNT_ROM_ARMOR, ROM_ARMOR_APM_CMD_INIT, &params);
	if (ret != ROM_ARMOR_RET_SUCCESS)
		printk(BIOS_EMERG, "Failed to initialize ROM Armor. ret=%u", ret);
}

static void rom_armor_finalize(void *unused)
{
	/*
	 * Lock down rom armor APM interface. Only SMMSTORE will be able to access
	 * the flash through APM interface from here on.
	 */
	call_smm(APM_CNT_ROM_ARMOR, ROM_ARMOR_APM_CMD_SHUTDOWN, NULL);
}

BOOT_STATE_INIT_ENTRY(BS_OS_RESUME, BS_ON_ENTRY, rom_armor_finalize, NULL);
BOOT_STATE_INIT_ENTRY(BS_PAYLOAD_LOAD, BS_ON_EXIT, rom_armor_finalize, NULL);
