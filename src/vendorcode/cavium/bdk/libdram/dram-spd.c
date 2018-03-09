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
#include <bdk.h>
#include "dram-internal.h"

#include <bdk-minimal.h>
#include <libbdk-arch/bdk-warn.h>
#include <libbdk-hal/bdk-config.h>
#include <libbdk-hal/bdk-twsi.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>

/**
 * Read the entire contents of a DIMM SPD and store it in the device tree. The
 * current DRAM config is also updated, so future SPD accesses used the cached
 * copy.
 *
 * @param node   Node the DRAM config is for
 * @param cfg    Current DRAM config. Updated with SPD data
 * @param lmc    LMC to read DIMM for
 * @param dimm   DIMM slot for SPD to read
 *
 * @return Zero on success, negative on failure
 */
static uint8_t spd_bufs[4 * 256];	/* FIXME(dhendrix): storage for SPD buffers, assume DDR4 */
int read_entire_spd(bdk_node_t node, dram_config_t *cfg, int lmc, int dimm)
{
    /* FIXME(dhendrix): hack to get around using allocated mem */
    assert(dimm < 4);

    /* If pointer to data is provided, use it, otherwise read from SPD over twsi */
    if (cfg->config[lmc].dimm_config_table[dimm].spd_ptr)
        return 0;
    if (!cfg->config[lmc].dimm_config_table[dimm].spd_addr)
        return -1;

    /* Figure out how to access the SPD */
    int spd_addr = cfg->config[lmc].dimm_config_table[dimm].spd_addr;
    int bus = spd_addr >> 12;
    int address = spd_addr & 0x7f;

    /* Figure out the size we will read */
    int64_t dev_type = bdk_twsix_read_ia(node, bus, address, DDR4_SPD_KEY_BYTE_DEVICE_TYPE, 1, 1);
    if (dev_type < 0)
        return -1; /* No DIMM */
    // FIXME: prudolph: Nobody needs 512 byte SPDs...
    //int spd_size = (dev_type == 0x0c) ? 512 : 256;
    int spd_size = 256;

    /*
     * FIXME: Assume DIMM doesn't support
     * 'Hybrid Module Extended Function Parameters' aka only 256 Byte SPD,
     * as the code below is broken ...
     */
    assert(spd_size == 256);
    uint8_t *spd_buf = &spd_bufs[dimm * 256];
    uint32_t *ptr = (uint32_t *)spd_buf;

    for (int bank = 0; bank < (spd_size >> 8); bank++)
    {
        /* this should only happen for DDR4, which has a second bank of 256 bytes */
        if (bank)
            bdk_twsix_write_ia(node, bus, 0x36 | bank, 0, 2, 1, 0);
        int bank_size = 256;
        for (int i = 0; i < bank_size; i += 4)
        {
            int64_t data = bdk_twsix_read_ia(node, bus, address, i, 4, 1);
            if (data < 0)
            {
                free(spd_buf);
                bdk_error("Failed to read SPD data at 0x%x\n", i + (bank << 8));
                /* Restore the bank to zero */
                if (bank)
                    bdk_twsix_write_ia(node, bus, 0x36 | 0, 0, 2, 1, 0);
                return -1;
            }
            else
                *ptr++ = bdk_be32_to_cpu(data);
        }
        /* Restore the bank to zero */
        if (bank)
            bdk_twsix_write_ia(node, bus, 0x36 | 0, 0, 2, 1, 0);
    }

    /* Store the SPD in the device tree */
    /* FIXME(dhendrix): No need for this? cfg gets updated, so the caller
     * (libdram_config()) has what it needs. */
//    bdk_config_set_blob(spd_size, spd_buf, BDK_CONFIG_DDR_SPD_DATA, dimm, lmc, node);
    cfg->config[lmc].dimm_config_table[dimm].spd_ptr = (void*)spd_buf;

    return 0;
}

/* Read an DIMM SPD value, either using TWSI to read it from the DIMM, or
 * from a provided array.
 */
int read_spd(bdk_node_t node, const dimm_config_t *dimm_config, int spd_field)
{
    /* If pointer to data is provided, use it, otherwise read from SPD over twsi */
    if (dimm_config->spd_ptr)
        return dimm_config->spd_ptr[spd_field];
    else if (dimm_config->spd_addr)
    {
        int data;
        int bus = dimm_config->spd_addr >> 12;
        int address = dimm_config->spd_addr & 0x7f;

	/* this should only happen for DDR4, which has a second bank of 256 bytes */
	int bank = (spd_field >> 8) & 1;
	if (bank) {
	    bdk_twsix_write_ia(node, bus, 0x36 | bank, 0, 2, 1, 0);
	    spd_field %= 256;
	}

        data = bdk_twsix_read_ia(node, bus, address, spd_field, 1, 1);

        /* Restore the bank to zero */
        if (bank) {
            bdk_twsix_write_ia(node, bus, 0x36 | 0, 0, 2, 1, 0);
        }
        
        return data;
    }
    else
        return -1;
}

