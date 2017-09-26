/*
 * SMI handler for Hudson southbridges
 *
 * Copyright (C) 2014 Alexandru Gagniuc <mr.nuke.me@gmail.com>
 * Subject to the GNU GPL v2, or (at your option) any later version.
 */

#include <console/console.h>
#include <cpu/x86/smm.h>
#include <delay.h>
#include <soc/smi.h>
#include <soc/southbridge.h>

static void sb_apmc_smi_handler(void)
{
	u32 reg32;
	const uint8_t cmd = inb(pm_acpi_smi_cmd_port());

	switch (cmd) {
	case APM_CNT_ACPI_ENABLE:
		reg32 = inl(ACPI_PM1_CNT_BLK);
		reg32 |= (1 << 0);	/* SCI_EN */
		outl(reg32, ACPI_PM1_CNT_BLK);
		break;
	case APM_CNT_ACPI_DISABLE:
		reg32 = inl(ACPI_PM1_CNT_BLK);
		reg32 &= ~(1 << 0);	/* clear SCI_EN */
		outl(ACPI_PM1_CNT_BLK, reg32);
		break;
	}

	mainboard_smi_apmc(cmd);
}

int southbridge_io_trap_handler(int smif)
{
	return 0;
}

static void process_smi_sci(void)
{
	const uint32_t status = smi_read32(SMI_SCI_STATUS);

	/* Clear events to prevent re-entering SMI if event isn't handled */
	smi_write32(SMI_SCI_STATUS, status);
}

static void process_gpe_smi(void)
{
	const uint32_t status = smi_read32(SMI_REG_SMISTS0);

	/* Only Bits [23:0] indicate GEVENT SMIs. */
	if (status & GEVENT_MASK) {
		/* A GEVENT SMI occurred */
		mainboard_smi_gpi(status & GEVENT_MASK);
	}

	/* Clear events to prevent re-entering SMI if event isn't handled */
	smi_write32(SMI_REG_SMISTS0, status);
}

static void process_smi_0x84(void)
{
	const uint32_t status = smi_read32(SMI_REG_SMISTS1);

	/* Clear events to prevent re-entering SMI if event isn't handled */
	smi_write32(SMI_REG_SMISTS1, status);
}

static void process_smi_0x88(void)
{
	const uint32_t status = smi_read32(SMI_REG_SMISTS2);

	if (status & TYPE_TO_MASK(SMITYPE_SMI_CMD_PORT)) {
		/* Command received via ACPI SMI command port */
		sb_apmc_smi_handler();
	}
	/* Clear events to prevent re-entering SMI if event isn't handled */
	smi_write32(SMI_REG_SMISTS2, status);
}

static void process_smi_0x8c(void)
{
	const uint32_t status = smi_read32(SMI_REG_SMISTS3);

	/* Clear events to prevent re-entering SMI if event isn't handled */
	smi_write32(SMI_REG_SMISTS4, status);
}

static void process_smi_0x90(void)
{
	const uint32_t status = smi_read32(SMI_REG_SMISTS4);

	/* Clear events to prevent re-entering SMI if event isn't handled */
	smi_write32(SMI_REG_SMISTS4, status);
}

void southbridge_smi_handler(void)
{
	const uint16_t smi_src = smi_read16(SMI_REG_POINTER);

	if (smi_src & SMI_STATUS_SRC_SCI)
		process_smi_sci();
	if (smi_src & SMI_STATUS_SRC_0)
		process_gpe_smi();
	if (smi_src & SMI_STATUS_SRC_1)
		process_smi_0x84();
	if (smi_src & SMI_STATUS_SRC_2)
		process_smi_0x88();
	if (smi_src & SMI_STATUS_SRC_3)
		process_smi_0x8c();
	if (smi_src & SMI_STATUS_SRC_4)
		process_smi_0x90();
}

void southbridge_smi_set_eos(void)
{
	uint32_t reg = smi_read32(SMI_REG_SMITRIG0);
	reg |= SMITRG0_EOS;
	smi_write32(SMI_REG_SMITRIG0, reg);
}
