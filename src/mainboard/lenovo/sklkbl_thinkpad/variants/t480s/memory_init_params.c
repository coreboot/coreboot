/* SPDX-License-Identifier: GPL-2.0-only */

#include <cbfs.h>
#include <gpio.h>
#include <soc/gpio.h>
#include <soc/romstage.h>
#include <spd_bin.h>
#include <stdio.h>
#include "../../variant.h"

static const struct pad_config memory_id_gpio_table[] = {
       PAD_CFG_GPI_TRIG_OWN(GPP_F16, UP_20K, DEEP, OFF, ACPI),         /* MEMORYID0 */
       PAD_CFG_GPI_TRIG_OWN(GPP_F17, UP_20K, DEEP, OFF, ACPI),         /* MEMORYID1 */
       PAD_CFG_GPI_TRIG_OWN(GPP_F18, UP_20K, DEEP, OFF, ACPI),         /* MEMORYID2 */
       PAD_CFG_GPI_TRIG_OWN(GPP_F19, UP_20K, DEEP, OFF, ACPI),         /* MEMORYID3 */
       PAD_CFG_GPI_TRIG_OWN(GPP_F20, UP_20K, DEEP, OFF, ACPI),         /* MEMORYID4 */
};

int variant_memory_sku(void)
{
        gpio_t spd_gpios[] = {
                GPP_F16,
                GPP_F17,
                GPP_F18,
                GPP_F19,
                GPP_F20,
        };

        return gpio_base2_value(spd_gpios, ARRAY_SIZE(spd_gpios));
}

void mainboard_memory_init_params(FSPM_UPD *mupd)
{
	int spd_idx;
	char spd_name[20];
	size_t spd_size;

	FSP_M_CONFIG *mem_cfg = &mupd->FspmConfig;
	mem_cfg->DqPinsInterleaved = true;			/* DDR_DQ in interleave mode */
	mem_cfg->CaVrefConfig      = 2;				/* VREF_CA to CH_A and VREF_DQ_B to CH_B */
	mem_cfg->MemorySpdDataLen  = CONFIG_DIMM_SPD_SIZE;

	/* Get SPD for soldered RAM SPD (CH A) */
	gpio_configure_pads(memory_id_gpio_table, ARRAY_SIZE(memory_id_gpio_table));

	spd_idx = variant_memory_sku();
	printk(BIOS_DEBUG, "Detected MEMORY_ID = %d\n", spd_idx);
	snprintf(spd_name, sizeof(spd_name), "spd_%d.bin", spd_idx);
	mem_cfg->MemorySpdPtr00    = (uintptr_t)cbfs_map(spd_name, &spd_size);

	/* Get SPD for memory slot (CH B) */
	struct spd_block blk = { .addr_map = { [1] = 0x51, } };
	get_spd_smbus(&blk);
	dump_spd_info(&blk);

	mem_cfg->MemorySpdPtr10    = (uintptr_t)blk.spd_array[1];
}
