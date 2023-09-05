/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <baseboard/variants.h>
#include <memory_info.h>
#include <string.h>

static const struct mb_cfg kano_memcfg = {
	.type = MEM_TYPE_LP4X,

	.rcomp = {
		/* Baseboard uses only 100ohm Rcomp resistors */
		.resistor = 100,

		/* Baseboard Rcomp target values */
		.targets = {40, 30, 30, 30, 30},
	},

	/* DQ byte map */
	.lpx_dq_map = {
		.ddr0 = {
			.dq0 = {  3,  0,  2,  1,  4,  6,  5,  7, },
			.dq1 = {  12, 13, 14, 15, 8,  9,  10, 11, },
		},
		.ddr1 = {
			.dq0 = { 13, 14, 11, 12,  10, 8, 15,  9, },
			.dq1 = {  5,  2,  4,  3,  1,  6,  0,  7, },
		},
		.ddr2 = {
			.dq0 = {  2,  3,  1,  0,  7,  6,  5,  4, },
			.dq1 = { 12,  13, 14, 15, 8,  9, 10, 11, },
		},
		.ddr3 = {
			.dq0 = { 13, 14, 12, 15, 11,  9,  8, 10, },
			.dq1 = {  5,  2,  1,  4,  7,  0,  3,  6, },
		},
		.ddr4 = {
			.dq0 = { 11, 10,  8,  9, 14, 15, 13, 12, },
			.dq1 = {  3,  0,  2,  1,  5,  4,  6,  7, },
		},
		.ddr5 = {
			.dq0 = { 11, 15, 13, 12, 10,  9, 14,  8, },
			.dq1 = {  3,  0,  2,  1,  6,  7,  5,  4, },
		},
		.ddr6 = {
			.dq0 = { 11, 13, 10, 12, 15,  9, 14,  8, },
			.dq1 = {  4,  3,  5,  2,  7,  0,  1,  6, },
		},
		.ddr7 = {
			.dq0 = { 12, 13, 15, 14, 11,  9, 10,  8, },
			.dq1 = {  4,  5,  1,  2,  6,  3,  0,  7, },
		},
	},

	/* DQS CPU<>DRAM map */
	.lpx_dqs_map = {
		.ddr0 = { .dqs0 = 0, .dqs1 = 1 },
		.ddr1 = { .dqs0 = 1, .dqs1 = 0 },
		.ddr2 = { .dqs0 = 0, .dqs1 = 1 },
		.ddr3 = { .dqs0 = 1, .dqs1 = 0 },
		.ddr4 = { .dqs0 = 1, .dqs1 = 0 },
		.ddr5 = { .dqs0 = 1, .dqs1 = 0 },
		.ddr6 = { .dqs0 = 1, .dqs1 = 0 },
		.ddr7 = { .dqs0 = 1, .dqs1 = 0 },
	},

	.ect = 1, /* Enable Early Command Training */
};

static const struct mb_cfg hynix_memcfg = {
	.type = MEM_TYPE_LP4X,

	.rcomp = {
		/* Baseboard uses only 100ohm Rcomp resistors */
		.resistor = 100,

		/* Baseboard Rcomp target values */
		.targets = {40, 30, 30, 30, 30},
	},

	/* DQ byte map */
	.lpx_dq_map = {
		.ddr0 = {
			.dq0 = {  3,  0,  2,  1,  4,  6,  5,  7, },
			.dq1 = {  12, 13, 14, 15, 8,  9,  10, 11, },
		},
		.ddr1 = {
			.dq0 = { 13, 14, 11, 12,  10, 8, 15,  9, },
			.dq1 = {  5,  2,  4,  3,  1,  6,  0,  7, },
		},
		.ddr2 = {
			.dq0 = {  2,  3,  1,  0,  7,  6,  5,  4, },
			.dq1 = { 12,  13, 14, 15, 8,  9, 10, 11, },
		},
		.ddr3 = {
			.dq0 = { 13, 14, 12, 15, 11,  9,  8, 10, },
			.dq1 = {  5,  2,  1,  4,  7,  0,  3,  6, },
		},
		.ddr4 = {
			.dq0 = { 11, 10,  8,  9, 14, 15, 13, 12, },
			.dq1 = {  3,  0,  2,  1,  5,  4,  6,  7, },
		},
		.ddr5 = {
			.dq0 = { 11, 15, 13, 12, 10,  9, 14,  8, },
			.dq1 = {  3,  0,  2,  1,  6,  7,  5,  4, },
		},
		.ddr6 = {
			.dq0 = { 11, 13, 10, 12, 15,  9, 14,  8, },
			.dq1 = {  4,  3,  5,  2,  7,  0,  1,  6, },
		},
		.ddr7 = {
			.dq0 = { 12, 13, 15, 14, 11,  9, 10,  8, },
			.dq1 = {  4,  5,  1,  2,  6,  3,  0,  7, },
		},
	},

	/* DQS CPU<>DRAM map */
	.lpx_dqs_map = {
		.ddr0 = { .dqs0 = 0, .dqs1 = 1 },
		.ddr1 = { .dqs0 = 1, .dqs1 = 0 },
		.ddr2 = { .dqs0 = 0, .dqs1 = 1 },
		.ddr3 = { .dqs0 = 1, .dqs1 = 0 },
		.ddr4 = { .dqs0 = 1, .dqs1 = 0 },
		.ddr5 = { .dqs0 = 1, .dqs1 = 0 },
		.ddr6 = { .dqs0 = 1, .dqs1 = 0 },
		.ddr7 = { .dqs0 = 1, .dqs1 = 0 },
	},

	.ect = 1, /* Enable Early Command Training */

	.cs_pi_start_high_in_ect = 1,
};

const struct mb_cfg *variant_memory_params(void)
{
	const char *part_num = mainboard_get_dram_part_num();
	const char *hynix_mem1 = "H54G46CYRBX267";
	const char *hynix_mem2 = "H54G56CYRBX247";

	if (part_num) {
		if (!strcmp(part_num, hynix_mem1) || !strcmp(part_num, hynix_mem2)) {
			printk(BIOS_INFO, "Enable cs_pi_start_high_in_ect for Hynix memory\n");
			return &hynix_memcfg;
		}
	}
	return &kano_memcfg;
}