static uint16_t ddr3_crc16(uint8_t *ptr, int count)
{
    /* From DDR3 spd specification */
    int crc, i;
    crc = 0;
    while (--count >= 0)
    {
        crc = crc ^ (int)*ptr++ << 8;
        for (i = 0; i < 8; ++i)
            if (crc & 0x8000)
                crc = crc << 1 ^ 0x1021;
            else
                crc = crc << 1;
    }
    return crc & 0xFFFF;
}

static int validate_spd_checksum_ddr3(bdk_node_t node, int twsi_addr, int silent)
{
    uint8_t spd_data[128];
    int crc_bytes = 126;
    uint16_t crc_comp;
    int i;
    int rv;
    int ret = 1;
    for (i = 0; i < 128; i++)
    {
        rv = bdk_twsix_read_ia(node, twsi_addr >> 12, twsi_addr & 0x7f, i, 1, 1);
        if (rv < 0)
            return 0;   /* TWSI read error */
        spd_data[i] = (uint8_t)rv;
    }
    /* Check byte 0 to see how many bytes checksum is over */
    if (spd_data[0] & 0x80)
        crc_bytes = 117;

    crc_comp = ddr3_crc16(spd_data, crc_bytes);

    if (spd_data[DDR3_SPD_CYCLICAL_REDUNDANCY_CODE_LOWER_NIBBLE] != (crc_comp & 0xff) ||
        spd_data[DDR3_SPD_CYCLICAL_REDUNDANCY_CODE_UPPER_NIBBLE] != (crc_comp >> 8))
    {
        if (!silent) {
            printf("DDR3 SPD CRC error, spd addr: 0x%x, calculated crc: 0x%04x, read crc: 0x%02x%02x\n",
		   twsi_addr, crc_comp,
		   spd_data[DDR3_SPD_CYCLICAL_REDUNDANCY_CODE_UPPER_NIBBLE],
		   spd_data[DDR3_SPD_CYCLICAL_REDUNDANCY_CODE_LOWER_NIBBLE]);
	}
        ret = 0;
    }
    return ret;
}

static int validate_spd_checksum(bdk_node_t node, int twsi_addr, int silent)
{
    int rv;

    debug_print("Validating DIMM at address 0x%x\n", twsi_addr);

    if (!twsi_addr) return 1; /* return OK if we are not doing real DIMMs */

    /* Look up module type to determine if DDR3 or DDR4 */
    rv = bdk_twsix_read_ia(node, twsi_addr >> 12, twsi_addr & 0x7f, 2, 1, 1);

    if (rv >= 0xB && rv <= 0xC) /* this is DDR3 or DDR4, do same */
        return validate_spd_checksum_ddr3(node, twsi_addr, silent);

    if (!silent)
        printf("Unrecognized DIMM type: 0x%x at spd address: 0x%x\n",
	       rv, twsi_addr);

    return 0;
}


int validate_dimm(bdk_node_t node, const dimm_config_t *dimm_config)
{
    int spd_addr;

    spd_addr = dimm_config->spd_addr;

    debug_print("Validating dimm spd addr: 0x%02x spd ptr: %x\n",
		spd_addr, dimm_config->spd_ptr);

    // if the slot is not possible
    if (!spd_addr && !dimm_config->spd_ptr)
        return -1;

    {
        int val0, val1;
        int ddr_type = get_ddr_type(node, dimm_config);

        switch (ddr_type)
        {
            case DDR3_DRAM:              /* DDR3 */
	    case DDR4_DRAM:              /* DDR4 */

		debug_print("Validating DDR%d DIMM\n", ((dimm_type >> 2) & 3) + 1);

#define DENSITY_BANKS DDR4_SPD_DENSITY_BANKS           // same for DDR3 and DDR4
#define ROW_COL_BITS  DDR4_SPD_ADDRESSING_ROW_COL_BITS // same for DDR3 and DDR4

		val0 = read_spd(node, dimm_config, DENSITY_BANKS);
		val1 = read_spd(node, dimm_config, ROW_COL_BITS);
		if (val0 < 0 && val1 < 0) {
		    debug_print("Error reading SPD for DIMM\n");
		    return 0; /* Failed to read dimm */
		}
		if (val0 == 0xff && val1 == 0xff) {
		    ddr_print("Blank or unreadable SPD for DIMM\n");
		    return 0; /* Blank SPD or otherwise unreadable device */
		}

		/* Don't treat bad checksums as fatal. */
		validate_spd_checksum(node, spd_addr, 0);
		break;

	    case 0x00:              /* Terminator detected. Fail silently. */
                return 0;

            default:
		debug_print("Unknown DIMM type 0x%x for DIMM @ 0x%x\n",
			     dimm_type, dimm_config->spd_addr);
                return 0;      /* Failed to read dimm */
        }
    }

    return 1;
}

