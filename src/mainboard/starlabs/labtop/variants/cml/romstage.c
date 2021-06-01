/* SPDX-License-Identifier: GPL-2.0-only */

#include <option.h>
#include <soc/cnl_memcfg_init.h>
#include <soc/romstage.h>
#include <console/console.h>
#include <gpio.h>
#include <string.h>
#include <types.h>

#include "baseboard/memory.h"

u8 get_memory_config_straps(void)
{
	/*
	 * The hardware supports a number of different memory configurations
	 * which are selected using four ID bits ID3 (GPP_H7), ID2 (GPP_H6),
	 * ID1 (GPP_E23) and ID0 (GPP_E22).
	 *
	 * The mapping is defined in the schematics as follows ID3 is always
	 * 0 and can be ignored):
	 *
	 * ID2ID1ID0Memory type
	 * --------------------
	 * 1  1  1 Samsung 4G single channel
	 * 1  1  0 Samsung 8G dual channel
	 * 1  0  1 Micron 4G single channel
	 * 1  0  0 Micron 8G dual channel
	 * 0  1  1 Hynix 4G single channel
	 * 0  1  0 Hynix 8G dual channel
	 * 0  0  1 Micron 16G dual channel
	 * 0  0  0 Hynix 16G dual channel
	 *
	 * We return the value of these bits so that the index into the SPD
	 * table can be .spd[] values can be configured correctly in the
	 * memory configuration structure.
	 */

	gpio_t memid_gpios[] = {
		GPP_E22,
		GPP_E23,
		GPP_H6
	};
	return (u8)gpio_base2_value(memid_gpios, ARRAY_SIZE(memid_gpios));
}

const struct cnl_mb_cfg *get_memory_cfg(struct cnl_mb_cfg *mem_cfg)
{
	u8 memid;

	struct cnl_mb_cfg std_memcfg = {
		/*
		* The dqs_map arrays map the DDR4 pins to the SoC pins
		* for both channels.
		*
		* the index = pin number on DDR4 part
		* the value = pin number on SoC
		*/
		.dqs_map[DDR_CH0] = {0, 6, 1, 3, 5, 2, 7, 4},
		.dqs_map[DDR_CH1] = {7, 5, 3, 6, 2, 4, 0, 1},

		/*
		* Mainboard uses 121, 81 and 100 rcomp resistors. See R6E1, R6E2
		* and R6E3 on page 6 of the schematics.
		*/
		.rcomp_resistor = {121, 81, 100},

		/*
		* Mainboard Rcomp target values.
		*/
		.rcomp_targets = {100, 40, 20, 20, 26},

		/*
		* Mainboard is a non-interleaved design - see pages 5 & 6
		* of the schematics.
		*/
		.dq_pins_interleaved = 0,

		/*
		* Mainboard is using DDR_VREF_CA for CH_A and DDR1_VREF_DQ for
		* CH_B - see page 5 of the schematics.
		*/
		.vref_ca_config = 2,

		/* Disable Early Command Training */
		.ect = 0,
	};

	memcpy(mem_cfg, &std_memcfg, sizeof(std_memcfg));

	memid = get_memory_config_straps();
	printk(BIOS_DEBUG, "Memory config straps: 0x%.2x\n", memid);

	/*
	 * If we are using single channel ID = 3, 5 or 7 then we only
	 * populate .spd[0].If we are dual channel then we also populate
	 * .spd[2] as well.
	 */
	mem_cfg->spd[0].read_type = READ_SPD_CBFS;
	mem_cfg->spd[0].spd_spec.spd_index = memid;
	if (memid != 3 && memid != 5 && memid != 7) {
		mem_cfg->spd[2].read_type = READ_SPD_CBFS;
		mem_cfg->spd[2].spd_spec.spd_index = memid;
	}

	return mem_cfg;
};

void mainboard_memory_init_params(FSPM_UPD *memupd)
{
	struct cnl_mb_cfg board_memcfg;

	const uint8_t vtd = get_uint_option("vtd", 1);
	memupd->FspmTestConfig.VtdDisable = !vtd;
	const uint8_t ht = get_uint_option("hyper_threading", memupd->FspmConfig.HyperThreading);
	memupd->FspmConfig.HyperThreading = ht;

	cannonlake_memcfg_init(&memupd->FspmConfig, get_memory_cfg(&board_memcfg));
}
