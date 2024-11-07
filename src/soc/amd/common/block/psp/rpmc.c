/* SPDX-License-Identifier: GPL-2.0-only */

#include <amdblocks/reset.h>
#include <bootstate.h>
#include <console/console.h>
#include <types.h>
#include "psp_def.h"

union psp_rpmc_caps {
	struct {
		uint32_t psp_nvram_healthy		:  1; /* [ 0.. 0] */
		uint32_t psp_nvram_rpmc_protected	:  1; /* [ 1.. 1] */
		uint32_t				:  8; /* [ 2.. 9] */
		uint32_t spi_rpmc_slots_available	:  4; /* [10..13] */
		uint32_t				:  2; /* [14..15] */
		uint32_t spi_rpmc_slot_used		:  4; /* [16..19] */
		uint32_t psp_rpmc_slot_available	:  4; /* [20..23] */
		uint32_t				:  6; /* [24..29] */
		uint32_t psp_rpmc_revision		:  2; /* [30..31] */
	} r0; /* RPMC revision 0: 4 RPMC fuse slots in SoC */
	struct {
		uint32_t psp_nvram_healthy		:  1; /* [ 0.. 0] */
		uint32_t psp_nvram_rpmc_protected	:  1; /* [ 1.. 1] */
		uint32_t				:  8; /* [ 2.. 9] */
		uint32_t spi_rpmc_slots_available	:  4; /* [10..13] */
		uint32_t				:  2; /* [14..15] */
		uint32_t spi_rpmc_slot_used		:  4; /* [16..19] */
		uint32_t psp_rpmc_first_slot_available	:  7; /* [20..26] */
		uint32_t psp_rpmc_all_slots_used	:  1; /* [27..27] */
		uint32_t				:  2; /* [28..29] */
		uint32_t psp_rpmc_revision		:  2; /* [30..31] */
	} r1; /* RPMC revison 1: 16 RPMC fuse slots in SoC */
	uint32_t raw;
};

enum psp_rpmc_revision {
	PSP_RPMC_REVISION_0 = 0,
	PSP_RPMC_REVISION_1 = 1,
};

#define HSTI_STATE_RPMC_PRODUCTION_ENABLED	BIT(8)
#define HSTI_STATE_RPMC_SPI_PRESENT		BIT(9)

static bool is_hsti_rpmc_provisioned(uint32_t hsti_state)
{
	return hsti_state & HSTI_STATE_RPMC_PRODUCTION_ENABLED;
}

static bool is_hsti_rpmc_spi_present(uint32_t hsti_state)
{
	return hsti_state & HSTI_STATE_RPMC_SPI_PRESENT;
}

static void print_hsti_rpmc_state(uint32_t hsti_state)
{
	printk(BIOS_SPEW, "RPMC %s provisioned\n",
	       is_hsti_rpmc_provisioned(hsti_state) ? "is" : "isn't");
	printk(BIOS_SPEW, "SPI flash %s RPMC\n",
	       is_hsti_rpmc_spi_present(hsti_state) ? "supports" : "doesn't support");
}

static enum psp_rpmc_revision get_rpmc_rev(union psp_rpmc_caps psp_caps)
{
	/* Since the PSP RPMC revision field is in the same location for both revision 0 and 1,
	   we can usethe r0 struct in both cases for this */
	return (enum psp_rpmc_revision)psp_caps.r0.psp_rpmc_revision;
}

static void print_rpmc_general_status(uint8_t healthy, uint8_t rpmc_protected)
{
	printk(BIOS_SPEW, "PSP RPMC NVRAM %s healthy\n", healthy ? "is" : "isn't");
	printk(BIOS_SPEW, "PSP RPMC NVRAM %s using RPMC protection\n",
	       rpmc_protected ? "is" : " isn't");
}

#define SPI_RPMC_COUNTER_COUNT	4

static void print_spi_rpmc_usage(uint8_t available, uint8_t used)
{
	for (size_t i = 0; i < SPI_RPMC_COUNTER_COUNT; i++) {
		printk(BIOS_SPEW, "SPI flash RPMC counter %ld %s provisioned\n", i,
		       available & BIT(i) ? "can still be" : "has already been");
	}

	for (size_t i = 0; i < SPI_RPMC_COUNTER_COUNT; i++) {
		printk(BIOS_SPEW, "SPI flash RPMC counter %ld is%s in use\n", i,
		       used & BIT(i) ? "" : " not");
	}
}

#define PSP_RPMC_R0_SLOT_COUNT	4

static void print_rpmc_rev0_status(union psp_rpmc_caps psp_caps)
{
	print_rpmc_general_status(psp_caps.r0.psp_nvram_healthy,
				  psp_caps.r0.psp_nvram_rpmc_protected);
	print_spi_rpmc_usage(psp_caps.r0.spi_rpmc_slots_available,
			     psp_caps.r0.spi_rpmc_slot_used);
	for (size_t i = 0; i < PSP_RPMC_R0_SLOT_COUNT; i++) {
		printk(BIOS_SPEW, "SoC RPMC slot %ld %s provisioned\n", i,
		       psp_caps.r0.psp_rpmc_slot_available & BIT(i) ? "can still be" :
		       "has already been");
	}
}