int get_dimm_part_number(char *buffer, bdk_node_t node,
			   const dimm_config_t *dimm_config,
			   int ddr_type)
{
    int i;
    int c;
    int skipping = 1;
    int strlen   = 0;

#define PART_LIMIT(t)  (((t) == DDR4_DRAM) ? 19 : 18)
#define PART_NUMBER(t) (((t) == DDR4_DRAM) ? DDR4_SPD_MODULE_PART_NUMBER : DDR3_SPD_MODULE_PART_NUMBER)

    int limit  = PART_LIMIT(ddr_type);
    int offset = PART_NUMBER(ddr_type);

    for (i = 0; i < limit; ++i) {

	c = (read_spd(node, dimm_config, offset+i) & 0xff);
	if (c == 0) // any null, we are done
	    break;

	/* Skip leading spaces. */
	if (skipping) {
	    if (isspace(c))
		continue;
	    else
		skipping = 0;
	}

	/* Put non-null non-leading-space-skipped char into buffer */
	buffer[strlen] = c;
	++strlen;
    }

    if (strlen > 0) {
	i = strlen - 1; // last char put into buf
	while (i >= 0 && isspace((int)buffer[i])) { // still in buf and a space
	    --i;
	    --strlen;
	}
    }
    buffer[strlen] = 0;       /* Insure that the string is terminated */

    return strlen;
}

uint32_t get_dimm_serial_number(bdk_node_t node, const dimm_config_t *dimm_config, int ddr_type)
{
    uint32_t serial_number = 0;
    int offset;

#define SERIAL_NUMBER(t) (((t) == DDR4_DRAM) ? DDR4_SPD_MODULE_SERIAL_NUMBER : DDR3_SPD_MODULE_SERIAL_NUMBER)

    offset = SERIAL_NUMBER(ddr_type);

    for (int i = 0, j = 24; i < 4; ++i, j -= 8) {
        serial_number |= ((read_spd(node, dimm_config, offset + i) & 0xff) << j);
    }

    return serial_number;
}

static uint32_t get_dimm_checksum(bdk_node_t node, const dimm_config_t *dimm_config, int ddr_type)
{
    uint32_t spd_chksum;

#define LOWER_NIBBLE(t) (((t) == DDR4_DRAM) ? DDR4_SPD_CYCLICAL_REDUNDANCY_CODE_LOWER_NIBBLE : DDR3_SPD_CYCLICAL_REDUNDANCY_CODE_LOWER_NIBBLE)
#define UPPER_NIBBLE(t) (((t) == DDR4_DRAM) ? DDR4_SPD_CYCLICAL_REDUNDANCY_CODE_UPPER_NIBBLE : DDR3_SPD_CYCLICAL_REDUNDANCY_CODE_UPPER_NIBBLE)

    spd_chksum  =   0xff & read_spd(node, dimm_config, LOWER_NIBBLE(ddr_type));
    spd_chksum |= ((0xff & read_spd(node, dimm_config, UPPER_NIBBLE(ddr_type))) << 8);

    return spd_chksum;
}

static
void report_common_dimm(bdk_node_t node, const dimm_config_t *dimm_config, int dimm,
			const char **dimm_types, int ddr_type, const char *volt_str,
                        int ddr_interface_num, int num_ranks, int dram_width, int dimm_size_mb)
{
    int spd_ecc;
    unsigned spd_module_type;
    uint32_t serial_number;
    char part_number[21]; /* 20 bytes plus string terminator is big enough for either */
    const char *sn_str;

    spd_module_type = get_dimm_module_type(node, dimm_config, ddr_type);
    spd_ecc = get_dimm_ecc(node, dimm_config, ddr_type);

    (void) get_dimm_part_number(part_number, node, dimm_config, ddr_type);

    serial_number = get_dimm_serial_number(node, dimm_config, ddr_type);
    if ((serial_number != 0) && (serial_number != 0xffffffff)) {
        sn_str = "s/n";
    } else {
        serial_number = get_dimm_checksum(node, dimm_config, ddr_type);
        sn_str = "chksum";
    }

    // FIXME: add output of DIMM rank/width, as in: 2Rx4, 1Rx8, etc
    printf("N%d.LMC%d.DIMM%d: %d MB, DDR%d %s %dRx%d %s, p/n: %s, %s: %u, %s\n",
           node, ddr_interface_num, dimm, dimm_size_mb, ddr_type,
           dimm_types[spd_module_type], num_ranks, dram_width,
           (spd_ecc ? "ECC" : "non-ECC"), part_number,
           sn_str, serial_number, volt_str);
}

