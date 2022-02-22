/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <acpi/acpigen.h>
#include <assert.h>
#include <console/console.h>
#include <intelblocks/acpi.h>
#include <intelblocks/pmc_ipc.h>
#include <stdlib.h>
#include <types.h>

#define LPI_S0_HELPER_UUID		"c4eb40a0-6cd2-11e2-bcfd-0800200c9a66"
#define PEP_S0IX_UUID			"57a6512e-3979-4e9d-9708-ff13b2508972"
#define SYSTEM_POWER_MANAGEMENT_HID	"INT33A1"
#define SYSTEM_POWER_MANAGEMENT_CID	"PNP0D80"
#define EC_S0IX_HOOK			"\\_SB.PCI0.LPCB.EC0.S0IX"
#define MAINBOARD_HOOK			"\\_SB.MS0X"
#define MAINBOARD_DISPLAY_HOOK		"\\_SB.MDSX"
#define ENABLE_PM_BITS_HOOK		"\\_SB.PCI0.EGPM"
#define RESTORE_PM_BITS_HOOK		"\\_SB.PCI0.RGPM"
#define LPI_STATES_ALL			0xff
#define MIN_DEVICE_STATE		ACPI_DEVICE_SLEEP_D0
#define PEPD_SCOPE			"\\_SB.PCI0"

struct reg_info {
	uint8_t *addr;
	size_t buffer_size;
};

static void read_pmc_lpm_requirements(const struct soc_pmc_lpm *lpm,
				      struct reg_info *info)
{
	assert(info);

	if (!CONFIG(SOC_INTEL_COMMON_BLOCK_ACPI_PEP_LPM_REQ) || !lpm) {
		memset(info, 0, sizeof(*info));
		return;
	}

	const size_t register_count = lpm->num_substates * lpm->num_req_regs;
	uint32_t *reg = calloc(register_count, sizeof(uint32_t));

	/* Read the various LPM state requirement registers from the PMC */
	for (size_t i = 0; i < lpm->num_substates; i++) {
		if (!(lpm->lpm_enable_mask & BIT(i)))
			continue;

		for (size_t j = 0; j < lpm->num_req_regs; j++) {
			const uint32_t offset = lpm->lpm_ipc_offset +
				i * lpm->req_reg_stride +
				j * sizeof(uint32_t);
			const uint32_t cmd_reg = pmc_make_ipc_cmd(PMC_IPC_CMD_RD_PMC_REG,
							PMC_IPC_CMD_SUBCMD_RD_PMC_REG, 0);
			struct pmc_ipc_buffer req = {.buf[0] = offset};
			struct pmc_ipc_buffer res = {};

			enum cb_err result = pmc_send_ipc_cmd(cmd_reg, &req, &res);
			if (result != CB_SUCCESS) {
				printk(BIOS_ERR, "Failed to retrieve LPM substate registers"
				       "from LPM, substate %zu, reg %zu\n", i, j);
			}

			uint32_t *ptr = reg + i * lpm->num_req_regs + j;
			*ptr = res.buf[0];
		}
	}

	info->addr = (uint8_t *)reg;
	info->buffer_size = register_count * sizeof(uint32_t);
}

/*
 * Windows expects a non-empty package for this subfunction, otherwise it
 * results in a bluescreen (`INTERNAL_POWER_ERROR`); returning an empty package
 * does not work. To workaround this, return a package describing a single
 * device, one that is known to exist, i.e.  ACPI_CPU_STRING.  expects at least
 * one device and crashes without it with a bluescreen.
 */
static void lpi_get_constraints(void *unused)
{
	char path[16];

	/*
	 * Return (Package() {
	 *     Package() { "\_SB.CP00", 0,
	 *         Package() { 0,
	 *             Package() { 0xff, 0 }}}})
	 */
	acpigen_emit_byte(RETURN_OP);
	acpigen_write_package(1);
	{
		acpigen_write_package(3);
		{
			snprintf(path, sizeof(path), CONFIG_ACPI_CPU_STRING, 0);
			acpigen_emit_namestring(path);
			acpigen_write_integer(0); /* device disabled */
			acpigen_write_package(2);
			{
				acpigen_write_integer(0); /* revision */
				acpigen_write_package(2);
				{
					acpigen_write_integer(LPI_STATES_ALL);
					acpigen_write_integer(MIN_DEVICE_STATE);
				}
				acpigen_write_package_end();
			}
			acpigen_write_package_end();
		}
		acpigen_write_package_end();
	}
	acpigen_write_package_end();
}

