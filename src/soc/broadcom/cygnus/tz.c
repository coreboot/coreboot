/*
 * Copyright (C) 2015 Broadcom Corporation
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <arch/io.h>
#include <soc/tz.h>

#define TZPC_TZPCR0SIZE				0x18034000
#define TZPC_TZPCR0SIZE_MASK			0x000003ff

#define TZPC_TZPCDECPROT0SET			0x18034804
#define TZPC_TZPCDECPROT0CLR			0x18034808
#define TZPC_TZPCDECPROT1SET			0x18034810
#define TZPC_TZPCDECPROT1CLR			0x18034814
#define TZPC_TZPCDECPROT2SET			0x1803481c
#define TZPC_TZPCDECPROT2CLR			0x18034820

#define TZPCDECPROT0_MASK			0x000000FF
#define TZPCDECPROT1_MASK			0x000000FF
#define TZPCDECPROT2_MASK			0x000000FF

#define AXIIC_Ihost_acp_security		0x1a000008
#define AXIIC_PCIe0_s0_security			0x1a000010
#define AXIIC_PCIe1_s0_security			0x1a000014
#define AXIIC_APBY_s0_security			0x1a00002c
#define AXIIC_APBZ_s0_security			0x1a000030
#define AXIIC_APBX_s0_security			0x1a000034
#define AXIIC_ihost_s0_security			0x1a000038
#define AXIIC_A9jtag_s0_security		0x1a00003c
#define AXIIC_APB_W1_security			0x1a000040
#define AXIIC_APB_W2_security			0x1a000044
#define AXIIC_APB_W3_security			0x1a000048
#define AXIIC_APB_W4_security			0x1a00004c
#define AXIIC_APBR_s0_security			0x1a00006c
#define AXIIC_APBS_s0_security			0x1a000070
#define AXIIC_CMICd_s0_security			0x1a000074
#define AXIIC_mhost0_s0_security		0x1a000078
#define AXIIC_mhost1_s0_security		0x1a00007c
#define AXIIC_Crypto_s0_security		0x1a000080
#define AXIIC_DMU_s0_security			0x1a000084
#define AXIIC_ext_s0_security			0x1a000088
#define AXIIC_ext_s1_security			0x1a00008c

#define AXIIC_APBY_s0_security_MASK		0x00003f1f
#define AXIIC_APBZ_s0_security_MASK		0x0000003f
#define AXIIC_APBX_s0_security_MASK		0x0000cfff
#define AXIIC_ext_s0_security_MASK		0xffffffff
#define AXIIC_ext_s1_security_MASK		0xffffffff
#define AXIIC_APBR_s0_security_MASK		0x0000436d
#define AXIIC_APBS_s0_security_MASK		0x000057ee
#define AXIIC_APB_W1_security_MASK		0x0000ffff
#define AXIIC_APB_W2_security_MASK		0x0000000f
#define AXIIC_APB_W3_security_MASK		0x00003fff
#define AXIIC_APB_W4_security_MASK		0x0000007f

/*
 * Note: the order need to match corresponding definitions for
 *       non virtual slave slave_vector in tz.h
 */
static uint32_t non_virtual_slave_regs[] = {
	AXIIC_Ihost_acp_security,
	AXIIC_PCIe0_s0_security,
	AXIIC_PCIe1_s0_security,
	AXIIC_ihost_s0_security,
	AXIIC_A9jtag_s0_security,
	AXIIC_CMICd_s0_security,
	AXIIC_mhost0_s0_security,
	AXIIC_mhost1_s0_security,
	AXIIC_Crypto_s0_security,
	AXIIC_DMU_s0_security
};

/*
 * Set master security.
 * Use defines in tz.h for both parameters.
 */
