/*
 * (C) Copyright 2001
 * Humboldt Solutions Ltd, adrian@humboldt.co.uk.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */

#ifndef _MPC107_H
#define _MPC107_H

#ifdef ASM
#define BMC_BASE	0x8000	/* Bridge memory controller base address */
#else

enum sdram_error_detect {
    ERRORS_NONE, ERRORS_PARITY, ERRORS_ECC
};
    
typedef struct sdram_dimm_info
{
    unsigned size;
    unsigned number;
    char part_number[20];
    struct sdram_bank_info *bank1;
    struct sdram_bank_info *bank2;
} sdram_dimm_info;

typedef struct sdram_bank_info
{
    unsigned number;
    unsigned char row_bits;
    unsigned char internal_banks;
    unsigned char col_bits;
    unsigned char data_width;
    /* Cycle and access times are stored with lowest CAS latency first. Units
       are 0.01ns */
    unsigned short cycle_time[3];
    unsigned short access_time[3];
    /* Best CAS latencies */
    unsigned char cas_latency[3];
    unsigned char cs_latency;
    unsigned char we_latency;
    unsigned char min_back_to_back;
    unsigned char min_row_precharge;
    unsigned char min_active_to_active;
    unsigned char min_ras_to_cas;
    unsigned char min_ras;
    unsigned char burst_mask;
    enum sdram_error_detect error_detect;
    /* Bank size */
    unsigned size;
    unsigned long start;
    unsigned long end;
    enum sdram_error_detect actual_detect;
    unsigned char actual_cas;
} sdram_bank_info;

void sdram_dimm_to_bank_info(const char *dimm_data,  sdram_dimm_info *dimm, int verbose);
void print_sdram_dimm_info(const sdram_dimm_info *dimm);
void print_sdram_bank_info(const sdram_bank_info *bank);

unsigned long hostbridge_config_memory(int no_banks, sdram_bank_info *bank, int for_real);
void hostbridge_probe_dimms(int no_dimms, sdram_dimm_info *dimm, sdram_bank_info * bank);
unsigned mpc107_config_memory(void);
#endif
#endif
