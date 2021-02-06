/* SPDX-License-Identifier: GPL-2.0-only */
#include <assert.h>
#include <console/console.h>
#include <fsp/util.h>
#include <soc/cnl_memcfg_init.h>
#include <spd_bin.h>
#include <string.h>

static void meminit_memcfg(FSP_M_CONFIG *mem_cfg,
			   const struct cnl_mb_cfg *board_cfg)
{
	/*
	 * DqByteMapChx expects 12 bytes of data, but the last 6 bytes
	 * are unused, so client passes in the relevant values and
	 * we null out the rest of the data.
	 */
	memset(&mem_cfg->DqByteMapCh0, 0, sizeof(mem_cfg->DqByteMapCh0));
	memcpy(&mem_cfg->DqByteMapCh0, &board_cfg->dq_map[DDR_CH0],
	       sizeof(board_cfg->dq_map[DDR_CH0]));

	memset(&mem_cfg->DqByteMapCh1, 0, sizeof(mem_cfg->DqByteMapCh1));
	memcpy(&mem_cfg->DqByteMapCh1, &board_cfg->dq_map[DDR_CH1],
	       sizeof(board_cfg->dq_map[DDR_CH1]));

	memcpy(&mem_cfg->DqsMapCpu2DramCh0, &board_cfg->dqs_map[DDR_CH0],
	       sizeof(board_cfg->dqs_map[DDR_CH0]));
	memcpy(&mem_cfg->DqsMapCpu2DramCh1, &board_cfg->dqs_map[DDR_CH1],
	       sizeof(board_cfg->dqs_map[DDR_CH1]));

	memcpy(&mem_cfg->RcompResistor, &board_cfg->rcomp_resistor,
	       sizeof(mem_cfg->RcompResistor));

	/* Early cannonlake requires rcomp targets to be 0 */
	memcpy(&mem_cfg->RcompTarget, &board_cfg->rcomp_targets,
	       sizeof(mem_cfg->RcompTarget));
}

/*
 * Initialize default memory settings using spd data contained in a buffer.
 */
static void meminit_spd_data(FSP_M_CONFIG *mem_cfg, uint8_t mem_slot,
			     size_t spd_data_len, uintptr_t spd_data_ptr)
{
	static size_t last_set_spd_data_len = 0;

	assert(spd_data_ptr != 0 && spd_data_len != 0);

	if (last_set_spd_data_len != 0 &&
	    last_set_spd_data_len != spd_data_len)
		die("spd data length disparity among slots");

	mem_cfg->MemorySpdDataLen = spd_data_len;
	last_set_spd_data_len = spd_data_len;

	switch (mem_slot) {
	case 0:
		mem_cfg->MemorySpdPtr00 = spd_data_ptr;
		break;
	case 1:
		mem_cfg->MemorySpdPtr01 = spd_data_ptr;
		break;
	case 2:
		mem_cfg->MemorySpdPtr10 = spd_data_ptr;
		break;
	case 3:
		mem_cfg->MemorySpdPtr11 = spd_data_ptr;
		break;
	default:
		die("nonexistent memory slot");
	}
	printk(BIOS_INFO, "memory slot: %d configuration done.\n", mem_slot);
}

/*
 * Initialize default memory settings using the spd file specified by
 * spd_index. The spd_index is an index into the SPD_SOURCES array defined
 * in spd/Makefile.inc.
 */
static void meminit_cbfs_spd_index(FSP_M_CONFIG *mem_cfg,
				   int spd_index, uint8_t mem_slot)
{
	static size_t spd_data_len;
	static uintptr_t spd_data_ptr;
	static int last_spd_index;

	assert(mem_slot < NUM_DIMM_SLOT);

	if ((spd_data_ptr == 0) || (last_spd_index != spd_index)) {
		printk(BIOS_DEBUG, "SPD INDEX = %d\n", spd_index);

		spd_data_ptr = spd_cbfs_map(spd_index);
		if (!spd_data_ptr)
			die("spd.bin not found or incorrect index\n");

		spd_data_len = CONFIG_DIMM_SPD_SIZE;

		/* Memory leak is ok since we have memory mapped boot media */
		assert(CONFIG(BOOT_DEVICE_MEMORY_MAPPED));

		last_spd_index = spd_index;
		print_spd_info((unsigned char *)spd_data_ptr);
	}

	meminit_spd_data(mem_cfg, mem_slot, spd_data_len, spd_data_ptr);
}

/* Initialize onboard memory configurations for CannonLake */
void cannonlake_memcfg_init(FSP_M_CONFIG *mem_cfg,
			    const struct cnl_mb_cfg *cnl_cfg)
{
	const struct spd_info *spdi;

	/* Early Command Training Enabled */
	mem_cfg->ECT = cnl_cfg->ect;
	mem_cfg->DqPinsInterleaved = cnl_cfg->dq_pins_interleaved;
	mem_cfg->CaVrefConfig = cnl_cfg->vref_ca_config;

	for (int i = 0; i < NUM_DIMM_SLOT; i++) {
		spdi = &(cnl_cfg->spd[i]);
		switch (spdi->read_type) {
		case NOT_EXISTING:
			break;
		case READ_SMBUS:
			mem_cfg->SpdAddressTable[i] =
				spdi->spd_spec.spd_smbus_address;
			break;
		case READ_SPD_CBFS:
			meminit_cbfs_spd_index(mem_cfg,
				spdi->spd_spec.spd_index, i);
			break;
		case READ_SPD_MEMPTR:
			meminit_spd_data(mem_cfg, i,
				spdi->spd_spec.spd_data_ptr_info.spd_data_len,
				spdi->spd_spec.spd_data_ptr_info.spd_data_ptr);
			break;
		default:
			die("no valid way to read mem info");
		}

		meminit_memcfg(mem_cfg, cnl_cfg);
	}
}