const char *ddr3_dimm_types[16] = {
    /* 0000 */ "Undefined",
    /* 0001 */ "RDIMM",
    /* 0010 */ "UDIMM",
    /* 0011 */ "SO-DIMM",
    /* 0100 */ "Micro-DIMM",
    /* 0101 */ "Mini-RDIMM",
    /* 0110 */ "Mini-UDIMM",
    /* 0111 */ "Mini-CDIMM",
    /* 1000 */ "72b-SO-UDIMM",
    /* 1001 */ "72b-SO-RDIMM",
    /* 1010 */ "72b-SO-CDIMM"
    /* 1011 */ "LRDIMM",
    /* 1100 */ "16b-SO-DIMM",
    /* 1101 */ "32b-SO-DIMM",
    /* 1110 */ "Reserved",
    /* 1111 */ "Reserved"
};

static
void report_ddr3_dimm(bdk_node_t node, const dimm_config_t *dimm_config,
                      int dimm, int ddr_interface_num, int num_ranks,
                      int dram_width, int dimm_size_mb)
{
    int spd_voltage;
    const char *volt_str;

    spd_voltage = read_spd(node, dimm_config, DDR3_SPD_NOMINAL_VOLTAGE);
    if ((spd_voltage == 0) || (spd_voltage & 3))
        volt_str = "1.5V";
    if (spd_voltage & 2)
        volt_str = "1.35V";
    if (spd_voltage & 4)
        volt_str = "1.2xV";

    report_common_dimm(node, dimm_config, dimm, ddr3_dimm_types,
                       DDR3_DRAM, volt_str, ddr_interface_num,
                       num_ranks, dram_width, dimm_size_mb);
}

const char *ddr4_dimm_types[16] = {
    /* 0000 */ "Extended",
    /* 0001 */ "RDIMM",
    /* 0010 */ "UDIMM",
    /* 0011 */ "SO-DIMM",
    /* 0100 */ "LRDIMM",
    /* 0101 */ "Mini-RDIMM",
    /* 0110 */ "Mini-UDIMM",
    /* 0111 */ "Reserved",
    /* 1000 */ "72b-SO-RDIMM",
    /* 1001 */ "72b-SO-UDIMM",
    /* 1010 */ "Reserved",
    /* 1011 */ "Reserved",
    /* 1100 */ "16b-SO-DIMM",
    /* 1101 */ "32b-SO-DIMM",
    /* 1110 */ "Reserved",
    /* 1111 */ "Reserved"
};

static
void report_ddr4_dimm(bdk_node_t node, const dimm_config_t *dimm_config,
                      int dimm, int ddr_interface_num, int num_ranks,
                      int dram_width, int dimm_size_mb)
{
    int spd_voltage;
    const char *volt_str;

    spd_voltage = read_spd(node, dimm_config, DDR4_SPD_MODULE_NOMINAL_VOLTAGE);
    if ((spd_voltage == 0x01) || (spd_voltage & 0x02))
	volt_str = "1.2V";
    if ((spd_voltage == 0x04) || (spd_voltage & 0x08))
	volt_str = "TBD1 V";
    if ((spd_voltage == 0x10) || (spd_voltage & 0x20))
	volt_str = "TBD2 V";

    report_common_dimm(node, dimm_config, dimm, ddr4_dimm_types,
                       DDR4_DRAM, volt_str, ddr_interface_num,
                       num_ranks, dram_width, dimm_size_mb);
}

void report_dimm(bdk_node_t node, const dimm_config_t *dimm_config,
                 int dimm, int ddr_interface_num, int num_ranks,
                 int dram_width, int dimm_size_mb)
{
        int ddr_type;

        /* ddr_type only indicates DDR4 or DDR3 */
        ddr_type = get_ddr_type(node, dimm_config);

        if (ddr_type == DDR4_DRAM)
	    report_ddr4_dimm(node, dimm_config, dimm, ddr_interface_num,
                             num_ranks, dram_width, dimm_size_mb);
        else
	    report_ddr3_dimm(node, dimm_config, dimm, ddr_interface_num,
                             num_ranks, dram_width, dimm_size_mb);
}

