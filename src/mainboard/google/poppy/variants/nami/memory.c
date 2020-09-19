/* SPDX-License-Identifier: GPL-2.0-only */

#include <baseboard/variants.h>
#include <console/console.h>
#include <gpio.h>
#include <variant/gpio.h>
#include <string.h>

/* Rcomp resistor */
static const u16 rcomp_resistor_ddp[] = { 121, 81, 100 };
static const u16 rcomp_resistor_sdp[] = { 200, 81, 100 };
static const u16 rcomp_resistor_lpddr3[] = { 200, 81, 162 };

/* Rcomp target */
static const u16 rcomp_target[] = { 100, 40, 20, 20, 26 };
static const u16 rcomp_target_lpddr3[] = { 100, 40, 40, 23, 40 };

/* DQ byte map */
static const u8 dq_map_lpddr3[][12] = {
	{ 0x0F, 0xF0, 0x00, 0xF0, 0x0F, 0xF0,
	  0x0F, 0x00, 0xFF, 0x00, 0xFF, 0x00 },
	{ 0x0F, 0xF0, 0x00, 0xF0, 0x0F, 0xF0,
	  0x0F, 0x00, 0xFF, 0x00, 0xFF, 0x00 }
};

/* DQS CPU<>DRAM map */
static const u8 dqs_map_lpddr3[][8] = {
	{ 1, 0, 3, 2, 6, 5, 4, 7 },
	{ 0, 3, 2, 1, 6, 4, 7, 5 },
};

/* Memory ids are 1-indexed, so subtract 1 to use 0-indexed values in bitmap. */
#define MEM_ID(x)	(1 << ((x) - 1))

/* Bitmap to indicate which memory ids are using DDP. */
static const uint16_t ddp_bitmap = MEM_ID(4);

static void fill_lpddr3_memory_params(struct memory_params *p)
{
	p->type = MEMORY_LPDDR3;
	p->use_sec_spd = 1;
	p->dq_map = dq_map_lpddr3;
	p->dq_map_size = sizeof(dq_map_lpddr3);
	p->dqs_map = dqs_map_lpddr3;
	p->dqs_map_size = sizeof(dqs_map_lpddr3);
	p->rcomp_resistor = rcomp_resistor_lpddr3;
	p->rcomp_resistor_size = sizeof(rcomp_resistor_lpddr3);
	p->rcomp_target = rcomp_target_lpddr3;
	p->rcomp_target_size = sizeof(rcomp_target_lpddr3);
}

static void fill_ddr4_memory_params(struct memory_params *p)
{
	p->type = MEMORY_DDR4;
	p->use_sec_spd = 0;

	int spd_index = variant_memory_sku();
	if (spd_index == 0)
		die("SPD index is 0\n");

	/* Rcomp resistor values are different for SDP and DDP. */
	if (ddp_bitmap & MEM_ID(spd_index)) {
		p->rcomp_resistor = rcomp_resistor_ddp;
		p->rcomp_resistor_size = sizeof(rcomp_resistor_ddp);
	} else {
		p->rcomp_resistor = rcomp_resistor_sdp;
		p->rcomp_resistor_size = sizeof(rcomp_resistor_sdp);
	}

	p->rcomp_target = rcomp_target;
	p->rcomp_target_size = sizeof(rcomp_target);
}

void variant_memory_params(struct memory_params *p)
{
	memset(p, 0, sizeof(*p));
	gpio_input_pulldown(GPIO_MEM_CONFIG_4);
	if (gpio_get(GPIO_MEM_CONFIG_4))
		/* set to LPDDR3 */
		fill_lpddr3_memory_params(p);
	else
		/* default to DDR4 */
		fill_ddr4_memory_params(p);

	/* GPP_D10 set to 0 for dual channel and 1 for single channel */
	if (gpio_get(GPP_D10))
		p->single_channel = 1;
}