void tz_set_masters_security(uint32_t masters, uint32_t ns_bit)
{
	uint32_t val;

	/* Check any TZPCDECPROT0 is set and then write to TZPCDECPROT0 */
	if (masters & TZPCDECPROT0_MASK) {
		val = masters & TZPCDECPROT0_MASK;
		if (ns_bit)
			write32((void *)TZPC_TZPCDECPROT0SET, val);
		else
			write32((void *)TZPC_TZPCDECPROT0CLR, val);
	}
	/* Check any TZPCDECPROT1 is set and then write to TZPCDECPROT1 */
	if ((masters >> 8) & TZPCDECPROT1_MASK) {
		val = (masters >> 8) & TZPCDECPROT1_MASK;
		if (ns_bit)
			write32((void *)TZPC_TZPCDECPROT1SET, val);
		else
			write32((void *)TZPC_TZPCDECPROT1CLR, val);
	}
	/* Check any TZPCDECPROT2 is set and then write to TZPCDECPROT2 */
	if ((masters >> 16) & TZPCDECPROT2_MASK) {
		val = (masters >> 16) & TZPCDECPROT2_MASK;
		if (ns_bit)
			write32((void *)TZPC_TZPCDECPROT2SET, val);
		else
			write32((void *)TZPC_TZPCDECPROT2CLR, val);
	}
}

/*
 * Set non virtual slave security.
 * Use defines in tz.h for both parameters.
 */
void tz_set_non_virtual_slaves_security(uint32_t slave_vector, uint32_t ns_bit)
{
	uint32_t i;
	uint32_t total = sizeof(non_virtual_slave_regs) /
			 sizeof(non_virtual_slave_regs[0]);
	uint32_t mask = ~(0xffffffff << total);

	ns_bit &= 0x1;
	slave_vector = slave_vector & mask;
	for (i = 0; i < total; i++) {
		if (slave_vector & (0x1 << i))
			write32((void *)(non_virtual_slave_regs[i]), ns_bit);
	}
}

/*
 * Set peripheral security.
 * Use defines in tz.h for both parameters.
 */
void tz_set_periph_security(uint32_t slave_vector, uint32_t ns_bit)
{
	uint32_t val;
	uint32_t mask_x = AXIIC_APBX_s0_security_MASK;
	uint32_t mask_y = AXIIC_APBY_s0_security_MASK;
	uint32_t tz_periphs_sec_status =
		(mask_x & read32((void *)AXIIC_APBX_s0_security)) |
		((mask_y & read32((void *)AXIIC_APBY_s0_security)) << 16);

	if (ns_bit == TZ_STATE_SECURE)
		tz_periphs_sec_status &= ~slave_vector;
	else
		tz_periphs_sec_status |= slave_vector;

	val = tz_periphs_sec_status & mask_x;
	write32((void *)AXIIC_APBX_s0_security, val);

	val = (tz_periphs_sec_status >> 16) & mask_y;
	write32((void *)AXIIC_APBY_s0_security, val);
}

/*
 * Set sec peripheral security.
 * Use defines in tz.h for both parameters.
 */
void tz_set_sec_periphs_security(uint32_t slave_vector, uint32_t ns_bit)
{
	uint32_t val;
	uint32_t mask = AXIIC_APBZ_s0_security_MASK;
	uint32_t tz_sec_periphs_sec_status =
		read32((void *)AXIIC_APBZ_s0_security);

	if (ns_bit == TZ_STATE_SECURE)
		tz_sec_periphs_sec_status &= ~slave_vector;
	else
		tz_sec_periphs_sec_status |= slave_vector;

	val = tz_sec_periphs_sec_status & mask;
	write32((void *)AXIIC_APBZ_s0_security, val);
}

/*
 * Set external slave security.
 * Use defines in tz.h for both parameters.
 */
void tz_set_ext_slaves_security(uint32_t slave_vector, uint32_t ns_bit)
{
	uint32_t val;
	uint32_t mask_s0 = AXIIC_ext_s0_security_MASK;
	uint32_t mask_s1 = AXIIC_ext_s1_security_MASK;
	uint32_t tz_ext_slaves_sec_status =
		(mask_s0 & read32((void *)AXIIC_ext_s0_security)) |
		((mask_s1 & read32((void *)AXIIC_ext_s0_security)) << 16);

	if (ns_bit == TZ_STATE_SECURE)
		tz_ext_slaves_sec_status &= ~slave_vector;
	else
		tz_ext_slaves_sec_status |= slave_vector;

	val = tz_ext_slaves_sec_status & mask_s0;
	write32((void *)AXIIC_ext_s0_security, val);

	val = (tz_ext_slaves_sec_status >> 16) & mask_s1;
	write32((void *)AXIIC_ext_s1_security, val);
}

