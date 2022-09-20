/* SPDX-License-Identifier: GPL-2.0-only */

#include <acpi/acpi.h>
#include <acpi/acpigen.h>
#include <baseboard/variants.h>
#include <boardid.h>
#include <delay.h>
#include <device/pci.h>
#include <gpio.h>
#include <timer.h>
#include <types.h>

#define GPU_1V8_PWR_EN		GPP_E18
#define GPU_1V8_PG		GPP_E20
#define NV33_PWR_EN		GPP_A21
#define NV33_PG			GPP_A22
#define NVVDD_PWR_EN		GPP_E0
#define NVVDD_PG		GPP_E3
#define PEXVDD_PWR_EN		GPP_E10
#define PEXVDD_PG		GPP_E17
#define FBVDD_PWR_EN		GPP_A19
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

	/* Delay after sequencing this rail. */
	unsigned int delay_ms;
};

/* In GCOFF exit order (i.e., power-on order) */
static struct power_rail_sequence gpu_on_seq[] = {
	{ "GPU 1.8V",		GPU_1V8_PWR_EN,	false, GPU_1V8_PG, },
	{ "NV3_3",		NV33_PWR_EN,	false, NV33_PG, },
	{ "NVVDD+MSVDD",	NVVDD_PWR_EN,	false, NVVDD_PG, },
	{ "PEXVDD",		PEXVDD_PWR_EN,	false, PEXVDD_PG, },
	{ "FBVDD",		FBVDD_PWR_EN,	true,  FBVDD_PG, },
};

/* In GCOFF entry order (i.e., power-off order) */
static struct power_rail_sequence gpu_off_seq[] = {
	{ "FBVDD",		FBVDD_PWR_EN,	true,  FBVDD_PG,	0,},
	{ "PEXVDD",		PEXVDD_PWR_EN,	false, PEXVDD_PG,	10,},
	{ "NVVDD+MSVDD",	NVVDD_PWR_EN,	false, NVVDD_PG,	2,},
	{ "NV3_3",		NV33_PWR_EN,	false, NV33_PG,		4,},
	{ "GPU 1.8V",		GPU_1V8_PWR_EN,	false, GPU_1V8_PG,	0,},
};

enum rail_state {
	RAIL_OFF = 0,
	RAIL_ON = 1,
};

/* Assert the VR's enable pin, and wait until the VR's power-good is asserted. */
static bool sequence_rail(const struct power_rail_sequence *seq, enum rail_state state)
{
	enum rail_state pwr_en_state = state;
	bool result;

	if (seq->pwr_en_active_low)
		pwr_en_state = !pwr_en_state;

	gpio_output(seq->pwr_en_gpio, pwr_en_state);
	result = wait_us(DEFAULT_PG_TIMEOUT_US, gpio_get(seq->pg_gpio) == state) >= 0;
	if (seq->delay_ms)
		mdelay(seq->delay_ms);

	return result;
}

static void dgpu_power_sequence_off(void)
{
	/* Assert reset and clear power-good */
	gpio_output(GPU_PERST_L, 0);

	/* Inform the GPU that the power is no longer good. */
	gpio_output(GPU_ALLRAILS_PG, 0);

	for (size_t i = 0; i < ARRAY_SIZE(gpu_off_seq); i++) {
		if (!sequence_rail(&gpu_off_seq[i], RAIL_OFF)) {
			printk(BIOS_ERR, "Failed to disable %s rail, continuing!\n",
			       gpu_off_seq[i].name);
		}
	}
}

static void dgpu_power_sequence_on(void)
{
	/* Assert PERST# */
	gpio_output(GPU_PERST_L, 0);

	for (size_t i = 0; i < ARRAY_SIZE(gpu_on_seq); i++) {
		if (!sequence_rail(&gpu_on_seq[i], RAIL_ON)) {
			printk(BIOS_ERR, "Failed to enable %s rail, sequencing back down!\n",
			       gpu_on_seq[i].name);

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
	mdelay(1);

	gpu_powered_on = true;
}

void variant_init(void)
{
	if (acpi_is_wakeup_s3())
		return;

	/* For board revs 3 and later, the power good pin for the NVVDD
	   VR moved from GPP_E16 to GPP_E3, so patch up the table for
	   old board revs. */
	if (board_id() < 3) {
		gpu_on_seq[2].pg_gpio = GPP_E16;
		gpu_off_seq[2].pg_gpio = GPP_E16;
	}

	dgpu_power_sequence_on();
}

/*
 * For board revs 3 and later, the PG pin for the NVVDD VR moved from GPP_E16 to
 * GPP_E3. To accommodate this, the DSDT contains a Name that this code will
 * write the correct GPIO # to depending on the board rev, and we'll use that
 * instead.
 */
void variant_fill_ssdt(const struct device *dev)
{
	const int nvvdd_pg_gpio = board_id() < 3 ? GPP_E16 : GPP_E3;
	acpigen_write_scope("\\_SB.PCI0.PEG0.PEGP");
	acpigen_write_method("_INI", 0);
	acpigen_write_store_int_to_namestr(nvvdd_pg_gpio, "NVPG");
	acpigen_write_method_end();
	acpigen_write_scope_end();
}

void variant_finalize(void)
{
	/*
	 * Currently the `pch_pirq_init()` function in lpc_lib.c will program
	 * PIRQ IRQs for all PCI devices discovered during enumeration. This may
	 * not be correct for all devices, and causes strange behavior with the
	 * Nvidia dGPU; it will start out with IRQ 11 and then after a
	 * suspend/resume cycle, it will get programmed back to 16, so the Linux
	 * kernel must be doing some IRQ sanitization at some point.  To fix
	 * this anomaly, explicitly program the IRQ to 16 (which we know is what
	 * IRQ it will eventually take).
	*/
	const struct device *dgpu = DEV_PTR(dgpu);
	pci_write_config8(dgpu, PCI_INTERRUPT_LINE, 16);
}
