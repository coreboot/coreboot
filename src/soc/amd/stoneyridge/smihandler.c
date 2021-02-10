/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <arch/io.h>
#include <console/console.h>
#include <cpu/x86/smm.h>
#include <cpu/x86/cache.h>
#include <cpu/amd/amd64_save_state.h>
#include <acpi/acpi.h>
#include <arch/hlt.h>
#include <device/pci_def.h>
#include <smmstore.h>
#include <soc/smi.h>
#include <soc/southbridge.h>
#include <amdblocks/acpimmio.h>
#include <amdblocks/acpi.h>
#include <amdblocks/smi.h>
#include <amdblocks/smm.h>
#include <elog.h>

/* bits in smm_io_trap   */
#define SMM_IO_TRAP_PORT_OFFSET		16
#define SMM_IO_TRAP_PORT_ADDRESS_MASK	0xffff
#define SMM_IO_TRAP_RW			(1 << 0)
#define SMM_IO_TRAP_VALID		(1 << 1)

static inline u16 get_io_address(u32 info)
{
	return ((info >> SMM_IO_TRAP_PORT_OFFSET) &
		SMM_IO_TRAP_PORT_ADDRESS_MASK);
}

static void *find_save_state(int cmd)
{
	int core;
	amd64_smm_state_save_area_t *state;
	u32 smm_io_trap;
	u8 reg_al;

	/* Check all nodes looking for the one that issued the IO */
	for (core = 0; core < CONFIG_MAX_CPUS; core++) {
		state = smm_get_save_state(core);
		smm_io_trap = state->smm_io_trap_offset;
		/* Check for Valid IO Trap Word (bit1==1) */
		if (!(smm_io_trap & SMM_IO_TRAP_VALID))
			continue;
		/* Make sure it was a write (bit0==0) */
		if (smm_io_trap & SMM_IO_TRAP_RW)
			continue;
		/* Check for APMC IO port */
		if (pm_acpi_smi_cmd_port() != get_io_address(smm_io_trap))
			continue;
		/* Check AL against the requested command */
		reg_al = state->rax;
		if (reg_al == cmd)
			return state;
	}
	return NULL;
}

static void southbridge_smi_gsmi(void)
{
	u8 sub_command;
	amd64_smm_state_save_area_t *io_smi;
	u32 reg_ebx;

	io_smi = find_save_state(APM_CNT_ELOG_GSMI);
	if (!io_smi)
		return;
	/* Command and return value in EAX */
	sub_command = (io_smi->rax >> 8) & 0xff;

	/* Parameter buffer in EBX */
	reg_ebx = io_smi->rbx;

	/* drivers/elog/gsmi.c */
	io_smi->rax = gsmi_exec(sub_command, &reg_ebx);
}

static void southbridge_smi_store(void)
{
	u8 sub_command;
	amd64_smm_state_save_area_t *io_smi;
	u32 reg_ebx;

	io_smi = find_save_state(APM_CNT_SMMSTORE);
	if (!io_smi)
		return;
	/* Command and return value in EAX */
	sub_command = (io_smi->rax >> 8) & 0xff;

	/* Parameter buffer in EBX */
	reg_ebx = io_smi->rbx;

	/* drivers/smmstore/smi.c */
	io_smi->rax = smmstore_exec(sub_command, (void *)reg_ebx);
}

static void sb_apmc_smi_handler(void)
{
	const uint8_t cmd = inb(pm_acpi_smi_cmd_port());

	switch (cmd) {
	case APM_CNT_ACPI_ENABLE:
		acpi_enable_sci();
		break;
	case APM_CNT_ACPI_DISABLE:
		acpi_disable_sci();
		break;
	case APM_CNT_ELOG_GSMI:
		if (CONFIG(ELOG_GSMI))
			southbridge_smi_gsmi();
		break;
	case APM_CNT_SMMSTORE:
		if (CONFIG(SMMSTORE))
			southbridge_smi_store();
		break;
	}

	mainboard_smi_apmc(cmd);
}

