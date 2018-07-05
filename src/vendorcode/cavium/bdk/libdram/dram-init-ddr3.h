/***********************license start***********************************
* Copyright (c) 2003-2017  Cavium Inc. (support@cavium.com). All rights
* reserved.
*
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions are
* met:
*
*   * Redistributions of source code must retain the above copyright
*     notice, this list of conditions and the following disclaimer.
*
*   * Redistributions in binary form must reproduce the above
*     copyright notice, this list of conditions and the following
*     disclaimer in the documentation and/or other materials provided
*     with the distribution.
*
*   * Neither the name of Cavium Inc. nor the names of
*     its contributors may be used to endorse or promote products
*     derived from this software without specific prior written
*     permission.
*
* This Software, including technical data, may be subject to U.S. export
* control laws, including the U.S. Export Administration Act and its
* associated regulations, and may be subject to export or import
* regulations in other countries.
*
* TO THE MAXIMUM EXTENT PERMITTED BY LAW, THE SOFTWARE IS PROVIDED "AS IS"
* AND WITH ALL FAULTS AND CAVIUM INC. MAKES NO PROMISES, REPRESENTATIONS OR
* WARRANTIES, EITHER EXPRESS, IMPLIED, STATUTORY, OR OTHERWISE, WITH RESPECT
* TO THE SOFTWARE, INCLUDING ITS CONDITION, ITS CONFORMITY TO ANY
* REPRESENTATION OR DESCRIPTION, OR THE EXISTENCE OF ANY LATENT OR PATENT
* DEFECTS, AND CAVIUM SPECIFICALLY DISCLAIMS ALL IMPLIED (IF ANY) WARRANTIES
* OF TITLE, MERCHANTABILITY, NONINFRINGEMENT, FITNESS FOR A PARTICULAR
* PURPOSE, LACK OF VIRUSES, ACCURACY OR COMPLETENESS, QUIET ENJOYMENT,
* QUIET POSSESSION OR CORRESPONDENCE TO DESCRIPTION. THE ENTIRE  RISK
* ARISING OUT OF USE OR PERFORMANCE OF THE SOFTWARE LIES WITH YOU.
***********************license end**************************************/

/**
 * Function for DDR3 init. Internal use only.
 */

extern void perform_octeon3_ddr3_sequence(bdk_node_t node, int rank_mask,
    int ddr_interface_num, int sequence);
extern void perform_ddr_init_sequence(bdk_node_t node, int rank_mask,
    int ddr_interface_num);
extern int ddr_memory_preserved(bdk_node_t node);

extern int init_octeon3_ddr3_interface(bdk_node_t node,
    const ddr_configuration_t *ddr_configuration, uint32_t ddr_hertz,
    uint32_t cpu_hertz, uint32_t ddr_ref_hertz, int board_type,
    int board_rev_maj, int board_rev_min, int ddr_interface_num,
    uint32_t ddr_interface_mask);

extern void
set_vref(bdk_node_t node, int ddr_interface_num, int rank,
	 int range, int value);

typedef struct {
    unsigned char *rodt_ohms;
    unsigned char *rtt_nom_ohms;
    unsigned char *rtt_nom_table;
    unsigned char *rtt_wr_ohms;
    unsigned char *dic_ohms;
    short         *drive_strength;
    short         *dqx_strength;
} impedence_values_t;

extern impedence_values_t ddr4_impedence_values;

extern int
compute_vref_value(bdk_node_t node, int ddr_interface_num,
		   int rankx, int dimm_count, int rank_count,
		   impedence_values_t *imp_values, int is_stacked_die);

extern unsigned short
load_dac_override(int node, int ddr_interface_num,
		  int dac_value, int byte);
extern int
read_DAC_DBI_settings(int node, int ddr_interface_num,
		      int dac_or_dbi, int *settings);
extern void
display_DAC_DBI_settings(int node, int ddr_interface_num, int dac_or_dbi,
                         int ecc_ena, int *settings, const char *title);

#define RODT_OHMS_COUNT        8
#define RTT_NOM_OHMS_COUNT     8
#define RTT_NOM_TABLE_COUNT    8
#define RTT_WR_OHMS_COUNT      8
#define DIC_OHMS_COUNT         3
#define DRIVE_STRENGTH_COUNT  15

extern uint64_t hertz_to_psecs(uint64_t hertz);
extern uint64_t psecs_to_mts(uint64_t psecs);
extern uint64_t mts_to_hertz(uint64_t mts);
//extern uint64_t pretty_psecs_to_mts(uint64_t psecs);
extern unsigned long pretty_psecs_to_mts(uint64_t psecs);	/* FIXME(dhendrix) */