/*
 * Set cfg slave security
 * Use defines in tz.h for both parameters.
 */
void tz_set_cfg_slaves_security(uint32_t slave_vector, uint32_t ns_bit)
{
	uint32_t val;
	uint32_t mask_r = AXIIC_APBR_s0_security_MASK;
	uint32_t mask_s = AXIIC_APBS_s0_security_MASK;
	uint32_t tz_cfg_slaves_sec_status =
		(mask_r & read32((void *)AXIIC_APBR_s0_security)) |
		((mask_s & read32((void *)AXIIC_APBS_s0_security)) << 16);

	if (ns_bit == TZ_STATE_SECURE)
		tz_cfg_slaves_sec_status &= ~slave_vector;
	else
		tz_cfg_slaves_sec_status |= slave_vector;

	val = tz_cfg_slaves_sec_status & mask_r;
	write32((void *)AXIIC_APBR_s0_security, val);

	val = (tz_cfg_slaves_sec_status >> 16) & mask_s;
	write32((void *)AXIIC_APBS_s0_security, val);
}

/*
 * Set SRAM secure region
 * parameter 'r0size' specify the secure RAM region in 4KB steps:
 * 0x00000000 = no secure region
 * 0x00000001 = 4KB secure region
 * 0x00000002 = 8KB secure region
 * .......
 * 0x000001FF = 2044KB secure region.
 * 0x00000200 or above sets the entire SRAM to secure regardless of size
 */
void tz_set_sram_sec_region(uint32_t r0size)
{
	uint32_t mask = TZPC_TZPCR0SIZE_MASK;

	write32((void *)TZPC_TZPCR0SIZE, r0size & mask);
}

/*
 * Set wrapper security
 * Use defines in tz.h for all parameters.
 */
void tz_set_wrapper_security(uint32_t wrapper1, uint32_t wrapper2,
			     uint32_t wrapper3, uint32_t wrapper4,
			     uint32_t ns_bit)
{
	uint32_t mask_w4 = AXIIC_APB_W4_security_MASK;
	uint32_t mask_w3 = AXIIC_APB_W3_security_MASK;
	uint32_t mask_w2 = AXIIC_APB_W2_security_MASK;
	uint32_t mask_w1 = AXIIC_APB_W1_security_MASK;
	uint32_t tz_wrapper1_sec_status = read32((void *)AXIIC_APB_W1_security);
	uint32_t tz_wrapper2_sec_status = read32((void *)AXIIC_APB_W2_security);
	uint32_t tz_wrapper3_sec_status = read32((void *)AXIIC_APB_W3_security);
	uint32_t tz_wrapper4_sec_status = read32((void *)AXIIC_APB_W4_security);

	if (ns_bit == TZ_STATE_SECURE) {
		tz_wrapper1_sec_status &= ~wrapper1;
		tz_wrapper2_sec_status &= ~wrapper2;
		tz_wrapper3_sec_status &= ~wrapper3;
		tz_wrapper4_sec_status &= ~wrapper4;
	} else {
		tz_wrapper1_sec_status |= wrapper1;
		tz_wrapper2_sec_status |= wrapper2;
		tz_wrapper3_sec_status |= wrapper3;
		tz_wrapper4_sec_status |= wrapper4;
	}
	write32((void *)AXIIC_APB_W1_security,
		tz_wrapper1_sec_status & mask_w1);
	write32((void *)AXIIC_APB_W2_security,
		tz_wrapper2_sec_status & mask_w2);
	write32((void *)AXIIC_APB_W3_security,
		tz_wrapper3_sec_status & mask_w3);
	write32((void *)AXIIC_APB_W4_security,
		tz_wrapper4_sec_status & mask_w4);
}
