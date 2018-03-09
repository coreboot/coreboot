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
#include "libbdk-arch/bdk-csrs-mio_tws.h"

#define RECOVERY_UDELAY  5
#define RECOVERY_CLK_CNT 9
#define ARBLOST_UDELAY   5000 /* 5ms */

/* This code is an optional part of the BDK. It is only linked in
    if BDK_REQUIRE() needs it */
BDK_REQUIRE_DEFINE(TWSI);

/**
 * Initialize the TWSI blocks. This just sets the clock rate.
 * Many times stuff will work without calling this, but some
 * TWSI devices will fail. This is normally called automatically
 * in bdk-init-main.c.
 *
 * @return Zero on success, negative on failure
 */
int bdk_twsix_initialize(bdk_node_t node)
{
    const int TWSI_BUS_FREQ = 100000;   /* 100 KHz */
    const int TWSI_THP = 24;    	/* TCLK half period (default 24) */
    const int io_clock_hz = bdk_clock_get_rate(node, BDK_CLOCK_SCLK);
    int N_divider;
    int M_divider;

    /* Set the TWSI clock to a conservative TWSI_BUS_FREQ.  Compute the
	clocks M divider based on the SCLK.
	TWSI freq = (core freq) / (20 x (M+1) x (thp+1) x 2^N)
	M = ((core freq) / (20 x (TWSI freq) x (thp+1) x 2^N)) - 1 */
    for (N_divider = 0; N_divider < 8; N_divider++)
    {
	M_divider = (io_clock_hz / (20 * TWSI_BUS_FREQ * (TWSI_THP + 1) * (1 << N_divider))) - 1;
	if (M_divider < 16)
	    break;
    }

    BDK_CSR_DEFINE(sw_twsi, BDK_MIO_TWSX_SW_TWSI(bus));
    sw_twsi.u = 0;
    sw_twsi.s.v = 1;       /* Clear valid bit */
    sw_twsi.s.op = 0x6;    /* See EOP field */
    sw_twsi.s.r = 0;       /* Select CLKCTL when R = 0 */
    sw_twsi.s.eop_ia = 3;  /* R=0 selects CLKCTL, R=1 selects STAT */
    sw_twsi.s.data = ((M_divider & 0xf) << 3) | ((N_divider & 0x7) << 0);

    int num_busses = 2;
    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX))
	num_busses = 6;

    for (int bus = 0; bus < num_busses; bus++)
    {
	/* Only init non-slave ports */
	BDK_CSR_INIT(state, node, BDK_MIO_TWSX_SW_TWSI(bus));
	if (!state.s.slonly)
	    BDK_CSR_WRITE(node, BDK_MIO_TWSX_SW_TWSI(bus), sw_twsi.u);
    }
    return 0;
}

/**
 * Do a twsi bus recovery in the case when the last transaction
 * on the bus has been left unfinished.
 *
 * @param twsi_id   which TWSI bus to use
 */
static void bdk_twsix_recover_bus(bdk_node_t node, int twsi_id)
{
    /* read TWSX_INT */
    BDK_CSR_INIT(twsx_int, node, BDK_MIO_TWSX_INT(twsi_id));

    for (int i = 0; i < RECOVERY_CLK_CNT * 2; i++)
    {
	if (!twsx_int.s.scl_ovr)
	{
	    /* SCL shouldn't be low here */
	    if (!twsx_int.s.scl)
	    {
		bdk_error("N%d.TWSI%d: SCL is stuck low\n", node, twsi_id);
		return;
	    }

	    /* Break if SDA is high */
	    if (twsx_int.s.sda)
		break;
	}

	twsx_int.s.scl_ovr = !twsx_int.s.scl_ovr;
	BDK_CSR_WRITE(node, BDK_MIO_TWSX_INT(twsi_id), twsx_int.u);
	bdk_wait_usec(RECOVERY_UDELAY);
    }

    /*
     * Generate STOP condition using the register overrides
     * in order to move the higher level controller out of
     * the bad state. This is a workaround for the TWSI hardware.
     */
    twsx_int.s.scl_ovr = 1;
    twsx_int.s.sda_ovr = 1;
    BDK_CSR_WRITE(node, BDK_MIO_TWSX_INT(twsi_id), twsx_int.u);
    bdk_wait_usec(RECOVERY_UDELAY);
    twsx_int.s.scl_ovr = 0;
    BDK_CSR_WRITE(node, BDK_MIO_TWSX_INT(twsi_id), twsx_int.u);
    bdk_wait_usec(RECOVERY_UDELAY);
    twsx_int.s.sda_ovr = 0;
    BDK_CSR_WRITE(node, BDK_MIO_TWSX_INT(twsi_id), twsx_int.u);
}

