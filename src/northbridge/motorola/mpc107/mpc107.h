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

#define MPC107_BASE 0xfc000000

#define MPC107_EUMBBAR			0x78

#define MPC107_PIC1			0xa8
#define MPC107_PIC1_CF_MP		0x000003
#define MPC107_PIC1_SPEC_PCI		0x000004
#define MPC107_PIC1_CF_APARK		0x000008
#define MPC107_PIC1_CF_LOOP_SNOOP	0x000010
#define MPC107_PIC1_LE_MODE		0x000020
#define MPC107_PIC1_ST_GATH_EN		0x000040
#define MPC107_PIC1_NO_BUS_WIDTH_CHECK	0x000080
#define MPC107_PIC1_TEA_EN		0x000400
#define MPC107_PIC1_MCP_EN		0x000800
#define MPC107_PIC1_FLASH_WR_EN		0x001000
#define MPC107_PIC1_CF_LBA_EN		0x002000
#define MPC107_PIC1_CF_MP_ID		0x00c000
#define MPC107_PIC1_ADDRESS_MAP		0x010000
#define MPC107_PIC1_PROC_TYPE		0x050000
#define MPC107_PIC1_RCS0		0x100000
#define MPC107_PIC1_CF_BREAD_WS		0xc00000

#define MPC107_I2CADR	0x3000
#define MPC107_I2CFDR	0x3004
#define MPC107_I2CCR	0x3008
#define MPC107_I2CSR	0x300c
#define MPC107_I2CDR	0x3010

#define MPC107_I2C_CCR_MEN	0x80
#define MPC107_I2C_CCR_MIEN	0x40
#define MPC107_I2C_CCR_MSTA	0x20
#define MPC107_I2C_CCR_MTX	0x10
#define MPC107_I2C_CCR_TXAK	0x08
#define MPC107_I2C_CCR_RSTA	0x04

#define MPC107_I2C_CSR_MCF	0x80
#define MPC107_I2C_CSR_MAAS	0x40
#define MPC107_I2C_CSR_MBB	0x20
#define MPC107_I2C_CSR_MAL	0x10
#define MPC107_I2C_CSR_SRW	0x04
#define MPC107_I2C_CSR_MIF	0x02
#define MPC107_I2C_CSR_RXAK	0x01

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
