/* SPDX-License-Identifier: GPL-2.0-only */

#include <acpi/acpi.h>
#include <acpi/acpigen.h>
#include <baseboard/variants.h>
#include <delay.h>
#include <gpio.h>
#include <timer.h>
#include <types.h>

#define GPU_1V8_PWR_EN		GPP_E18
#define GPU_1V8_PG		GPP_E20
#define NV33_PWR_EN		GPP_A21
#define NV33_PG			GPP_A22
#define NVVDD_PWR_EN		GPP_E0
#define NVVDD_PG		GPP_E16
#define PEXVDD_PWR_EN		GPP_E10
#define PEXVDD_PG		GPP_E17
#define FBVDD_PWR_EN		GPP_A17
#define FBVDD_PG		GPP_E4
#define GPU_PERST_L		GPP_B3
#define GPU_ALLRAILS_PG		GPP_E5

#define DEFAULT_PG_TIMEOUT_US	20000

#define VGAR_BYTE_OFFSET	5

/* Maximum size of PCI config space to save. */
#define GPU_CONFIG_SAVE_SPACE_BYTES	0x100

static bool gpu_powered_on;

struct power_rail_sequence {
	const char *name;

	/* This is the GPIO (output) connected to the VR's enable pin. */
	gpio_t pwr_en_gpio;
	bool pwr_en_active_low;

	/* This is the GPIO (input) connected to the VR's power-good pin. */
	gpio_t pg_gpio;
};

/* In GCOFF exit order (i.e., power-on order) */
static const struct power_rail_sequence gpu_rails[] = {
	{ "GPU 1.8V",		GPU_1V8_PWR_EN,	false, GPU_1V8_PG, },
	{ "NV3_3",		NV33_PWR_EN,	false, NV33_PG, },
	{ "NVVDD+MSVDD",	NVVDD_PWR_EN,	true,  NVVDD_PG, },
	{ "PEXVDD",		PEXVDD_PWR_EN,	false, PEXVDD_PG, },
	{ "FBVDD",		FBVDD_PWR_EN,	false, FBVDD_PG, },
};

enum rail_state {
	RAIL_OFF = 0,
	RAIL_ON = 1,
};

/* Assert the VR's enable pin, and wait until the VR's power-good is asserted. */
static bool sequence_rail(const struct power_rail_sequence *seq, enum rail_state state)
{
	if (seq->pwr_en_active_low)
		state = !state;

	gpio_output(seq->pwr_en_gpio, state);
	return wait_us(DEFAULT_PG_TIMEOUT_US, gpio_get(seq->pg_gpio) == state) > 0;
}

static void dgpu_power_sequence_off(void)
{
	/* Assert reset and clear power-good */
	gpio_output(GPU_PERST_L, 0);
	mdelay(5);

	/* Inform the GPU that the power is no longer good. */
	gpio_output(GPU_ALLRAILS_PG, 0);

	for (int i = (int)ARRAY_SIZE(gpu_rails) - 1; i >= 0; i--) {
		if (!sequence_rail(&gpu_rails[i], RAIL_OFF)) {
			printk(BIOS_ERR, "Failed to disable %s rail, continuing!\n",
			       gpu_rails[i].name);
		}
	}
}

static void dgpu_power_sequence_on(void)
{
	/* Assert PERST# */
	gpio_output(GPU_PERST_L, 0);

	for (size_t i = 0; i < ARRAY_SIZE(gpu_rails); i++) {
		if (!sequence_rail(&gpu_rails[i], RAIL_ON)) {
			printk(BIOS_ERR, "Failed to enable %s rail, sequencing back down!\n",
			       gpu_rails[i].name);

			/* If an error occurred, then perform the power-off sequence and
			   return early to avoid setting GPU_ALLRAILS_PG and PERST_L. */
			dgpu_power_sequence_off();
			return;
	       }
	}

	/* Set power-good and release PERST# */
	gpio_output(GPU_ALLRAILS_PG, 1);
	mdelay(1);
	gpio_output(GPU_PERST_L, 1);

	printk(BIOS_INFO, "Sequenced GPU successfully\n");
	gpu_powered_on = true;
}

void variant_init(void)
{
	if (acpi_is_wakeup_s3())
		return;

	dgpu_power_sequence_on();
}

void variant_finalize(void)
{
	if (acpi_is_wakeup_s3() || !gpu_powered_on)
		return;

	/*
	 * Because the dGPU is used here in a way similar to "hybrid graphics"
	 * modes, it is powered down here. The DRIVERS_GFX_NVIDIA_SAVE_BARS
	 * option is selected for agah, so the BARs will be saved to ACPI memory
	 * during its finalize routine. Thus, it is powered down here, as the
	 * proper resources have already been allocated.
	 */
	dgpu_power_sequence_off();

	printk(BIOS_INFO, "GPU power sequenced off.\n");
}

/* Save PCI BARs to the ACPI copy of the "saved PCI config space" */
void variant_fill_ssdt(const struct device *unused)
{
	if (!gpu_powered_on)
		return;

	const struct device *dgpu = DEV_PTR(dgpu);
	acpigen_write_scope("\\_SB.PCI0.PEG0.PEGP");
	acpigen_write_method("_INI", 0);
	{
		/* Local0 = VGAR */
		acpigen_write_store();
		acpigen_emit_namestring("VGAR");
		acpigen_emit_byte(LOCAL0_OP);

		/*
		 * CreateDWordField(Local0, 11, BAR0)
		 * BAR0 = bases[0]
		 * CreateDWordField(Local0, 15, BAR1)
		 * BAR1 = bases[1]
		 * ...
		 */
		for (unsigned int idx = PCI_BASE_ADDRESS_0, i = 0; idx <= PCI_BASE_ADDRESS_5;
		     idx += sizeof(uint32_t), ++i) {
			char name[ACPI_NAME_BUFFER_SIZE];
			const struct resource *res;

			res = probe_resource(dgpu, idx);
			if (!res)
				continue;

			snprintf(name, sizeof(name), "BAR%1d", i);
			acpigen_write_create_dword_field(LOCAL0_OP, idx - VGAR_BYTE_OFFSET,
							 name);
			acpigen_write_store_int_to_namestr(res->base & 0xffffffff, name);
		}

		/* VGAR = Local0 */
		acpigen_write_store_op_to_namestr(LOCAL0_OP, "VGAR");
	}

	acpigen_write_method_end();
	acpigen_write_scope_end();
}