static void lpi_s0ix_entry(void *unused)
{
	/* Inform the EC */
	acpigen_write_if_cond_ref_of(EC_S0IX_HOOK);
	acpigen_emit_namestring(EC_S0IX_HOOK);
	acpigen_write_integer(1);
	acpigen_write_if_end();

	/* Provide a board level S0ix hook */
	acpigen_write_if_cond_ref_of(MAINBOARD_HOOK);
	acpigen_emit_namestring(MAINBOARD_HOOK);
	acpigen_write_integer(1);
	acpigen_write_if_end();

	/* Save the current PM bits then enable GPIO PM with
	   MISCCFG_GPIO_PM_CONFIG_BITS */
	acpigen_write_if_cond_ref_of(ENABLE_PM_BITS_HOOK);
	acpigen_emit_namestring(ENABLE_PM_BITS_HOOK);
	acpigen_write_if_end();
}

static void lpi_s0ix_exit(void *unused)
{
	/* Inform the EC */
	acpigen_write_if_cond_ref_of(EC_S0IX_HOOK);
	acpigen_emit_namestring(EC_S0IX_HOOK);
	acpigen_write_integer(0);
	acpigen_write_if_end();

	/* Provide a board level S0ix hook */
	acpigen_write_if_cond_ref_of(MAINBOARD_HOOK);
	acpigen_emit_namestring(MAINBOARD_HOOK);
	acpigen_write_integer(0);
	acpigen_write_if_end();

	/* Restore GPIO all Community PM */
	acpigen_write_if_cond_ref_of(RESTORE_PM_BITS_HOOK);
	acpigen_emit_namestring(RESTORE_PM_BITS_HOOK);
	acpigen_write_if_end();
}

static void lpi_display_on(void *unused)
{
	/* Provide a board level S0ix hook */
	acpigen_write_if_cond_ref_of(MAINBOARD_DISPLAY_HOOK);
	acpigen_emit_namestring(MAINBOARD_DISPLAY_HOOK);
	acpigen_write_integer(1);
	acpigen_write_if_end();
}

static void lpi_display_off(void *unused)
{
	/* Provide a board level S0ix hook */
	acpigen_write_if_cond_ref_of(MAINBOARD_DISPLAY_HOOK);
	acpigen_emit_namestring(MAINBOARD_DISPLAY_HOOK);
	acpigen_write_integer(0);
	acpigen_write_if_end();
}

static void (*lpi_s0_helpers[])(void *) = {
	NULL,			/* enumerate functions (autogenerated) */
	lpi_get_constraints,	/* get device constraints */
	NULL,			/* get crash dump device */
	lpi_display_off,	/* display off notify */
	lpi_display_on,		/* display on notify */
	lpi_s0ix_entry,		/* s0ix entry */
	lpi_s0ix_exit,		/* s0ix exit */
};

static void pep_s0ix_return_lpm_requirements(void *arg)
{
	if (!CONFIG(SOC_INTEL_COMMON_BLOCK_ACPI_PEP_LPM_REQ)) {
		acpigen_write_return_singleton_buffer(0x0);
		return;
	}

	struct reg_info *info = (struct reg_info *)arg;
	acpigen_write_return_byte_buffer(info->addr, info->buffer_size);
}

static void (*pep_s0ix[])(void *) = {
	NULL,					/* enumerate functions (autogenerated) */
	pep_s0ix_return_lpm_requirements,	/* Return LPM requirements */
};

void generate_acpi_power_engine_with_lpm(const struct soc_pmc_lpm *lpm)
{
	struct reg_info info;
	size_t uuid_count = 1;
	struct dsm_uuid ids[] = {
		DSM_UUID(LPI_S0_HELPER_UUID, lpi_s0_helpers, ARRAY_SIZE(lpi_s0_helpers), NULL),
		DSM_UUID(PEP_S0IX_UUID, pep_s0ix, ARRAY_SIZE(pep_s0ix), &info),
	};

	acpigen_write_scope(PEPD_SCOPE);
	acpigen_write_device("PEPD");

	acpigen_write_name_string("_HID", SYSTEM_POWER_MANAGEMENT_HID);
	acpigen_write_name("_CID");
	acpigen_emit_eisaid(SYSTEM_POWER_MANAGEMENT_CID);

	read_pmc_lpm_requirements(lpm, &info);
	if (info.buffer_size)
		uuid_count++;

	acpigen_write_dsm_uuid_arr(ids, uuid_count);
	acpigen_write_device_end();
	acpigen_write_scope_end();

	free(info.addr);
	printk(BIOS_INFO, PEPD_SCOPE ".PEPD: Intel Power Engine Plug-in\n");
}

void generate_acpi_power_engine(void)
{
	generate_acpi_power_engine_with_lpm(NULL);
}