static void print_rpmc_rev1_status(union psp_rpmc_caps psp_caps)
{
	print_rpmc_general_status(psp_caps.r1.psp_nvram_healthy,
				  psp_caps.r1.psp_nvram_rpmc_protected);
	print_spi_rpmc_usage(psp_caps.r1.spi_rpmc_slots_available,
			     psp_caps.r1.spi_rpmc_slot_used);
	if (psp_caps.r1.psp_rpmc_all_slots_used) {
		printk(BIOS_SPEW, "All SoC RPMC slots already used\n");
	} else {
		printk(BIOS_SPEW, "First available SoC RPMC slot is %d\n",
		       psp_caps.r1.psp_rpmc_first_slot_available);
	}
}

static void psp_rpmc_report_status(union psp_rpmc_caps psp_caps, uint32_t hsti_state)
{
	const enum psp_rpmc_revision rev = get_rpmc_rev(psp_caps);

	print_hsti_rpmc_state(hsti_state);

	printk(BIOS_SPEW, "RPMC revision %d\n", rev);

	switch (rev) {
	case PSP_RPMC_REVISION_0:
		print_rpmc_rev0_status(psp_caps);
		break;
	case PSP_RPMC_REVISION_1:
		print_rpmc_rev1_status(psp_caps);
		break;
	default:
		printk(BIOS_WARNING, "Unexpected RPMC revision\n");
	}
}

static bool is_psp_rpmc_slot_available(union psp_rpmc_caps psp_caps)
{
	const enum psp_rpmc_revision rev = get_rpmc_rev(psp_caps);

	switch (rev) {
	case PSP_RPMC_REVISION_0:
		/*
		 * psp_rpmc_slot_available doesn't contain the number of available slots, but
		 * one bit for each slot. When none of those bits is set, there are no usable
		 * slots any more
		 */
		return psp_caps.r0.psp_rpmc_slot_available != 0;
	case PSP_RPMC_REVISION_1:
		return !psp_caps.r1.psp_rpmc_all_slots_used;
	default:
		return false;
	}
}

static enum cb_err get_first_available_spi_rpmc_counter(union psp_rpmc_caps psp_caps,
							uint32_t *rpmc_counter_address)
{
	const enum psp_rpmc_revision rev = get_rpmc_rev(psp_caps);
	uint8_t spi_rpmc_available;
	unsigned int i;

	switch (rev) {
	case PSP_RPMC_REVISION_0:
		spi_rpmc_available = psp_caps.r0.spi_rpmc_slots_available;
		break;
	case PSP_RPMC_REVISION_1:
		spi_rpmc_available = psp_caps.r1.spi_rpmc_slots_available;
		break;
	default:
		return CB_ERR;
	}

	for (i = 0; i < SPI_RPMC_COUNTER_COUNT; i++) {
		if (spi_rpmc_available & BIT(i)) {
			*rpmc_counter_address = i;
			return CB_SUCCESS;
		}
	}

	/* No RPMC counter available any more in the SPI flash */
	return CB_ERR;
}

static void psp_rpmc_provision(union psp_rpmc_caps psp_caps, uint32_t hsti_state)
{
	uint32_t rpmc_counter_addr = 0;

	if (is_hsti_rpmc_provisioned(hsti_state))
		return;

	if (!is_hsti_rpmc_spi_present(hsti_state)) {
		printk(BIOS_ERR, "SPI flash doesn't support RPMC\n");
		return;
	}

	if (!is_psp_rpmc_slot_available(psp_caps)) {
		printk(BIOS_ERR, "No more RPMC provisioning slots available on this SoC\n");
		return;
	}

	if (get_first_available_spi_rpmc_counter(psp_caps, &rpmc_counter_addr) != CB_SUCCESS) {
		printk(BIOS_ERR,
		       "No more RPMC conters available for provisioning in the SPI flash\n");
		return;
	}

	struct mbox_cmd_set_rpmc_address_buffer buffer = {
		.header = {
			.size = sizeof(buffer)
		},
		.address = rpmc_counter_addr,
	};

	printk(BIOS_DEBUG, "RPMC: perform fusing using RPMC counter address %d\n",
	       rpmc_counter_addr);

	const int cmd_status = send_psp_command(MBOX_BIOS_CMD_SET_RPMC_ADDRESS, &buffer);

	psp_print_cmd_status(cmd_status, &buffer.header);

	if (cmd_status) {
		printk(BIOS_ERR, "RPMC: Fusing request failed\n");
		return;
	}

	printk(BIOS_NOTICE, "RPMC: Rebooting\n");
	/* Reboot so that the PSP will do the actual provisioning and fusing */
	warm_reset();
}

static void psp_rpmc_configuration(void *unused)
{
	union psp_rpmc_caps psp_caps;
	uint32_t hsti_state;

	if (psp_get_psp_capabilities(&psp_caps.raw) != CB_SUCCESS ||
	    psp_get_hsti_state(&hsti_state) != CB_SUCCESS) {
		printk(BIOS_ERR, "Getting RPMC state from PSP failed.\n");
		return;
	}

	psp_rpmc_report_status(psp_caps, hsti_state);

	if (CONFIG(PERFORM_RPMC_PROVISIONING))
		psp_rpmc_provision(psp_caps, hsti_state);
}

BOOT_STATE_INIT_ENTRY(BS_PAYLOAD_LOAD, BS_ON_ENTRY, psp_rpmc_configuration, NULL);
