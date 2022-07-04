/* SPDX-License-Identifier: GPL-2.0-only */

#include <console/console.h>
#include <gpio.h>
#include <option.h>
#include <soc/cnl_memcfg_init.h>
#include <soc/romstage.h>
#include <string.h>
#include <types.h>

static unsigned int get_memory_config_straps(void)
{
	/*
	 * The hardware supports a number of different memory configurations
	 * which are selected using four ID bits ID3 (GPP_H7), ID2 (GPP_H6),
	 * ID1 (GPP_E23) and ID0 (GPP_E22).
	 *
	 * The mapping is defined in the schematics as follows (ID3 is always
	 * 0 and can be ignored):
	 *
	 * ID2	ID1	ID0	Memory type
	 * -----------------------------------------------
	 * 0	0	0	Hynix 16G dual channel
	 * 0	0	1	Micron 16G dual channel
	 * 0	1	0	Hynix 8G dual channel
	 * 0	1	1	Hynix 4G single channel
	 * 1	0	0	Micron 8G dual channel
	 * 1	0	1	Micron 4G single channel
	 * 1	1	0	Samsung 8G dual channel
	 * 1	1	1	Samsung 4G single channel
	 *
	 * We return the value of these bits so that the index into the SPD
	 * table can be .spd[] values can be configured correctly in the
	 * memory configuration structure.
	 */

	gpio_t memid_gpios[] = {GPP_E22, GPP_E23, GPP_H6};
	return (u8)gpio_base2_value(memid_gpios, ARRAY_SIZE(memid_gpios));
}

static bool is_dual_channel(const unsigned int memid)
{
	return memid != 3 && memid != 5 && memid != 7;
}

static void fill_spd_data(struct cnl_mb_cfg *mem_cfg)
{
	const unsigned int memid = get_memory_config_straps();
	printk(BIOS_DEBUG, "Memory config straps: 0x%.2x\n", memid);
	/*
	 * If we are using single channel ID = 3, 5 or 7 then we only
	 * populate .spd[0].If we are dual channel then we also populate
	 * .spd[2] as well.
	 */
	mem_cfg->spd[0].read_type = READ_SPD_CBFS;
	mem_cfg->spd[0].spd_spec.spd_index = memid;
	if (is_dual_channel(memid)) {
		mem_cfg->spd[2].read_type = READ_SPD_CBFS;
		mem_cfg->spd[2].spd_spec.spd_index = memid;
	}
}

void mainboard_memory_init_params(FSPM_UPD *memupd)
{
	struct cnl_mb_cfg memcfg = {
		.rcomp_resistor = {121, 81, 100},
		.rcomp_targets = {100, 40, 20, 20, 26},
		.dq_pins_interleaved = 0,
		.vref_ca_config = 2,
		.ect = 0,
	};

	const uint8_t vtd = get_uint_option("vtd", 1);
	memupd->FspmTestConfig.VtdDisable = !vtd;

	fill_spd_data(&memcfg);
	cannonlake_memcfg_init(&memupd->FspmConfig, &memcfg);
}
