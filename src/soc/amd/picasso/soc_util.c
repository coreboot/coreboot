/* SPDX-License-Identifier: GPL-2.0-only */

#include <console/console.h>
#include <cpu/cpu.h>
#include <fsp/util.h>
#include <FspGuids.h>
#include <misc_data.h>
#include <soc/cpu.h>
#include <soc/soc_util.h>
#include <types.h>

/*
 * The Zen/Zen+ based APUs can be RV (sometimes called RV1), PCO or RV2 silicon. RV2 has less
 * PCIe, USB3 and DisplayPort connectivity than RV(1) or PCO. A Picasso SoC is always PCO
 * silicon, a Dali SoC can either be RV2 or fused-down PCO silicon that has the same
 * connectivity as the RV2 one and Pollock is always RV2 silicon. Picasso and Dali are in a FP5
 * package while Pollock is in the smaller FT5 package.
 */

#define SOCKET_TYPE_SHIFT	28
#define SOCKET_TYPE_MASK	(0xf << SOCKET_TYPE_SHIFT)

/* some Pollock engineering samples return the wrong socket type */
enum socket_type get_socket_type(void)
{
	uint32_t ebx = cpuid_ebx(0x80000001);
	ebx = (ebx & SOCKET_TYPE_MASK) >> SOCKET_TYPE_SHIFT;
	return (enum socket_type)ebx;
}

void print_socket_type(void)
{
	enum socket_type socket = get_socket_type();

	printk(BIOS_INFO, "Socket type: ");

	switch (socket) {
	case SOCKET_FP5:
		printk(BIOS_INFO, "FP5\n");
		break;
	case SOCKET_AM4:
		printk(BIOS_INFO, "AM4\n");
		break;
	case SOCKET_FT5:
		printk(BIOS_INFO, "FT5\n");
		break;
	default:
		printk(BIOS_INFO, "unknown\n");
	}
}

/* returns 0 in case or errors */
static uint32_t get_internal_silicon_type(void)
{
	static uint32_t silicon_type;
	size_t hob_size = 0;
	const struct picasso_misc_data *hob;

	if (silicon_type)
		return silicon_type;

	hob = fsp_find_extension_hob_by_guid(PICASSO_MISC_DATA_HOB_GUID.b, &hob_size);

	if (hob == NULL || hob_size == 0) {
		printk(BIOS_ERR, "Couldn't find Picasso misc data HOB.\n");
		return 0;
	}

	if (hob->version != PICASSO_MISC_DATA_VERSION) {
		printk(BIOS_ERR, "Unexpected Picasso misc data HOB version.\n");
		return 0;
	}

	silicon_type = hob->silicon_id;

	printk(BIOS_DEBUG, "Silicon ID = 0x%x\n", silicon_type);

	return silicon_type;
}

#define SILICON_IS_MYSTERY_MEAT	(1 << 31)
#define SILICON_IS_RV2		(1 << 30)

static bool is_rv2_silicon(void)
{
	return get_internal_silicon_type() & SILICON_IS_RV2;
}

static bool is_mystery_silicon(void)
{
	return get_internal_silicon_type() & SILICON_IS_MYSTERY_MEAT;
}

static bool is_fam17_1x(void)
{
	/* mask lower model number nibble and stepping */
	return cpuid_eax(1) >> 8 == PICASSO_B1_CPUID >> 8;
}

static bool is_fam17_11(void)
{
	/* only mask stepping */
	return cpuid_eax(1) >> 4 == RAVEN1_B0_CPUID >> 4;
}

static bool is_fam17_18(void)
{
	/* only mask stepping */
	return cpuid_eax(1) >> 4 == PICASSO_B1_CPUID >> 4;
}

static bool is_fam17_2x(void)
{
	/* mask lower model number nibble and stepping */
	return cpuid_eax(1) >> 8 == RAVEN2_A1_CPUID >> 8;
}

static bool is_fam17_20(void)
{
	/* only mask stepping */
	return cpuid_eax(1) >> 4 == RAVEN2_A1_CPUID >> 4;
}

enum silicon_type get_silicon_type(void)
{
	/*
	 * RV2 is fam17_20, but might return a fam17_1x CPUID in the is_mystery_silicon() case.
	 * is_rv2_silicon() has the correct information, but requires the HOB to be present.
	 */
	if (is_fam17_20() || is_rv2_silicon())
		return SILICON_RV2;

	if (is_fam17_18() && !is_rv2_silicon())
		return SILICON_PCO;

	if (is_fam17_11() && !is_rv2_silicon())
		return SILICON_RV1;

	/* some cases might still be missing */

	return SILICON_UNKNOWN;
}

/* some Pollock engineering samples return the wrong socket type and get detected as Dali */
enum soc_type get_soc_type(void)
{
	switch (get_socket_type()) {
	case SOCKET_FP5:
		if (is_fam17_1x() && !is_mystery_silicon())
			return SOC_PICASSO;

		if (is_fam17_2x() || (is_fam17_1x() && is_mystery_silicon()))
			return SOC_DALI;

		break;
	case SOCKET_FT5:
		/* add is_fam17_20() CPUID sanity check here? */
		return SOC_POLLOCK;
		break;
	case SOCKET_AM4:
		/* AM4 SoC type detection logic not implemented */
		break;
	}

	return SOC_UNKNOWN;
}

void print_silicon_type(void)
{
	const enum silicon_type silicon = get_silicon_type();

	printk(BIOS_INFO, "Silicon type: ");

	switch (silicon) {
	case SILICON_RV1:
		printk(BIOS_INFO, "RV1\n");
		break;
	case SILICON_PCO:
		printk(BIOS_INFO, "PCO\n");
		break;
	case SILICON_RV2:
		printk(BIOS_INFO, "RV2\n");
		break;
	default:
		printk(BIOS_INFO, "unknown\n");
	}
}

void print_soc_type(void)
{
	const enum soc_type soc = get_soc_type();

	printk(BIOS_INFO, "SoC type: ");

	switch (soc) {
	case SOC_PICASSO:
		printk(BIOS_INFO, "Picasso\n");
		break;
	case SOC_DALI:
		printk(BIOS_INFO, "Dali\n");
		break;
	case SOC_POLLOCK:
		printk(BIOS_INFO, "Pollock\n");
		break;
	default:
		printk(BIOS_INFO, "unknown\n");
	}
}

bool soc_is_reduced_io_sku(void)
{
	return get_silicon_type() == SILICON_RV2 || get_soc_type() == SOC_DALI;
}

bool soc_is_raven2(void)
{
	return get_silicon_type() == SILICON_RV2;
}
