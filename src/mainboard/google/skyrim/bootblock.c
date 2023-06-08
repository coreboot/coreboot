/* SPDX-License-Identifier: GPL-2.0-only */

#include <amdblocks/espi.h>
#include <amdblocks/reset.h>
#include <bootblock_common.h>
#include <baseboard/variants.h>
#include <console/console.h>
#include <ec/google/chromeec/ec.h>
#include <pc80/mc146818rtc.h>
#include <soc/espi.h>
#include <string.h>

#define CMOS_EXTENDED_ADDR(x)          (128 + (x))
#define CMOS_MEM_RESTORE_OFFSET        0x0D
#define CMOS_BITMAP_SKIP_RESET_TOGGLE  0x10
#define HYNIX_PART_NAME                "H9JCNNNCP3MLYR-N6E"
#define HYNIX_PART_LEN                 18

/* Ensure SKIP_RESET_TOGGLE CMOS bit set for specific Hynix part on Frostflow, cleared otherwise */
static void hynix_dram_cmos_check(void)
{
	char cbi_part_number[DIMM_INFO_PART_NUMBER_SIZE];
	bool skip_reset_toggle, cmos_bit_set;
	unsigned char byte_value;

	byte_value = cmos_read(CMOS_EXTENDED_ADDR(CMOS_MEM_RESTORE_OFFSET));
	cmos_bit_set = (byte_value & CMOS_BITMAP_SKIP_RESET_TOGGLE) != 0;

	if (CONFIG(BOARD_GOOGLE_FROSTFLOW)) {

		printk(BIOS_SPEW, "Checking DRAM part #\n");
		if (google_chromeec_cbi_get_dram_part_num(
				cbi_part_number, sizeof(cbi_part_number)) == 0) {

			skip_reset_toggle = strncmp(cbi_part_number, HYNIX_PART_NAME, HYNIX_PART_LEN) == 0;
			if (skip_reset_toggle) {
				printk(BIOS_SPEW, "SKIP_RESET_TOGGLE needed, checking CMOS bit is set\n");
				if (!cmos_bit_set) {
					printk(BIOS_SPEW, "Bit is unset; setting and rebooting\n");
					cmos_write((byte_value | CMOS_BITMAP_SKIP_RESET_TOGGLE),
							CMOS_EXTENDED_ADDR(CMOS_MEM_RESTORE_OFFSET));
					warm_reset();
				}
				printk(BIOS_SPEW, "Bit already set; nothing to do.\n");
				return;
			}
		} else {
			printk(BIOS_ERR, "Unable to read DRAM part # from CBI; CMOS bit will be cleared if set\n");
		}
	}
	/* Ensure SKIP_RESET_TOGGLE bit cleared if not FF, not bad DRAM part, or error reading part # */
	if (cmos_bit_set) {
		printk(BIOS_SPEW, "CMOS SKIP_RESET_TOGGLE bit is set; clearing and rebooting\n");
		cmos_write((byte_value & ~CMOS_BITMAP_SKIP_RESET_TOGGLE),
					CMOS_EXTENDED_ADDR(CMOS_MEM_RESTORE_OFFSET));
		warm_reset();
	} else {
		printk(BIOS_SPEW, "No change to CMOS SKIP_RESET_TOGGLE bit is needed\n");
	}
}

void mb_set_up_early_espi(void)
{
	size_t num_gpios;
	const struct soc_amd_gpio *gpios;

	variant_espi_gpio_table(&gpios, &num_gpios);
	gpio_configure_pads(gpios, num_gpios);

	espi_switch_to_spi1_pads();
}

void bootblock_mainboard_early_init(void)
{
	size_t num_gpios, override_num_gpios;
	const struct soc_amd_gpio *gpios, *override_gpios;

	variant_tpm_gpio_table(&gpios, &num_gpios);
	gpio_configure_pads(gpios, num_gpios);

	variant_early_gpio_table(&gpios, &num_gpios);
	variant_early_override_gpio_table(&override_gpios, &override_num_gpios);
	gpio_configure_pads_with_override(gpios, num_gpios, override_gpios, override_num_gpios);
}

void bootblock_mainboard_init(void)
{
	size_t num_gpios;
	const struct soc_amd_gpio *gpios;

	hynix_dram_cmos_check();

	variant_bootblock_gpio_table(&gpios, &num_gpios);
	gpio_configure_pads(gpios, num_gpios);
}
