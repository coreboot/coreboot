/* SPDX-License-Identifier: GPL-2.0-only */
/*
 * Intel GM965 (Crestline) Northbridge - Romstage Entry Point
 *
 * Modeled on coreboot GM45 romstage.c, adapted for GM965 + ICH8-M.
 * Orchestrates early initialization, RAM init, DMI/PCIe link setup,
 * and handoff to ramstage.
 */

#include <cbmem.h>
#include <romstage_handoff.h>
#include <console/console.h>
#include <device/pci_ops.h>
#include <device/smbus_host.h>
#include <arch/romstage.h>
#include <northbridge/intel/gm965/gm965.h>
#include <southbridge/intel/i82801hx/i82801hx.h>
#include <southbridge/intel/common/pmclib.h>
#include <southbridge/intel/common/pmutil.h>
#include <string.h>
#include <lib.h>
#include <commonlib/helpers.h>

#define LPC_DEV PCI_DEV(0, 0x1f, 0)

#define GM965_RAM_DEBUG 0

/*
 * Default SPD address map: one DIMM per channel, standard SA[2:0] strapping.
 * Slot 0 = channel 0 DIMM 0, slot 2 = channel 1 DIMM 0.
 * Boards with a non-standard wiring override this in their early_init.c.
 */
void __weak mainboard_get_spd_map(u8 spd_map[4])
{
	spd_map[0] = 0x50;
	spd_map[1] = 0x00;
	spd_map[2] = 0x51;
	spd_map[3] = 0x00;
}

void __weak mb_pre_raminit_setup(sysinfo_t *sysinfo)
{
}

void __weak mb_post_raminit_setup(void)
{
}

void __weak mainboard_late_rcba_config(void)
{
}

/* Platform has no romstage entry point under mainboard directory,
 * so this one is named with prefix mainboard.
 */
void mainboard_romstage_entry(void)
{
	sysinfo_t sysinfo;
	int s3resume;
	int cbmem_initted;

	/* Basic northbridge setup */
	gm965_early_init();

	/* First, run everything needed for console output. */
	enable_smbus();

	/* ASPM related setting, set early by original BIOS. */
	dmibar_clrbits16(0x204, 3 << 10);

	/* Check for S3 resume. */
	s3resume = southbridge_detect_s3_resume();

	/* RAM initialization */
	memset(&sysinfo, 0, sizeof(sysinfo));

	mb_pre_raminit_setup(&sysinfo);

	raminit(&sysinfo);

	/* DMI link init: SB side first, then NB side, then poll.
	 * Vendor order: ich8m_dmi_setup -> gm965_dmi_init -> ich8m_dmi_poll_vc1
	 * The NB side waits for VC1 negotiation which requires SB VC1 enabled. */
	printk(BIOS_DEBUG, "romstage: i82801hx_dmi_setup\n");
	i82801hx_dmi_setup();
	printk(BIOS_DEBUG, "romstage: gm965_dmi_init\n");
	gm965_dmi_init();
	printk(BIOS_DEBUG, "romstage: i82801hx_dmi_poll_vc1\n");
	i82801hx_dmi_poll_vc1();
	/*
	 * PM init: programs MCH power management registers.
	 * Vendor BIOS: bioscode_5.rom, called after DMI/PCIe init.
	 * Without this, the memory controller has no proper power state
	 * management, causing intermittent data corruption under load.
	 */
	printk(BIOS_DEBUG, "romstage: gm965_pm_init\n");
	gm965_pm_init(&sysinfo);

	/*
	 * IGD init: programs HSync/VSync, PM_F10 bit 0, IGD scheduling.
	 * Vendor BIOS: bioscode_7.rom, called after PM init.
	 */
	printk(BIOS_DEBUG, "romstage: gm965_igd_init\n");
	gm965_igd_init(&sysinfo);

	printk(BIOS_DEBUG, "romstage: write SSKPD=0xcafe\n");
	mchbar_write16(SSKPD_MCHBAR, 0xcafe);

	if (GM965_RAM_DEBUG) {
		/* Full MCHBAR dump - all ranges raminit touches, skip zeros */
		for (uint16_t off = 0; off < 0x2000; off += 4) {
			uint32_t val = mchbar_read32(off);
			if (val)
				printk(BIOS_DEBUG, "MCHBAR 0x%04x: 0x%08x\n", off, val);
		}

		/* Memory verification: test each rank to confirm training worked. */
		{
			printk(BIOS_DEBUG, "romstage: per-rank memory verification\n");
			/* 8 rank slots: ch0 r0/r1/r2/r3, ch1 r0/r1/r2/r3 */
			static const uint16_t drb_offsets[] = {
				0x1200, 0x1200, 0x1204, 0x1204,  /* ch0 */
				0x1300, 0x1300, 0x1304, 0x1304,  /* ch1 */
			};
			uint32_t prev_bnd = 0;
			for (int rank = 0; rank < 8; rank++) {
				uint32_t reg = mchbar_read32(drb_offsets[rank]);
				uint32_t bnd = (rank & 1)
					? (reg >> 16) & 0xff
					: reg & 0xff;
				if (bnd == 0 || bnd == prev_bnd) {
					prev_bnd = bnd;
					continue;
				}
				uint32_t rank_start = prev_bnd << 25;
				uint32_t test_addr = rank_start + 1 * MiB;
				uint32_t tolud = (pci_read_config16(D0F0, D0F0_TOLUD)
						  & 0xfff0) << 16;
				if (test_addr >= tolud) {
					prev_bnd = bnd;
					continue;
				}
				printk(BIOS_DEBUG, "  rank %d: test at 0x%08x\n",
				       rank, test_addr);
				ram_check(test_addr);
				prev_bnd = bnd;
			}
		}
	}

	printk(BIOS_DEBUG, "romstage: cbmem_top=0x%08x\n",
	       (unsigned int)cbmem_top());

	printk(BIOS_DEBUG, "romstage: cbmem_recovery\n");
	cbmem_initted = !cbmem_recovery(s3resume);
	printk(BIOS_DEBUG, "romstage: cbmem_initted=%d\n", cbmem_initted);

	setup_sdram_meminfo(&sysinfo);

	mb_post_raminit_setup();

	southbridge_configure_default_intmap();
	mainboard_late_rcba_config();

	printk(BIOS_DEBUG, "romstage: handoff s3resume=%d\n", s3resume);
	romstage_handoff_init(cbmem_initted && s3resume);
	printk(BIOS_SPEW, "exit main()\n");
}
