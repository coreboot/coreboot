/* SPDX-License-Identifier: GPL-2.0-only */

#include <cbfs.h>
#include <console/console.h>
#include <fsp/util.h>
#include <soc/pci_devs.h>
#include <soc/pm.h>
#include <soc/romstage.h>
#include <soc/reg_access.h>

static const struct reg_script clear_smi_and_wake_events_script[] = {
	/* Clear any SMI or wake events */
	REG_GPE0_READ(R_QNC_GPE0BLK_GPE0S),
	REG_GPE0_READ(R_QNC_GPE0BLK_SMIS),
	REG_GPE0_OR(R_QNC_GPE0BLK_GPE0S, B_QNC_GPE0BLK_GPE0S_ALL),
	REG_GPE0_OR(R_QNC_GPE0BLK_SMIS, B_QNC_GPE0BLK_SMIS_ALL),
	REG_SCRIPT_END
};

void clear_smi_and_wake_events(void)
{
	struct chipset_power_state *ps;

	/* Clear SMI and wake events */
	ps = get_power_state();
	if (ps->prev_sleep_state != 3) {
		printk(BIOS_SPEW, "Clearing SMI interrupts and wake events\n");
		reg_script_run_on_dev(LPC_BDF,
			clear_smi_and_wake_events_script);
	}
}

void disable_rom_shadow(void)
{
	uint32_t data;

	/* Determine if the shadow ROM is enabled */
	data = port_reg_read(QUARK_NC_HOST_BRIDGE_SB_PORT_ID,
				QNC_MSG_FSBIC_REG_HMISC);
	if ((data & (ESEG_RD_DRAM | FSEG_RD_DRAM))
		!= (ESEG_RD_DRAM | FSEG_RD_DRAM)) {

		/* Disable the ROM shadow 0x000e0000 - 0x000fffff */
		data |= ESEG_RD_DRAM | FSEG_RD_DRAM;
		port_reg_write(QUARK_NC_HOST_BRIDGE_SB_PORT_ID,
			QNC_MSG_FSBIC_REG_HMISC, data);
	}
}

void *locate_rmu_file(size_t *rmu_file_len)
{
	size_t fsize;
	void *rmu_data;

	/* Locate the rmu.bin file in the read-only region of the flash */
	rmu_data = cbfs_ro_map("rmu.bin", &fsize);
	if (!rmu_data)
		return NULL;

	if (rmu_file_len != NULL)
		*rmu_file_len = fsize;

	return rmu_data;
}
