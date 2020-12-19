/* SPDX-License-Identifier: GPL-2.0-only */

#include <boardid.h>
#include <boot/coreboot_tables.h>
#include <delay.h>
#include <device/device.h>
#include <gpio.h>
#include <soc/clock.h>
#include <soc/soc_services.h>
#include <soc/usb.h>
#include <symbols.h>

#include <vendorcode/google/chromeos/chromeos.h>
#include "mmu.h"

#define USB_ENABLE_GPIO		51

static void setup_usb(void)
{
#if !CONFIG(BOARD_VARIANT_AP148)
	gpio_tlmm_config_set(USB_ENABLE_GPIO, FUNC_SEL_GPIO,
			     GPIO_PULL_UP, GPIO_10MA, GPIO_ENABLE);
	gpio_set(USB_ENABLE_GPIO, 1);
#endif
	usb_clock_config();

	setup_usb_host1();
}

#define TPM_RESET_GPIO 22
static void setup_tpm(void)
{
	if (board_id() != BOARD_ID_PROTO_0)
		return; /* Only proto0 have TPM reset connected to GPIO22 */

	gpio_tlmm_config_set(TPM_RESET_GPIO, FUNC_SEL_GPIO, GPIO_PULL_UP,
			     GPIO_4MA, GPIO_ENABLE);
	/*
	 * Generate a reset pulse. The spec calls for 80 us minimum, let's
	 * make it twice as long. If the output was driven low originally, the
	 * reset pulse will be even longer.
	 */
	gpio_set(TPM_RESET_GPIO, 0);
	udelay(160);
	gpio_set(TPM_RESET_GPIO, 1);
}

#define SW_RESET_GPIO 26
static void assert_sw_reset(void)
{
	if (board_id() == BOARD_ID_PROTO_0)
		return;

	/*
	 * only proto0.2 and later care about this. We want to keep the
	 * ethernet switch in reset, otherwise it comes up in default
	 * (bridging) mode.
	 */
	gpio_tlmm_config_set(SW_RESET_GPIO, FUNC_SEL_GPIO,
			     GPIO_PULL_UP, GPIO_4MA, GPIO_ENABLE);

	gpio_set(SW_RESET_GPIO, 1);
}

static void mainboard_init(struct device *dev)
{
	 /* disable mmu and d-cache before setting up secure world.*/
	 dcache_mmu_disable();
	 start_tzbsp();
	 /* Setup mmu and d-cache again as non secure entries. */
	 setup_mmu(DRAM_INITIALIZED);
	 start_rpm();
	 setup_usb();
	 assert_sw_reset();
	 setup_tpm();
	 /* Functionally a 0-cost no-op if NAND is not present */
	 board_nand_init();

	/* Copy WIFI calibration data into CBMEM. */
	if (CONFIG(CHROMEOS))
		cbmem_add_vpd_calibration_data();

	/*
	 * Make sure bootloader can issue sounds The frequency is calculated
	 * as "<frame_rate> * <bit_width> * <channels> * 4", i.e.
	 *
	 * 48000 * 2 * 16 * 4 = 6144000
	 */
	audio_clock_config(6144000);
}

static void mainboard_enable(struct device *dev)
{
	dev->ops->init = &mainboard_init;
}

struct chip_operations mainboard_ops = {
	.name	= "storm",
	.enable_dev = mainboard_enable,
};

void lb_board(struct lb_header *header)
{
	struct lb_range *dma;

	dma = (struct lb_range *)lb_new_record(header);
	dma->tag = LB_TAG_DMA;
	dma->size = sizeof(*dma);
	dma->range_start = (uintptr_t)_dma_coherent;
	dma->range_size = REGION_SIZE(dma_coherent);

	/* Retrieve the switch interface MAC addresses. */
	if (CONFIG(CHROMEOS))
		lb_table_add_macs_from_vpd(header);
}