static void sb_slp_typ_handler(void)
{
	uint32_t pci_ctrl, reg32;
	uint16_t pm1cnt, reg16;
	uint8_t slp_typ, rst_ctrl;

	/* Figure out SLP_TYP */
	pm1cnt = acpi_read16(MMIO_ACPI_PM1_CNT_BLK);
	printk(BIOS_SPEW, "SMI#: SLP = 0x%04x\n", pm1cnt);
	slp_typ = acpi_sleep_from_pm1(pm1cnt);

	/* Do any mainboard sleep handling */
	mainboard_smi_sleep(slp_typ);

	switch (slp_typ) {
	case ACPI_S0:
		printk(BIOS_DEBUG, "SMI#: Entering S0 (On)\n");
		break;
	case ACPI_S3:
		printk(BIOS_DEBUG, "SMI#: Entering S3 (Suspend-To-RAM)\n");
		break;
	case ACPI_S4:
		printk(BIOS_DEBUG, "SMI#: Entering S4 (Suspend-To-Disk)\n");
		break;
	case ACPI_S5:
		printk(BIOS_DEBUG, "SMI#: Entering S5 (Soft Power off)\n");
		break;
	default:
		printk(BIOS_DEBUG, "SMI#: ERROR: SLP_TYP reserved\n");
		break;
	}

	if (slp_typ >= ACPI_S3) {
		/* Sleep Type Elog S3, S4, and S5 entry */
		elog_gsmi_add_event_byte(ELOG_TYPE_ACPI_ENTER, slp_typ);

		wbinvd();

		clear_all_smi_status();

		/* Do not send SMI before AcpiPm1CntBlkx00[SlpTyp] */
		pci_ctrl = pm_read32(PM_PCI_CTRL);
		pci_ctrl &= ~FORCE_SLPSTATE_RETRY;
		pci_ctrl |= FORCE_STPCLK_RETRY;
		pm_write32(PM_PCI_CTRL, pci_ctrl);

		/* Enable SlpTyp */
		rst_ctrl = pm_read8(PM_RST_CTRL1);
		rst_ctrl |= SLPTYPE_CONTROL_EN;
		pm_write8(PM_RST_CTRL1, rst_ctrl);

		/*
		 * Before the final command, check if there's pending wake
		 * event. Read enable first, so that reading the actual status
		 * is as close as possible to entering S3. The idea is to
		 * minimize the opportunity for a wake event to happen before
		 * actually entering S3. If there's a pending wake event, log
		 * it and continue normal path. S3 will fail and the wake event
		 * becomes a SCI.
		 */
		if (CONFIG(ELOG_GSMI)) {
			reg16 = acpi_read16(MMIO_ACPI_PM1_EN);
			reg16 &= acpi_read16(MMIO_ACPI_PM1_STS);
			if (reg16)
				elog_add_extended_event(
						ELOG_SLEEP_PENDING_PM1_WAKE,
						(u32)reg16);

			reg32 = acpi_read32(MMIO_ACPI_GPE0_EN);
			reg32 &= acpi_read32(MMIO_ACPI_GPE0_STS);
			if (reg32)
				elog_add_extended_event(
						ELOG_SLEEP_PENDING_GPE0_WAKE,
						reg32);
		} /* if (CONFIG(ELOG_GSMI)) */

		/*
		 * An IO cycle is required to trigger the STPCLK/STPGNT
		 * handshake when the Pm1 write is reissued.
		 */
		outw(pm1cnt | SLP_EN, pm_read16(PM1_CNT_BLK));
		hlt();
	}
}

int southbridge_io_trap_handler(int smif)
{
	return 0;
}

/*
 * Table of functions supported in the SMI handler.  Note that SMI source setup
 * in southbridge.c is unrelated to this list.
 */
static const struct smi_sources_t smi_sources[] = {
	{ .type = SMITYPE_SMI_CMD_PORT, .handler = sb_apmc_smi_handler },
	{ .type = SMITYPE_SLP_TYP, .handler = sb_slp_typ_handler},
};

void *get_smi_source_handler(int source)
{
	int i;

	for (i = 0 ; i < ARRAY_SIZE(smi_sources) ; i++)
		if (smi_sources[i].type == source)
			return smi_sources[i].handler;

	return NULL;
}