/**
 * Do a twsi read from a 7 bit device address using an (optional)
 * internal address. Up to 4 bytes can be read at a time.
 *
 * @param twsi_id   which TWSI bus to use
 * @param dev_addr  Device address (7 bit)
 * @param internal_addr
 *      	    Internal address.  Can be 0, 1 or 2 bytes in width
 * @param num_bytes Number of data bytes to read (1-4)
 * @param ia_width_bytes
 *      	    Internal address size in bytes (0, 1, or 2)
 *
 * @return Read data, or -1 on failure
 */
int64_t bdk_twsix_read_ia(bdk_node_t node, int twsi_id, uint8_t dev_addr, uint16_t internal_addr, int num_bytes, int ia_width_bytes)
{
    bdk_mio_twsx_sw_twsi_t sw_twsi_val;
    bdk_mio_twsx_sw_twsi_ext_t twsi_ext;
    int retry_limit = 5;

    if (num_bytes < 1 || num_bytes > 4 || ia_width_bytes < 0 || ia_width_bytes > 2)
	return -1;
retry:
    twsi_ext.u = 0;
    sw_twsi_val.u = 0;
    sw_twsi_val.s.v = 1;
    sw_twsi_val.s.r = 1;
    sw_twsi_val.s.sovr = 1;
    sw_twsi_val.s.size = num_bytes - 1;
    sw_twsi_val.s.addr = dev_addr;

    if (ia_width_bytes > 0)
    {
	sw_twsi_val.s.op = 1;
	sw_twsi_val.s.ia = (internal_addr >> 3) & 0x1f;
	sw_twsi_val.s.eop_ia = internal_addr & 0x7;
	if (ia_width_bytes == 2)
	{
	    sw_twsi_val.s.eia = 1;
	    twsi_ext.s.ia = internal_addr >> 8;
	    BDK_CSR_WRITE(node, BDK_MIO_TWSX_SW_TWSI_EXT(twsi_id), twsi_ext.u);
	}
    }

    BDK_CSR_WRITE(node, BDK_MIO_TWSX_SW_TWSI(twsi_id), sw_twsi_val.u);
    if (BDK_CSR_WAIT_FOR_FIELD(node, BDK_MIO_TWSX_SW_TWSI(twsi_id), v, ==, 0, 10000))
    {
	bdk_warn("N%d.TWSI%d: Timeout waiting for read to complete...start recovering process\n",
		 node, twsi_id);
	/* perform bus recovery */
	bdk_twsix_recover_bus(node, twsi_id);
	if (retry_limit-- > 0)
	    goto retry;

	bdk_error("N%d.TWSI%d: Timeout waiting for operation to complete\n", node, twsi_id);
	return -1;
    }
    sw_twsi_val.u = BDK_CSR_READ(node, BDK_MIO_TWSX_SW_TWSI(twsi_id));
    if (!sw_twsi_val.s.r)
    {
	/* Check the reason for the failure.  We may need to retry to handle multi-master
	** configurations.
	** Lost arbitration : 0x38, 0x68, 0xB0, 0x78
	** Core busy as slave: 0x80, 0x88, 0xA0, 0xA8, 0xB8, 0xC0, 0xC8
	*/
	if (sw_twsi_val.s.data == 0x38
	    || sw_twsi_val.s.data == 0x68
	    || sw_twsi_val.s.data == 0xB0
	    || sw_twsi_val.s.data == 0x78
	    || sw_twsi_val.s.data == 0x80
	    || sw_twsi_val.s.data == 0x88
	    || sw_twsi_val.s.data == 0xA0
	    || sw_twsi_val.s.data == 0xA8
	    || sw_twsi_val.s.data == 0xB8
	    || sw_twsi_val.s.data == 0xC8)
	{
	    /*
	     * One of the arbitration lost conditions is recognized.
	     * The TWSI hardware has switched to the slave mode and
	     * expects the STOP condition on the bus.
	     * Make a delay before next retry.
	     */
	    bdk_wait_usec(ARBLOST_UDELAY);
	    if (retry_limit-- > 0)
		goto retry;
	}
	/* For all other errors, return an error code */
	return -1;
    }

    return (sw_twsi_val.s.data & (0xFFFFFFFF >> (32 - num_bytes*8)));
}


