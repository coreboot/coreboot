/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <baseboard/gpio.h>
#include <bootmode.h>
#include <boot/coreboot_tables.h>
#include <gpio.h>
#include <soc/gpio.h>
#include <variant/gpio.h>
#include <types.h>
#include <vendorcode/google/chromeos/chromeos.h>
#include <security/tpm/tss.h>
#include <device/device.h>
#include <intelblocks/pmclib.h>
#include <soc/pmc.h>
#include <soc/pci_devs.h>

enum rec_mode_state {
	REC_MODE_UNINITIALIZED,
	REC_MODE_NOT_REQUESTED,
	REC_MODE_REQUESTED,
};

void fill_lb_gpios(struct lb_gpios *gpios)
{
	struct lb_gpio chromeos_gpios[] = {
		{-1, ACTIVE_HIGH, get_lid_switch(), "lid"},
		{-1, ACTIVE_HIGH, 0, "power"},
		{-1, ACTIVE_HIGH, gfx_get_init_done(), "oprom"},
		{-1, ACTIVE_HIGH, 0, "EC in RW"},
	};
	lb_add_gpios(gpios, chromeos_gpios, ARRAY_SIZE(chromeos_gpios));
}

int get_write_protect_state(void)
{
	return gpio_get(GPIO_PCH_WP);
}

static bool raw_get_recovery_mode_switch(void)
{
	return !gpio_get(GPIO_REC_MODE);
}


int get_recovery_mode_switch(void)
{
	static enum rec_mode_state saved_rec_mode = REC_MODE_UNINITIALIZED;
	enum rec_mode_state state = REC_MODE_NOT_REQUESTED;
	uint8_t cr50_state = 0;

	/* Check cached state, since TPM will only tell us the first time */
	if (saved_rec_mode != REC_MODE_UNINITIALIZED)
		return saved_rec_mode == REC_MODE_REQUESTED;

	/*
	 * Read one-time recovery request from cr50 in verstage only since
	 * the TPM driver won't be set up in time for other stages like romstage
	 * and the value from the TPM would be wrong anyway since the verstage
	 * read would have cleared the value on the TPM.
	 *
	 * The TPM recovery request is passed between stages through vboot data
	 * or cbmem depending on stage.
	 */
	if (ENV_SEPARATE_VERSTAGE &&
	    tlcl_cr50_get_recovery_button(&cr50_state) == TPM_SUCCESS &&
	    cr50_state)
		state = REC_MODE_REQUESTED;

	/* Read state from the GPIO controlled by servo. */
	if (raw_get_recovery_mode_switch())
		state = REC_MODE_REQUESTED;

	/* Store the state in case this is called again in verstage. */
	saved_rec_mode = state;

	return state == REC_MODE_REQUESTED;
}

int get_lid_switch(void)
{
	return 1;
}

void mainboard_prepare_cr50_reset(void)
{
	/* Ensure system powers up after CR50 reset */
	if (ENV_RAMSTAGE)
		pmc_soc_set_afterg3_en(true);
}

int get_ec_is_trusted(void)
{
	/* Do not have a Chrome EC involved in entering recovery mode;
	   Always return trusted. */
	return 1;
}