static int
get_ddr4_spd_speed(bdk_node_t node, const dimm_config_t *dimm_config)
{
    int spdMTB = 125;
    int spdFTB = 1;

    int tCKAVGmin
	  = spdMTB *        read_spd(node, dimm_config, DDR4_SPD_MINIMUM_CYCLE_TIME_TCKAVGMIN)
	  + spdFTB * (signed char) read_spd(node, dimm_config, DDR4_SPD_MIN_CYCLE_TIME_FINE_TCKAVGMIN);

    return pretty_psecs_to_mts(tCKAVGmin);
}

static int
get_ddr3_spd_speed(bdk_node_t node, const dimm_config_t *dimm_config)
{
    int spd_mtb_dividend = 0xff & read_spd(node, dimm_config, DDR3_SPD_MEDIUM_TIMEBASE_DIVIDEND);
    int spd_mtb_divisor  = 0xff & read_spd(node, dimm_config, DDR3_SPD_MEDIUM_TIMEBASE_DIVISOR);
    int spd_tck_min      = 0xff & read_spd(node, dimm_config, DDR3_SPD_MINIMUM_CYCLE_TIME_TCKMIN);

    short ftb_Dividend     = read_spd(node, dimm_config, DDR3_SPD_FINE_TIMEBASE_DIVIDEND_DIVISOR) >> 4;
    short ftb_Divisor      = read_spd(node, dimm_config, DDR3_SPD_FINE_TIMEBASE_DIVIDEND_DIVISOR) & 0xf;

    ftb_Divisor      = (ftb_Divisor == 0) ? 1 : ftb_Divisor; /* Make sure that it is not 0 */

    int mtb_psec     = spd_mtb_dividend * 1000 / spd_mtb_divisor;
    int tCKmin       = mtb_psec * spd_tck_min;
    tCKmin          += ftb_Dividend *
        		(signed char) read_spd(node, dimm_config, DDR3_SPD_MINIMUM_CYCLE_TIME_FINE_TCKMIN)
        		/ ftb_Divisor;

    return pretty_psecs_to_mts(tCKmin);
}

static int
speed_bin_down(int speed)
{
    if (speed == 2133)
        return 1866;
    else if (speed == 1866)
        return 1600;
    else
        return speed;
}

int
dram_get_default_spd_speed(bdk_node_t node, const ddr_configuration_t *ddr_config)
{
    int lmc, dimm;
    int speed, ret_speed = 0;
    int ddr_type = get_ddr_type(node, &ddr_config[0].dimm_config_table[0]);
    int dimm_speed[8], dimm_count = 0;
    int dimms_per_lmc = 0;

    for (lmc = 0; lmc < 4; lmc++) {
        for (dimm = 0; dimm < DDR_CFG_T_MAX_DIMMS; dimm++) {
            const dimm_config_t *dimm_config = &ddr_config[lmc].dimm_config_table[dimm];
            if (/*dimm_config->spd_addr ||*/ dimm_config->spd_ptr)
            {
                speed = (ddr_type == DDR4_DRAM)
                    ? get_ddr4_spd_speed(node, dimm_config)
                    : get_ddr3_spd_speed(node, dimm_config);
                //printf("N%d.LMC%d.DIMM%d: SPD speed %d\n", node, lmc, dimm, speed);
                dimm_speed[dimm_count] = speed;
                dimm_count++;
                if (lmc == 0)
                    dimms_per_lmc++;
            }
        }
    }

    // all DIMMs must be same speed
    speed = dimm_speed[0];
    for (dimm = 1; dimm < dimm_count; dimm++) {
        if (dimm_speed[dimm] != speed) {
            ret_speed = -1;
            goto finish_up;
        }
    }

    // if 2400 or greater, use 2133
    if (speed >= 2400)
        speed = 2133;

    // use next speed down if 2DPC...
    if (dimms_per_lmc > 1)
        speed = speed_bin_down(speed);

    // Update the in memory config to match the automatically calculated speed
    bdk_config_set_int(speed, BDK_CONFIG_DDR_SPEED, node);

    // do filtering for our jittery PLL
    if (speed == 2133)
        speed = 2100;
    else if (speed == 1866)
        speed = 1880;

    // OK, return what we have...
    ret_speed = mts_to_hertz(speed);

 finish_up:
    //printf("N%d: Returning default SPD speed %d\n", node, ret_speed);
    return ret_speed;
}