/**
 * Write 1-8 bytes to a TWSI device using an internal address.
 *
 * @param twsi_id   which TWSI interface to use
 * @param dev_addr  TWSI device address (7 bit only)
 * @param internal_addr
 *      	    TWSI internal address (0, 8, or 16 bits)
 * @param num_bytes Number of bytes to write (1-8)
 * @param ia_width_bytes
 *      	    internal address width, in bytes (0, 1, 2)
 * @param data      Data to write.  Data is written MSB first on the twsi bus, and
 *      	    only the lower num_bytes bytes of the argument are valid.  (If
 *      	    a 2 byte write is done, only the low 2 bytes of the argument is
 *      	    used.
 *
 * @return Zero on success, -1 on error
 */
int bdk_twsix_write_ia(bdk_node_t node, int twsi_id, uint8_t dev_addr, uint16_t internal_addr, int num_bytes, int ia_width_bytes, uint64_t data)
{
    bdk_mio_twsx_sw_twsi_t sw_twsi_val;
    bdk_mio_twsx_sw_twsi_ext_t twsi_ext;
    int retry_limit = 5;
    int to;

    if (num_bytes < 1 || num_bytes > 8 || ia_width_bytes < 0 || ia_width_bytes > 2)
	return -1;

retry:
    twsi_ext.u = 0;
    sw_twsi_val.u = 0;
    sw_twsi_val.s.v = 1;
    sw_twsi_val.s.sovr = 1;
    sw_twsi_val.s.size = num_bytes - 1;
    sw_twsi_val.s.addr = dev_addr;
    sw_twsi_val.s.data = 0xFFFFFFFF & data;

    if (ia_width_bytes > 0)
    {
	sw_twsi_val.s.op = 1;
	sw_twsi_val.s.ia = (internal_addr >> 3) & 0x1f;
	sw_twsi_val.s.eop_ia = internal_addr & 0x7;
    }
    if (ia_width_bytes == 2)
    {
	sw_twsi_val.s.eia = 1;
	twsi_ext.s.ia = internal_addr >> 8;
    }
    if (num_bytes > 4)
	twsi_ext.s.data = data >> 32;

    BDK_CSR_WRITE(node, BDK_MIO_TWSX_SW_TWSI_EXT(twsi_id), twsi_ext.u);
    BDK_CSR_WRITE(node, BDK_MIO_TWSX_SW_TWSI(twsi_id), sw_twsi_val.u);
    if (BDK_CSR_WAIT_FOR_FIELD(node, BDK_MIO_TWSX_SW_TWSI(twsi_id), v, ==, 0, 10000))
    {
	bdk_warn("N%d.TWSI%d: Timeout waiting for write to complete...start recovering process\n",
		 node, twsi_id);
	/* perform bus recovery */
	bdk_twsix_recover_bus(node, twsi_id);
	if (retry_limit-- > 0)
	    goto retry;

	// After retry but still not success, report error and return
	bdk_error("N%d.TWSI%d: Timeout waiting for operation to complete\n", node, twsi_id);
	return -1;
    }

    /* Poll until reads succeed, or polling times out */
    to = 100;
    while (to-- > 0)
    {
	if (bdk_twsix_read_ia(node, twsi_id, dev_addr, 0, 1, 0) >= 0)
	    break;
    }
    if (to <= 0)
	return -1;

    return 0;
}
