/* SPDX-License-Identifier: GPL-2.0-only */

/*
 * SoC Specific TSENS Map for Calypso
 */

#include <soc/qcom_tsens.h>

/* Controller Configuration for SoC TSENS */
static const struct tsens_controller tsens_blocks[] = {
	/* Name,   TM base,    SROT base,  Sensor count */
	{"tsens0", 0x0c22c000, 0x0c222000, 13},
	{"tsens1", 0x0c22d000, 0x0c223000, 9},
	{"tsens2", 0x0c22e000, 0x0c224000, 13},
	{"tsens3", 0x0c22f000, 0x0c225000, 8},
	{"tsens4", 0x0c230000, 0x0c226000, 11},
	{"tsens5", 0x0c231000, 0x0c227000, 15},
};

/* Sensor Definition Mapping */
const struct thermal_zone_map qcom_thermal_zones[] = {
	/* Label,     Type,       Controller,    HW ID, Threshold (in milli degree C) */

	/* SoC TSENS Controller 0 (Cluster 0) */
	{"aoss-0",    TYPE_TSENS, &tsens_blocks[0], 0,  105000},
	{"cpu-0-0-0", TYPE_TSENS, &tsens_blocks[0], 1,  108000},
	{"cpu-0-0-1", TYPE_TSENS, &tsens_blocks[0], 2,  108000},
	{"cpu-0-1-0", TYPE_TSENS, &tsens_blocks[0], 3,  108000},
	{"cpu-0-1-1", TYPE_TSENS, &tsens_blocks[0], 4,  108000},
	{"cpu-0-2-0", TYPE_TSENS, &tsens_blocks[0], 5,  108000},
	{"cpu-0-2-1", TYPE_TSENS, &tsens_blocks[0], 6,  108000},
	{"cpu-0-3-0", TYPE_TSENS, &tsens_blocks[0], 7,  108000},
	{"cpu-0-3-1", TYPE_TSENS, &tsens_blocks[0], 8,  108000},
	{"cpuss-0-0", TYPE_TSENS, &tsens_blocks[0], 9,  105000},
	{"cpuss-0-1", TYPE_TSENS, &tsens_blocks[0], 10, 105000},
	{"ddr-0",     TYPE_TSENS, &tsens_blocks[0], 11, 105000},
	{"video",     TYPE_TSENS, &tsens_blocks[0], 12, 105000},

	/* SoC TSENS Controller 1 (Cluster 1) */
	{"aoss-1",    TYPE_TSENS, &tsens_blocks[1], 0,  105000},
	{"cpu-1-0-0", TYPE_TSENS, &tsens_blocks[1], 1,  108000},
	{"cpu-1-0-1", TYPE_TSENS, &tsens_blocks[1], 2,  108000},
	{"cpu-1-1-0", TYPE_TSENS, &tsens_blocks[1], 3,  108000},
	{"cpu-1-1-1", TYPE_TSENS, &tsens_blocks[1], 4,  108000},
	{"cpu-1-2-0", TYPE_TSENS, &tsens_blocks[1], 5,  108000},
	{"cpu-1-2-1", TYPE_TSENS, &tsens_blocks[1], 6,  108000},
	{"cpu-1-3-0", TYPE_TSENS, &tsens_blocks[1], 7,  108000},
	{"cpu-1-3-1", TYPE_TSENS, &tsens_blocks[1], 8,  108000},

	/* SoC TSENS Controller 2 (Cluster 2) */
	{"aoss-2",    TYPE_TSENS, &tsens_blocks[2], 0,  105000},
	{"cpu-2-0-0", TYPE_TSENS, &tsens_blocks[2], 1,  108000},
	{"cpu-2-0-1", TYPE_TSENS, &tsens_blocks[2], 2,  108000},
	{"cpu-2-1-0", TYPE_TSENS, &tsens_blocks[2], 3,  108000},
	{"cpu-2-1-1", TYPE_TSENS, &tsens_blocks[2], 4,  108000},
	{"cpu-2-2-0", TYPE_TSENS, &tsens_blocks[2], 5,  108000},
	{"cpu-2-2-1", TYPE_TSENS, &tsens_blocks[2], 6,  108000},
	{"cpu-2-3-0", TYPE_TSENS, &tsens_blocks[2], 7,  108000},
	{"cpu-2-3-1", TYPE_TSENS, &tsens_blocks[2], 8,  108000},
	{"cpuss-2-0", TYPE_TSENS, &tsens_blocks[2], 9,  105000},
	{"cpuss-2-1", TYPE_TSENS, &tsens_blocks[2], 10, 105000},
	{"ddr-1",     TYPE_TSENS, &tsens_blocks[2], 11, 105000},
	{"display",   TYPE_TSENS, &tsens_blocks[2], 12, 105000},

	/* SoC TSENS Controller 3 (NSP / System) */
	{"aoss-3",    TYPE_TSENS, &tsens_blocks[3], 0,  105000},
	{"nsp-0",     TYPE_TSENS, &tsens_blocks[3], 1,  105000},
	{"nsp-1",     TYPE_TSENS, &tsens_blocks[3], 2,  105000},
	{"nsp-2",     TYPE_TSENS, &tsens_blocks[3], 3,  105000},
	{"nsp-3",     TYPE_TSENS, &tsens_blocks[3], 4,  105000},
	{"nsp-4",     TYPE_TSENS, &tsens_blocks[3], 5,  105000},
	{"nsp-5",     TYPE_TSENS, &tsens_blocks[3], 6,  105000},
	{"nsp-6",     TYPE_TSENS, &tsens_blocks[3], 7,  105000},

	/* SoC TSENS Controller 4 (Cluster 3) */
	{"aoss-4",    TYPE_TSENS, &tsens_blocks[4], 0,  105000},
	{"cpu-3-0-0", TYPE_TSENS, &tsens_blocks[4], 1,  108000},
	{"cpu-3-0-1", TYPE_TSENS, &tsens_blocks[4], 2,  108000},
	{"cpu-3-1-0", TYPE_TSENS, &tsens_blocks[4], 3,  108000},
	{"cpu-3-1-1", TYPE_TSENS, &tsens_blocks[4], 4,  108000},
	{"cpu-3-2-0", TYPE_TSENS, &tsens_blocks[4], 5,  108000},
	{"cpu-3-2-1", TYPE_TSENS, &tsens_blocks[4], 6,  108000},
	{"cpu-3-3-0", TYPE_TSENS, &tsens_blocks[4], 7,  108000},
	{"cpu-3-3-1", TYPE_TSENS, &tsens_blocks[4], 8,  108000},
	{"cpuss-3-0", TYPE_TSENS, &tsens_blocks[4], 9,  105000},
	{"cpuss-3-1", TYPE_TSENS, &tsens_blocks[4], 10, 105000},

	/* SoC TSENS Controller 5 (GPU / Camera) */
	{"gpuss-0",   TYPE_TSENS, &tsens_blocks[5], 0,  95000},
	{"gpuss-1",   TYPE_TSENS, &tsens_blocks[5], 1,  95000},
	{"gpuss-2",   TYPE_TSENS, &tsens_blocks[5], 2,  95000},
	{"gpuss-3",   TYPE_TSENS, &tsens_blocks[5], 3,  95000},
	{"gpuss-4",   TYPE_TSENS, &tsens_blocks[5], 4,  95000},
	{"gpuss-5",   TYPE_TSENS, &tsens_blocks[5], 5,  95000},
	{"gpuss-6",   TYPE_TSENS, &tsens_blocks[5], 6,  95000},
	{"gpuss-7",   TYPE_TSENS, &tsens_blocks[5], 7,  95000},
	{"gpuss-8",   TYPE_TSENS, &tsens_blocks[5], 8,  95000},
	{"gpuss-9",   TYPE_TSENS, &tsens_blocks[5], 9,  95000},
	{"gpuss-10",  TYPE_TSENS, &tsens_blocks[5], 10, 95000},
	{"gpuss-11",  TYPE_TSENS, &tsens_blocks[5], 11, 95000},
	{"camera-0",  TYPE_TSENS, &tsens_blocks[5], 12, 105000},
	{"camera-1",  TYPE_TSENS, &tsens_blocks[5], 13, 105000},
	{"camera-2",  TYPE_TSENS, &tsens_blocks[5], 14, 105000},

	/* Sentinel */
	{NULL, 0, NULL, 0, 0}
};
