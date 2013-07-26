/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2011 Sven Schnelle <svens@stackframe.org>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; version 2 of
 * the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston,
 * MA 02110-1301 USA
 */

#include "raminit.h"
#include <arch/io.h>
#include <device/pci_def.h>
#include <device/pnp_def.h>
#include <cpu/x86/lapic.h>
#include <cpu/intel/speedstep.h>
#include <console/console.h>
#include <spd.h>
#include <types.h>
#include <string.h>
#include <cbmem.h>
#include <stdlib.h>
#include <lib.h>
#include <delay.h>

static int i5000_for_each_channel(struct i5000_fbd_branch *branch,
				  int (*cb)(struct i5000_fbd_channel *))
{
	struct i5000_fbd_channel *c;
	int ret;

	for(c = branch->channel; c < branch->channel + I5000_MAX_CHANNEL; c++)
		if (c->used && (ret = cb(c)))
			return ret;
	return 0;
}

static int i5000_for_each_branch(struct i5000_fbd_setup *setup,
				 int (*cb)(struct i5000_fbd_branch *))
{
	struct i5000_fbd_branch *b;
	int ret;

	for(b = setup->branch; b < setup->branch + I5000_MAX_BRANCH; b++)
		if (b->used && (ret = cb(b)))
			return ret;
	return 0;
}

static int i5000_for_each_dimm(struct i5000_fbd_setup *setup,
			       int (*cb)(struct i5000_fbdimm *))
{
	struct i5000_fbdimm *d;
	int ret, i;

	for(i = 0; i < I5000_MAX_DIMMS; i++) {
		d = setup->dimms[i];
		if ((ret = cb(d))) {
			return ret;
		}
	}
	return 0;
}

static int i5000_for_each_dimm_present(struct i5000_fbd_setup *setup,
				       int (*cb)(struct i5000_fbdimm *))
{
	struct i5000_fbdimm *d;
	int ret, i;

	for(i = 0; i < I5000_MAX_DIMMS; i++) {
		d = setup->dimms[i];
		if (d->present && (ret = cb(d)))
			return ret;
	}
	return 0;
}

static int spd_read_byte(struct i5000_fbdimm *d, u8 addr, int count, u8 *out)
{
	u16 status;
	device_t dev = d->branch->branchdev;

	int cmdreg = d->channel->num ? I5000_SPDCMD1 : I5000_SPDCMD0;
	int stsreg = d->channel->num ? I5000_SPD1 : I5000_SPD0;

	while(count-- > 0) {
		pci_write_config32(dev, cmdreg, 0xa8000000 |	\
				   (d->num & 0x03) << 24 | addr++ << 16);

		int timeout = 1000;
		while((status = pci_read_config16(dev, stsreg)) & I5000_SPD_BUSY && timeout--)
			udelay(10);

		if (status & I5000_SPD_SBE || !timeout)
			return -1;

		if (status & I5000_SPD_RDO) {
			*out = status & 0xff;
			out++;
		}
	}
	return 0;
}

static void i5000_clear_fbd_errors(void)
{
	device_t dev16_1, dev16_2;

	dev16_1 = PCI_ADDR(0, 16, 1, 0);
	dev16_2 = PCI_ADDR(0, 16, 2, 0);

	pci_write_config32(dev16_1, I5000_EMASK_FBD,
				pci_read_config32(dev16_1, I5000_EMASK_FBD));

	pci_write_config32(dev16_1, I5000_NERR_FAT_FBD,
				pci_read_config32(dev16_1, I5000_NERR_FAT_FBD));

	pci_write_config32(dev16_1, I5000_FERR_FAT_FBD,
				pci_read_config32(dev16_1, I5000_FERR_FAT_FBD));

	pci_write_config32(dev16_1, I5000_NERR_NF_FBD,
				pci_read_config32(dev16_1, I5000_NERR_NF_FBD));

	pci_write_config32(dev16_1, I5000_FERR_NF_FBD,
				pci_read_config32(dev16_1, I5000_FERR_NF_FBD));

	pci_write_config32(dev16_2, I5000_FERR_GLOBAL,
				pci_read_config32(dev16_2, I5000_FERR_GLOBAL));

	pci_write_config32(dev16_2, I5000_NERR_GLOBAL,
				pci_read_config32(dev16_2, I5000_NERR_GLOBAL));
}

static int i5000_branch_reset(struct i5000_fbd_branch *b)
{
	device_t dev = b->branchdev;

	pci_write_config8(dev, I5000_FBDRST, 0x00);

	udelay(5000);

	pci_write_config8(dev, I5000_FBDRST, 0x05);
	udelay(1);
	pci_write_config8(dev, I5000_FBDRST, 0x04);
	udelay(2);
	pci_write_config8(dev, I5000_FBDRST, 0x05);
	pci_write_config8(dev, I5000_FBDRST, 0x07);
	return 0;
}

static int delay_ns_to_clocks(struct i5000_fbdimm *d, int del)
{
	int div;

	switch (d->setup->ddr_speed) {
	case DDR_533MHZ:
		div = 375;
		break;

	default:
		printk(BIOS_ERR, "Invalid clock: %d, using 667MHz\n",
		       d->setup->ddr_speed);

	case DDR_667MHZ:
		div = 300;
		break;
	}

	return (del * 100) / div;
}

static int mtb2clks(struct i5000_fbdimm *d, int del)
{
	int val, div;

	switch (d->setup->ddr_speed) {
	case DDR_533MHZ:
		div = 375;
		break;
	default:
		printk(BIOS_ERR, "Invalid clock: %d, using 667MHz\n",
		       d->setup->ddr_speed);

	case DDR_667MHZ:
		div = 300;
		break;
	}

	val = (del * 1000 * d->mtb_dividend) / (d->mtb_divisor * div);
	if ((val % 10) > 0)
		val += 10;
	return val / 10;
}

static int i5000_read_spd_data(struct i5000_fbdimm *d)
{
	struct i5000_fbd_setup *s = d->setup;
	u8 addr, val, org, ftb, cas, t_ras_rc_h, t_rtp, t_wtr;
	u8 bb, bl, t_wr, t_rp, t_rcd, t_rc, t_ras, t_aa_min;
	u8 cmd2data_addr;
	int t_ck_min, dimmsize;

	if (spd_read_byte(d, SPD_MEMORY_TYPE, 1, &val)) {
		printk(BIOS_DEBUG, "DIMM %d/%d/%d not present\n",
		       d->branch->num, d->channel->num, d->num);
		return 0; // No FBDIMM present
	}

	if (val != 0x09)
		return -1; // SDRAM type not FBDIMM

	if (spd_read_byte(d, 0x65, 14, d->amb_personality_bytes))
		return -1;

	switch(s->ddr_speed) {
	case DDR_533MHZ:
		cmd2data_addr = FBDIMM_SPD_CMD2DATA_533;
		break;

	case DDR_667MHZ:
		cmd2data_addr = FBDIMM_SPD_CMD2DATA_667;
		break;

	default:
		printk(BIOS_ERR, "Unsupported FBDIMM clock\n");
		return -1;
	}

	if (spd_read_byte(d, FBDIMM_SPD_SDRAM_ADDRESSING, 1, &addr) ||
	    spd_read_byte(d, FBDIMM_SPD_MODULE_ORGANIZATION, 1, &org) ||
	    spd_read_byte(d, FBDIMM_SPD_FTB, 1, &ftb) ||
	    spd_read_byte(d, FBDIMM_SPD_MTB_DIVIDEND, 1, &d->mtb_dividend) ||
	    spd_read_byte(d, FBDIMM_SPD_MTB_DIVISOR, 1, &d->mtb_divisor) ||
	    spd_read_byte(d, FBDIMM_SPD_MIN_TCK, 1, &d->t_ck_min) ||
	    spd_read_byte(d, FBDIMM_SPD_T_WR, 1, &t_wr) ||
	    spd_read_byte(d, FBDIMM_SPD_T_RCD, 1, &t_rcd) ||
	    spd_read_byte(d, FBDIMM_SPD_T_RRD, 1, &d->t_rrd) ||
	    spd_read_byte(d, FBDIMM_SPD_T_RP, 1, &t_rp) ||
	    spd_read_byte(d, FBDIMM_SPD_T_RAS_RC_MSB, 1, &t_ras_rc_h) ||
	    spd_read_byte(d, FBDIMM_SPD_T_RAS, 1, (u8 *)&t_ras) ||
	    spd_read_byte(d, FBDIMM_SPD_T_RC, 1, (u8 *)&t_rc) ||
	    spd_read_byte(d, FBDIMM_SPD_T_RFC, 2, (u8 *)&d->t_rfc) ||
	    spd_read_byte(d, FBDIMM_SPD_T_WTR, 1, &t_wtr) ||
	    spd_read_byte(d, FBDIMM_SPD_T_RTP, 1, &t_rtp) ||
	    spd_read_byte(d, FBDIMM_SPD_T_BB, 1, &bb) ||
	    spd_read_byte(d, FBDIMM_SPD_BURST_LENGTHS_SUPPORTED, 1, &bl) ||
	    spd_read_byte(d, FBDIMM_SPD_ODT, 1, &d->odt) ||
	    spd_read_byte(d, FBDIMM_SPD_T_REFI, 1, &d->t_refi) ||
	    spd_read_byte(d, FBDIMM_SPD_CAS_LATENCIES, 1, &cas) ||
	    spd_read_byte(d, FBDIMM_SPD_CMD2DATA_533, 1, &d->cmd2datanxt[DDR_533MHZ]) ||
	    spd_read_byte(d, FBDIMM_SPD_CMD2DATA_667, 1, &d->cmd2datanxt[DDR_667MHZ]) ||
	    spd_read_byte(d, FBDIMM_SPD_CAS_MIN_LATENCY, 1, &t_aa_min)) {
		printk(BIOS_ERR, "failed to read data from SPD\n");
		return 0;
	}


	t_ck_min = (d->t_ck_min * 100) / d->mtb_divisor;
	if (t_ck_min <= 300)
		d->speed = DDR_667MHZ;
	else if (t_ck_min <= 375)
		d->speed = DDR_533MHZ;
	else {
		printk(BIOS_ERR, "Unsupported t_ck_min: %d\n", t_ck_min);
		return -1;
	}

	d->sdram_width = org & 0x07;
	if (d->sdram_width > 1) {
		printk(BIOS_ERR, "SDRAM width %d not supported\n", d->sdram_width);
		return -1;
	}

	if (s->ddr_speed == DDR_667MHZ && d->speed == DDR_533MHZ)
		s->ddr_speed = DDR_533MHZ;

	d->banks = 4 << (addr & 0x03);
	d->columns = 9 + ((addr >> 2) & 0x03);
	d->rows = 12 + ((addr >> 5) & 0x03);
	d->ranks = (org >> 3) & 0x03;
	d->min_cas_latency = cas & 0x0f;

	s->bl &= bl;

	if (!s->bl) {
		printk(BIOS_ERR, "no compatible burst length found\n");
		return -1;
	}

	s->t_rc = MAX(s->t_rc, mtb2clks(d,
		      t_rc | ((t_ras_rc_h & 0xf0) << 4)));
	s->t_rrd = MAX(s->t_rrd, mtb2clks(d, d->t_rrd));
	s->t_rfc = MAX(s->t_rfc, mtb2clks(d, d->t_rfc));
	s->t_rcd = MAX(s->t_rcd, mtb2clks(d, t_rcd));
	s->t_cl = MAX(s->t_cl, mtb2clks(d, t_aa_min));
	s->t_wr = MAX(s->t_wr, mtb2clks(d, t_wr));
	s->t_rp = MAX(s->t_rp, mtb2clks(d, t_rp));
	s->t_rtp = MAX(s->t_rtp, mtb2clks(d, t_rtp));
	s->t_wtr = MAX(s->t_wtr, mtb2clks(d, t_wtr));
	s->t_ras = MAX(s->t_ras, mtb2clks(d,
					  t_ras | ((t_ras_rc_h & 0x0f) << 8)));
	s->t_r2r = MAX(s->t_r2r, bb & 3);
	s->t_r2w = MAX(s->t_r2w, (bb >> 4) & 3);
	s->t_w2r = MAX(s->t_w2r, (bb >> 2) & 3);

	d->ranksize = (1 << (d->banks + d->columns + d->rows + 1)) >> 20;
	dimmsize = d->ranksize * d->ranks;
	d->branch->totalmem += dimmsize;
	s->totalmem += dimmsize;

	d->channel->columns = d->columns;
	d->channel->rows = d->rows;
	d->channel->ranks = d->ranks;
	d->channel->banks = d->banks;
	d->channel->width = d->sdram_width;

	printk(BIOS_INFO, "DIMM %d/%d/%d %dMB: %d banks, "
	       "%d columns, %d rows, %d ranks\n",
	       d->branch->num, d->channel->num, d->num, dimmsize,
	       d->banks, d->columns, d->rows, d->ranks);

	d->present = 1;
	d->branch->used |= 1;
	d->channel->used |= 1;
	d->channel->highest_amb = d->num;
	return 0;
}

static int i5000_amb_smbus_write(struct i5000_fbdimm *d,  int byte1, int byte2)
{
	u16 status;
	device_t dev = PCI_DEV(0, d->branch->num ? 22 : 21, 0);
	int cmdreg = d->channel->num ? I5000_SPDCMD1 : I5000_SPDCMD0;
	int stsreg = d->channel->num ? I5000_SPD1 : I5000_SPD0;
	int timeout = 1000;

	pci_write_config32(dev, cmdreg, 0xb8000000 | ((d->num & 0x03) << 24) |
			   (byte1 << 16) | (byte2 << 8) | 1);

	while(((status = pci_read_config16(dev, stsreg)) & I5000_SPD_BUSY) && timeout--)
		udelay(10);

	if (status & I5000_SPD_WOD && timeout)
		return 0;

	printk(BIOS_ERR, "SMBus write failed: %d/%d/%d, byte1 %02x, byte2 %02x status %04x\n",
	       d->branch->num, d->channel->num, d->num, byte1, byte2, status);
	for(;;);
	return -1;
}

static int i5000_amb_smbus_read(struct i5000_fbdimm *d, int byte1, u8 *out)
{
	u16 status;
	device_t dev = PCI_DEV(0, d->branch->num ? 22 : 21, 0);
	int cmdreg = d->channel->num ? I5000_SPDCMD1 : I5000_SPDCMD0;
	int stsreg = d->channel->num ? I5000_SPD1 : I5000_SPD0;
	int timeout = 1000;

	pci_write_config32(dev, cmdreg, 0xb8000000 | ((d->num & 0x03) << 24) |
			   (byte1 << 16));

	while(((status = pci_read_config16(dev, stsreg)) & I5000_SPD_BUSY) && timeout--)
		udelay(10);

	if ((status & I5000_SPD_RDO) && timeout)
		*out = status & 0xff;

	if (status & I5000_SPD_SBE || !timeout) {
		printk(BIOS_ERR, "SMBus write failed: %d/%d/%d, byte1 %02x status %04x\n",
		       d->branch->num, d->channel->num, d->num, byte1, status);
		return -1;
	}
	return 0;

}

static int i5000_amb_smbus_write_config8(struct i5000_fbdimm *d,
					 int fn, int reg, u8 val)
{
	if (i5000_amb_smbus_write(d, 0x84, 00) ||
	    i5000_amb_smbus_write(d, 0x04, fn) ||
	    i5000_amb_smbus_write(d, 0x04, (reg >> 8) & 0xff) ||
	    i5000_amb_smbus_write(d, 0x04, reg & 0xff) ||
	    i5000_amb_smbus_write(d, 0x44, val)) {
		printk(BIOS_ERR, "AMB SMBUS write failed\n");
		return 1;
	}
	return 0;
}

static int i5000_amb_smbus_write_config16(struct i5000_fbdimm *d,
					  int fn, int reg, u16 val)
{
	if (i5000_amb_smbus_write(d, 0x88, 00) ||
	    i5000_amb_smbus_write(d, 0x08, fn) ||
	    i5000_amb_smbus_write(d, 0x08, (reg >> 8) & 0xff) ||
	    i5000_amb_smbus_write(d, 0x08, reg & 0xff) ||
	    i5000_amb_smbus_write(d, 0x08, (val >> 8) & 0xff) ||
	    i5000_amb_smbus_write(d, 0x48, val & 0xff)) {
		printk(BIOS_ERR, "AMB SMBUS write failed\n");
		return 1;
	}
	return 0;
}

static int i5000_amb_smbus_write_config32(struct i5000_fbdimm *d,
					  int fn, int reg, u32 val)
{
	if (i5000_amb_smbus_write(d, 0x8c, 00) ||
	    i5000_amb_smbus_write(d, 0x0c, fn) ||
	    i5000_amb_smbus_write(d, 0x0c, (reg >> 8) & 0xff) ||
	    i5000_amb_smbus_write(d, 0x0c, reg & 0xff) ||
	    i5000_amb_smbus_write(d, 0x0c, (val >> 24) & 0xff) ||
	    i5000_amb_smbus_write(d, 0x0c, (val >> 16) & 0xff) ||
	    i5000_amb_smbus_write(d, 0x0c, (val >> 8) & 0xff) ||
	    i5000_amb_smbus_write(d, 0x4c, val & 0xff)) {
		printk(BIOS_ERR, "AMB SMBUS write failed\n");
		return 1;
	}
	return 0;
}

static int i5000_amb_smbus_read_config32(struct i5000_fbdimm *d,
					 int fn, int reg, u32 *val)
{
	u8 byte3, byte2, byte1, byte0;

	if (i5000_amb_smbus_write(d, 0x80, 00) ||
	    i5000_amb_smbus_write(d, 0x00, fn) ||
	    i5000_amb_smbus_write(d, 0x00, (reg >> 8) & 0xff) ||
	    i5000_amb_smbus_write(d, 0x40, reg & 0xff) ||
	    i5000_amb_smbus_read(d, 0x80, &byte3) ||
	    i5000_amb_smbus_read(d, 0x00, &byte3) ||
	    i5000_amb_smbus_read(d, 0x00, &byte2) ||
	    i5000_amb_smbus_read(d, 0x00, &byte1) ||
	    i5000_amb_smbus_read(d, 0x40, &byte0)) {
		printk(BIOS_ERR, "AMB SMBUS read failed\n");
		return 1;
	}
	*val = (byte3 << 24) | (byte2 << 16) | (byte1 << 8) | byte0;
	return 0;
}

static void i5000_amb_write_config8(struct i5000_fbdimm *d,
				    int fn, int reg, u32 val)
{
	write8(DEFAULT_AMBASE + AMB_ADDR(d->ambase, fn, reg), val);
}

static void i5000_amb_write_config16(struct i5000_fbdimm *d,
				     int fn, int reg, u32 val)
{
	write16(DEFAULT_AMBASE + AMB_ADDR(d->ambase, fn, reg), val);
}

static void i5000_amb_write_config32(struct i5000_fbdimm *d,
				     int fn, int reg, u32 val)
{
	write32(DEFAULT_AMBASE + AMB_ADDR(d->ambase, fn, reg), val);
}

static u32 i5000_amb_read_config32(struct i5000_fbdimm *d,
				   int fn, int reg)
{
	return read32(DEFAULT_AMBASE + AMB_ADDR(d->ambase, fn, reg));
}

static int ddr_command(struct i5000_fbdimm *d, int rank, u32 addr, u32 command)
{
	u32 drc, status;

	printk(BIOS_SPEW, "DIMM %d/%d/%d: rank %d: sending command %x (addr %08x)...",
	       d->branch->num, d->channel->num, d->num, rank, command, addr);

	drc = i5000_amb_read_config32(d, 3, AMB_DRC);
	drc &= ~((3 << 9) | (1 << 12));
	drc |= (rank << 9);

	command &= 0x0f;
	command |= AMB_DCALCSR_START | (rank << 21);

	printk(BIOS_DEBUG, "%s: AMB_DCALADDR: %08x AMB_DCALCSR: %08x\n", __func__, addr, command);
	i5000_amb_write_config32(d, 3, AMB_DRC, drc);
	i5000_amb_write_config32(d, 4, AMB_DCALADDR, addr);
	i5000_amb_write_config32(d, 4, AMB_DCALCSR, command);

	udelay(1000);
	while((status = (i5000_amb_read_config32(d, 4, AMB_DCALCSR)))
	      & (1 << 31));

	if (status & (1 << 30)) {
		printk(BIOS_SPEW, "failed (status 0x%08x)\n", status);
		return -1;
	}

	printk(BIOS_SPEW, "done\n");
	return 0;
}

static int i5000_ddr_calibration(struct i5000_fbdimm *d)
{
	u32 status;

	i5000_amb_write_config32(d, 3, AMB_MBADDR, 0);
	i5000_amb_write_config32(d, 3, AMB_MBCSR, 0x80100050);
	while((status = i5000_amb_read_config32(d, 3, AMB_MBCSR)) & (1 << 31));

	i5000_amb_write_config32(d, 3, AMB_MBCSR, 0x80200050);
	while((status = i5000_amb_read_config32(d, 3, AMB_MBCSR)) & (1 << 31));

	if (ddr_command(d, d->ranks == 2 ? 3 : 1, 0, AMB_DCALCSR_OPCODE_RECV_ENABLE_CAL) ||
	    ddr_command(d, d->ranks == 2 ? 3 : 1, 0, AMB_DCALCSR_OPCODE_DQS_DELAY_CAL))
		return -1;
	return 0;
}

static int i5000_ddr_init(struct i5000_fbdimm *d)
{

	int rank;
	u32 val;
	u8 odt;

	for(rank = 0; rank < d->ranks; rank++) {
		printk(BIOS_DEBUG, "%s: %d/%d/%d rank %d\n", __func__,
		       d->branch->num, d->channel->num, d->num, rank);

		if (ddr_command(d, 1 << rank,
				0, AMB_DCALCSR_OPCODE_NOP))
			return -1;

		if (ddr_command(d, 1 << rank,
				0x4000000, AMB_DCALCSR_OPCODE_PRECHARGE))
			return -1;

		/* EMRS(2) */
		if (ddr_command(d, 1 << rank,
				2, AMB_DCALCSR_OPCODE_MRS_EMRS))
			return -1;

		/* EMRS(3) */
		if (ddr_command(d, 1 << rank,
				3, AMB_DCALCSR_OPCODE_MRS_EMRS))
			return -1;

		/* EMRS(1) */
		if (ddr_command(d, 1 << rank,
				1, AMB_DCALCSR_OPCODE_MRS_EMRS))
			return -1;

		/* MRS: DLL reset */
		if (ddr_command(d, 1 << rank,
				0x1000000, AMB_DCALCSR_OPCODE_MRS_EMRS))
			return -1;

		udelay(20);

		if (ddr_command(d, 1 << rank,
				0x4000000, AMB_DCALCSR_OPCODE_PRECHARGE))
			return -1;

		if (ddr_command(d, 1 << rank,
				0, AMB_DCALCSR_OPCODE_REFRESH))
			return -1;

		if (ddr_command(d, 1 << rank, 0,
				AMB_DCALCSR_OPCODE_REFRESH))
			return -1;

		/* burst length + cas latency */
		val = (((d->setup->bl & BL_BL8) ? 3 : 2) << 16) |
			(1 << 19) /* interleaved burst */ |
			(d->setup->t_cl << 20) |
			(((d->setup->t_wr - 1) & 7) << 25);

		printk(BIOS_DEBUG, "MRS: 0x%08x\n", val);
		if (ddr_command(d, 1 << rank,
				val, AMB_DCALCSR_OPCODE_MRS_EMRS))
			return -1;

		/* OCD calibration default */
		if (ddr_command(d, 1 << rank, 0x03800001,
				AMB_DCALCSR_OPCODE_MRS_EMRS))
			return -1;


		odt = d->odt;
		if (rank)
			odt >>= 4;

		val = (d->setup->t_al << 19) |
			((odt & 1) << 18) |
		        ((odt & 2) << 21) | 1;

		printk(BIOS_DEBUG, "EMRS(1): 0x%08x\n", val);

		/* ODT, OCD exit, additive latency */
		if (ddr_command(d, 1 << rank, val, AMB_DCALCSR_OPCODE_MRS_EMRS))
			return -1;
	}
	return 0;
}

static int i5000_amb_preinit(struct i5000_fbdimm *d)
{
	u32 *p32 = (u32 *)d->amb_personality_bytes;
	u16 *p16 = (u16 *)d->amb_personality_bytes;
	u32 id, drc, fbdsbcfg = 0x0909;

	printk(BIOS_DEBUG, "%s: %d/%d/%d\n", __func__,
	       d->branch->num, d->channel->num, d->num);

	i5000_amb_smbus_write_config32(d, 1, 0xb0, p32[0]);
	i5000_amb_smbus_write_config16(d, 1, 0xb4, p16[2]);

	drc = (d->setup->t_al << 4) | (d->setup->t_cl);
	printk(BIOS_SPEW, "DRC: %02X, CMD2DATANXT: %02x\n", drc,
	       d->cmd2datanxt[d->setup->ddr_speed]);

	switch(d->setup->ddr_speed) {
	case DDR_533MHZ:
		fbdsbcfg |= (1 << 16);
		break;
	case DDR_667MHZ:
		fbdsbcfg |= (2 << 16);
		break;
	default:
		return -1;
	}

	printk(BIOS_DEBUG, "FBDSBCFGNXT: %08x\n", fbdsbcfg);
	i5000_amb_smbus_write_config32(d, 1, AMB_FBDSBCFGNXT, fbdsbcfg);
	i5000_amb_smbus_write_config32(d, 1, AMB_FBDLOCKTO, 0x1651);
	i5000_amb_smbus_write_config8(d, 1, AMB_CMD2DATANXT,
		d->cmd2datanxt[d->setup->ddr_speed]);

	i5000_amb_smbus_write_config8(d, 3, AMB_DRC, drc);

	if (!i5000_amb_smbus_read_config32(d, 0, 0, &id)) {
		d->vendor = id & 0xffff;
		d->device = id >> 16;
	}

	pci_write_config8(d->branch->branchdev,
			       d->channel->num ? I5000_FBDSBTXCFG1 : I5000_FBDSBTXCFG0, 0x04);
	return 0;
}

static void i5000_fbd_next_state(struct i5000_fbd_branch *b, int state)
{
	int timeout = 10000;
	device_t dev = b->branchdev;

	printk(BIOS_DEBUG, "  FBD state branch %d: %02x,", b->num, state);

	pci_write_config8(dev, I5000_FBDHPC, state);

	printk(BIOS_DEBUG, "waiting for new state...");

	while(pci_read_config8(dev, I5000_FBDST) != state && timeout--)
		udelay(10);

	if (timeout) {
		printk(BIOS_DEBUG, "done\n");
		return;
	}

	printk(BIOS_ERR, "timeout while entering state %02x on branch %d\n",
	       state, b->num);
}

static int i5000_wait_pattern_recognized(struct i5000_fbd_channel *c)
{
	int i = 10;
	device_t dev = PCI_ADDR(0, c->branch->num ? 22 : 21, 0,
				c->num ? I5000_FBDISTS1 : I5000_FBDISTS0);

	printk(BIOS_DEBUG, "      waiting for pattern recognition...");
	while(pci_read_config16(dev, 0) != 0x1fff && --i > 0)
		udelay(5000);

	printk(BIOS_DEBUG, i ?  "done\n" : "failed\n");
	printk(BIOS_DEBUG, "%d/%d Round trip latency: %d\n", c->branch->num, c->num,
	       pci_read_config8(c->branch->branchdev, c->num ? I5000_FBDLVL1 : I5000_FBDLVL0) & 0x3f);
	return !i;
}

static const char *pattern_names[16] = {
	"EI", "EI", "EI", "EI",
	"EI", "EI", "EI", "EI",
	"TS0", "TS1", "TS2", "TS3",
	"RESERVED", "TS2 (merge disabled)", "TS2 (merge enabled)","All ones",
};

static int i5000_drive_pattern(struct i5000_fbd_channel *c, int pattern, int wait)
{
	device_t dev = PCI_ADDR(0, c->branch->num ? 22 : 21, 0,
				c->num ? I5000_FBDICMD1 : I5000_FBDICMD0);

	printk(BIOS_DEBUG, "    %d/%d  driving pattern %s to AMB%d (%02x)\n",
	       c->branch->num, c->num,
	       pattern_names[(pattern >> 4) & 0xf], pattern & 3, pattern);
	pci_write_config8(dev, 0, pattern);

	if (!wait)
		return 0;

	return i5000_wait_pattern_recognized(c);
}

static int i5000_set_ambpresent(struct i5000_fbd_channel *c)
{
	int i;
	device_t branchdev = c->branch->branchdev;
	u16 ambpresent = 0x8000;

	for(i = 0; i < I5000_MAX_DIMM_PER_CHANNEL; i++) {
		if (c->dimm[i].present)
			ambpresent |= (1 << i);
	}

	printk(BIOS_DEBUG, "AMBPRESENT: %04x\n", ambpresent);
	pci_write_config16(branchdev,
			   c->num ?
			   I5000_AMBPRESENT1 :		\
			   I5000_AMBPRESENT0, ambpresent);

	return 0;
}


static int i5000_drive_test_patterns(struct i5000_fbd_channel *c, int highest_amb, int mchpad)
{
	device_t branchdev = c->branch->branchdev;
	int off = c->num ? 0x100 : 0;
	u32 portctl;
	int i, cnt = 1000;

	portctl = pci_read_config32(branchdev, I5000_FBD0IBPORTCTL + off);
	portctl &= ~0x01000020;
	if (mchpad)
		portctl |= 0x00800000;
	else
		portctl &= ~0x00800000;
	portctl &= ~0x01000020;
	pci_write_config32(branchdev, I5000_FBD0IBPORTCTL + off, portctl);

	/* drive calibration patterns */
	if (i5000_drive_pattern(c, I5000_FBDICMD_TS0 | highest_amb, 1))
		return -1;

	if (i5000_drive_pattern(c, I5000_FBDICMD_TS1 | highest_amb, 1))
		return -1;

	while (!(pci_read_config32(branchdev, I5000_FBD0IBPORTCTL + off) & 4) && cnt--)
		udelay(10);

	if (!cnt) {
		printk(BIOS_ERR, "IBIST timeout\n");
		return -1;
	}

	if (i5000_drive_pattern(c, I5000_FBDICMD_TS2 | highest_amb, 1))
		return -1;

	for(i = 0; i < highest_amb; i++) {
		if ((i5000_drive_pattern(c, I5000_FBDICMD_TS2_NOMERGE | i, 1)))
			return -1;
	}

	if (i5000_drive_pattern(c, I5000_FBDICMD_TS2 | highest_amb, 1))
		return -1;

	if (i5000_drive_pattern(c, I5000_FBDICMD_TS3 | highest_amb, 1))
		return -1;

	if (i5000_set_ambpresent(c))
		return -1;
	return 0;
}

static int i5000_train_channel_idle(struct i5000_fbd_channel *c)
{
	int i;
	u32 fbdsbcfg = 0x0b1b;

	switch(c->setup->ddr_speed) {
	case DDR_533MHZ:
		fbdsbcfg |= (1 << 16);
		break;
	case DDR_667MHZ:
		fbdsbcfg |= (2 << 16);
		break;
	default:
		return -1;
	}

	pci_write_config8(c->branch->branchdev,
			       c->num ? I5000_FBDSBTXCFG1 : I5000_FBDSBTXCFG0, 0x05);

	for(i = 0; i < 4; i++) {
		if (c->dimm[i].present)
			i5000_amb_smbus_write_config32(c->dimm + i, 1, AMB_FBDSBCFGNXT, i ? (fbdsbcfg | 0x1000) : fbdsbcfg);
	}

	return i5000_drive_pattern(c, I5000_FBDICMD_IDLE, 1);
}

static int i5000_drive_test_patterns0(struct i5000_fbd_channel *c)
{
	if (i5000_train_channel_idle(c))
		return -1;

	return i5000_drive_test_patterns(c, c->highest_amb, 0);
}

static int i5000_drive_test_patterns1(struct i5000_fbd_channel *c)
{
	if (i5000_train_channel_idle(c))
		return -1;

	return i5000_drive_test_patterns(c, c->highest_amb, 1);
}

static int i5000_setup_channel(struct i5000_fbd_channel *c)
{
	device_t branchdev = c->branch->branchdev;
	int off = c->branch->num ? 0x100 : 0;
	u32 val;

	pci_write_config32(branchdev, I5000_FBD0IBTXPAT2EN + off, 0);
	pci_write_config32(branchdev, I5000_FBD0IBTXPAT2EN + off, 0);
	pci_write_config32(branchdev, I5000_FBD0IBTXMSK + off, 0x3ff);
	pci_write_config32(branchdev, I5000_FBD0IBRXMSK + off, 0x1fff);

	pci_write_config16(branchdev, off + 0x0162, c->used ? 0x20db : 0x18db);

	/* unknown */
	val = pci_read_config32(branchdev, off + 0x0164);
	val &= 0xfffbcffc;
	val |= 0x4004;
	pci_write_config32(branchdev, off + 0x164, val);

	pci_write_config32(branchdev, off + 0x15c, 0xff);
	i5000_drive_pattern(c, I5000_FBDICMD_ALL_ONES, 0);
	return 0;
}

static int i5000_link_training0(struct i5000_fbd_branch *b)
{
	device_t branchdev = b->branchdev;

	pci_write_config8(branchdev, I5000_FBDPLLCTRL, b->used ? 0 : 1);

	if (i5000_for_each_channel(b, i5000_setup_channel))
		return -1;

	if (i5000_for_each_channel(b, i5000_train_channel_idle))
		return -1;

	i5000_fbd_next_state(b, I5000_FBDHPC_STATE_INIT);

	if (i5000_for_each_channel(b, i5000_drive_test_patterns0))
		return -1;

	i5000_fbd_next_state(b, I5000_FBDHPC_STATE_READY);
	return 0;
}

static int i5000_link_training1(struct i5000_fbd_branch *b)
{
	if (i5000_for_each_channel(b, i5000_train_channel_idle))
		return -1;

	i5000_fbd_next_state(b, I5000_FBDHPC_STATE_INIT);

	if (i5000_for_each_channel(b, i5000_drive_test_patterns1))
		return -1;

	i5000_fbd_next_state(b, I5000_FBDHPC_STATE_READY);
	return 0;
}


static int i5000_amb_check(struct i5000_fbdimm *d)
{
	u32 id = i5000_amb_read_config32(d, 0, 0);

	printk(BIOS_DEBUG, "AMB %d/%d/%d ID: %04x:%04x\n",
	       d->branch->num, d->channel->num, d->num,
	       id & 0xffff, id >> 16);

	if ((id & 0xffff) != d->vendor || id >> 16 != d->device) {
		printk(BIOS_ERR, "AMB mapping failed\n");
		return -1;
	}
	return 0;
}

static int i5000_amb_postinit(struct i5000_fbdimm *d)
{
	u32 *p32 = (u32 *)d->amb_personality_bytes;
	u16 *p16 = (u16 *)d->amb_personality_bytes;

	i5000_amb_write_config16(d, 1, 0xb6, p16[3]);
	i5000_amb_write_config32(d, 1, 0xb8, p32[2]);
	i5000_amb_write_config16(d, 1, 0xbc, p16[6]);
	return 0;
}

static int i5000_amb_dram_timing_init(struct i5000_fbdimm *d)
{
	struct i5000_fbd_setup *s;
	u32 val, tref;
	int refi;

	s = d->setup;

	printk(BIOS_DEBUG, "DIMM %d/%d/%d config:\n",
	       d->branch->num, d->channel->num, d->num);

	val = 0x44;
	printk(BIOS_DEBUG, "\tDDR2ODTC: 0x%02x\n", val);
	i5000_amb_write_config8(d, 4, AMB_DDR2ODTC, val);

	val = (0x0c << 24) | /* CLK control */
		(1 << 18) | /* ODTZ enabled */
		(((d->setup->bl & BL_BL8) ? 1 : 0) << 8) | /* 8 byte burst length supported */
		((d->setup->t_al & 0x0f) << 4) | /* additive latency */
		(d->setup->t_cl & 0x0f); /* CAS latency */

	if (d->ranks > 1) {
		val |= (0x03 << 9);
	} else {
		val |= (0x01 << 9);
	}

	printk(BIOS_DEBUG, "AMB_DRC: %08x\n", val);
	i5000_amb_write_config32(d, 3, AMB_DRC, val);

	val = (d->sdram_width << 30) |
		((d->ranks == 2 ? 1 : 0) << 29) |
		((d->banks == 8 ? 1 : 0) << 28) |
		((d->rows - 13) << 26) |
		((d->columns - 10) << 24) |
		(1 << 16) | /* Auto refresh exit */
		(0x27 << 8) | /* t_xsnr */
		(d->setup->t_rp << 4) |
		(((d->t_ck_min * d->mtb_dividend) / d->mtb_divisor) & 0x0f);

	printk(BIOS_DEBUG, "\tAMB_DSREFTC: %08x\n", val);
	i5000_amb_write_config32(d, 3, AMB_DSREFTC, val);

	tref = 15;

	switch(d->t_refi & 0x0f) {
	case 0:
		refi = 15625;
		break;
	case 1:
		refi = 3900;
		tref = 3;
		break;
	case 2:
		refi = 7800;
		tref = 7;
		break;
	case 3:
		refi = 31250;
		break;
	case 4:
		refi = 62500;
		break;
	case 5:
		refi = 125000;
		break;
	default:
		printk(BIOS_ERR, "unsupported t_refi value: %d, using 7.8us\n",
		       d->t_refi & 0x0f);
		refi = 7800;
		break;
	}

	s->t_ref = tref;
	val = delay_ns_to_clocks(d, refi) | (s->t_rfc << 16);

	printk(BIOS_DEBUG, "\tAMB_DAREFTC: %08x\n", val);
	i5000_amb_write_config32(d, 3, AMB_DAREFTC, val);

	u8 t_r2w = ((s->bl & BL_BL8) ? 4 : 2) +
		(((d->t_ck_min * d->mtb_dividend) / d->mtb_divisor));
	u8 t_w2r = (s->t_cl - 1) + ((s->bl & BL_BL8) ? 4 : 2) + s->t_wtr;

	val = ((6 - s->t_rp) << 8) | ((6 - s->t_rcd) << 10) |
		((26 - s->t_rc) << 12) | ((9 - s->t_wr) << 16) |
		((12 - t_w2r) << 20) | ((10 - t_r2w) << 24) |
		((s->t_rtp - 2) << 27);

	switch(s->t_ras) {
	case 15:
		val |= (1 << 29);
		break;
	case 12:
		val |= (2 << 29);
		break;
	default:
		break;
	}

	printk(BIOS_DEBUG, "\tAMB_DRT: %08x\n", val);
	i5000_amb_write_config32(d, 3, AMB_DRT, val);
	return 0;
}

static int i5000_do_amb_membist_start(struct i5000_fbdimm *d, int rank, int pattern)
{
	printk(BIOS_DEBUG, "DIMM %d/%d/%d rank %d pattern %d\n",
	       d->branch->num, d->channel->num, d->num, rank, pattern);

	i5000_amb_write_config32(d, 3, AMB_DAREFTC,
				 i5000_amb_read_config32(d, 3, AMB_DAREFTC) | 0x8000);

	i5000_amb_write_config32(d, 3, AMB_MBLFSRSED, 0x12345678);
	i5000_amb_write_config32(d, 3, AMB_MBADDR, 0);
	i5000_amb_write_config32(d, 3, AMB_MBCSR, 0x800000f0 | (rank << 20) | ((pattern & 3) << 8));
	return 0;
}

static int i5000_do_amb_membist_status(struct i5000_fbdimm *d, int rank)
{
	int cnt = 1000;
	u32 res;

	while((res = i5000_amb_read_config32(d, 3, AMB_MBCSR)) & (1 << 31) && cnt--)
		udelay(1000);

	if (cnt && !(res & (1 << 30)))
		return 0;

	printk(BIOS_ERR, "DIMM %d/%d/%d rank %d failed membist check\n",
	       d->branch->num, d->channel->num, d->num, rank);
	return -1;
}

static int i5000_amb_membist_zero1_start(struct i5000_fbdimm *d)
{
	if (i5000_do_amb_membist_start(d, 1, 0))
		return -1;
	return 0;
}

static int i5000_amb_membist_zero2_start(struct i5000_fbdimm *d)
{

	if (d->ranks < 2)
		return 0;
	if (i5000_do_amb_membist_start(d, 2, 0))
		return -1;
	return 0;
}

static int i5000_amb_membist_status1(struct i5000_fbdimm *d)
{
	if (i5000_do_amb_membist_status(d, 1))
		return -1;
	return 0;
}

static int i5000_amb_membist_status2(struct i5000_fbdimm *d)
{
	if (d->ranks < 2)
		return 0;

	if (i5000_do_amb_membist_status(d, 2))
		return -1;
	return 0;
}

static int i5000_amb_membist_end(struct i5000_fbdimm *d)
{
	printk(BIOS_DEBUG, "AMB_DRC MEMBIST: %08x\n", i5000_amb_read_config32(d, 3, AMB_DRC));
	return 0;
}

static int i5000_membist(struct i5000_fbd_setup *setup)
{
	return	i5000_for_each_dimm_present(setup, i5000_amb_membist_zero1_start) ||
		i5000_for_each_dimm_present(setup, i5000_amb_membist_status1) ||
		i5000_for_each_dimm_present(setup, i5000_amb_membist_zero2_start) ||
		i5000_for_each_dimm_present(setup, i5000_amb_membist_status2) ||
		i5000_for_each_dimm_present(setup, i5000_amb_membist_end);
}

static int i5000_enable_mc_autorefresh(struct i5000_fbdimm *d)
{
	u32 tmp = i5000_amb_read_config32(d, 3, AMB_DSREFTC);
	tmp &= ~(1 << 16);
	printk(BIOS_DEBUG, "new AMB_DSREFTC: 0x%08x\n", tmp);
	i5000_amb_write_config32(d, 3, AMB_DSREFTC, tmp);
	return 0;
}

static int i5000_amb_clear_error_status(struct i5000_fbdimm *d)
{
	i5000_amb_write_config32(d, 1, AMB_FERR, 9);
	i5000_amb_write_config32(d, 1, AMB_NERR, 9);
	i5000_amb_write_config32(d, 1, AMB_EMASK, 0xf2);
	i5000_amb_write_config8(d, 3, 0x80, 0xcf);
	i5000_amb_write_config8(d, 3, 0x81, 0xd3);
	i5000_amb_write_config8(d, 3, 0x82, 0xf8);
	return 0;
}

static void i5000_program_mtr(struct i5000_fbd_channel *c, int mtr)
{
	u32 val;

	if (c->dimm[0].present || c->dimm[1].present) {
		val = (((c->columns - 10) & 3) |
		       (((c->rows - 13) & 3) << 2) |
		       (((c->ranks == 2) ? 1 : 0) << 4) |
		       (((c->banks == 8) ? 1 : 0) << 5) |
		       ((c->width ? 1 : 0) << 6) |
		       (1 << 7) | /* Electrical Throttling enabled */
		       (1 << 8)); /* DIMM present and compatible */
		printk(BIOS_DEBUG, "MTR0: %04x\n", val);
		pci_write_config16(c->branch->branchdev, mtr, val);
	}

	if (c->dimm[2].present || c->dimm[3].present) {
		val = (((c->columns - 10) & 3) |
		       (((c->rows - 13) & 3) << 4) |
		       ((c->ranks ? 1 : 0) << 4) |
		       (((c->banks == 8) ? 1 : 0) << 5) |
		       ((c->width ? 1 : 0) << 6) |
		       (1 << 7) | /* Electrical Throttling enabled */
		       (1 << 8)); /* DIMM present and compatible */
		printk(BIOS_DEBUG, "MTR1: %04x\n", val);
		pci_write_config16(c->branch->branchdev, mtr+2, val);
	}
}

static int get_dmir(u8 *rankmap, int *_set, int limit)
{
	int i, dmir = 0, set = 0;

	for(i = 7; set < limit && i >= 0; i--) {
		if (!(*rankmap & (1 << i)))
			continue;

		*rankmap &= ~(1 << i);

		switch(limit) {
		case 1:
			dmir |= (i |
				 (i << 3) |
				 (i << 6) |
				 (i << 9));
			break;
		case 2:
			dmir |= (i << (set * 3)) |
				(i << (6 + set * 3));
			break;
		case 4:
			dmir |= (i << (set * 3));
			break;

		default:
			break;
		}
		set++;
	}
	*_set = set;
	return dmir;
}

static int i5000_setup_dmir(struct i5000_fbd_branch *b)
{
	struct i5000_fbdimm *d;
	device_t dev = b->branchdev;
	u8 rankmap = 0, dmir = 0;
	u32 dmirval = 0;
	int i, set, rankoffset = 0, ranksize = 0, ranks = 0;

	if (!b->used)
		return 0;

	for(i = 0; i < I5000_MAX_DIMM_PER_CHANNEL; i++) {
		rankmap >>= 2;
		d = b->channel[0].dimm + i;

		if (!d->present)
			continue;

		if (d->ranks == 2) {
			rankmap |= 0xc0;
			ranks += 2;
		} else {
			rankmap |= 0x40;
			ranks++;
		}
	}

	printk(BIOS_DEBUG, "total ranks: %d, rankmap: %02x\n", ranks, rankmap);

	dmir = I5000_DMIR0;

	ranksize = b->channel[0].dimm[0].ranksize << 8;

	if (!b->setup->single_channel)
		ranksize <<= 1;

	while(ranks) {

		if (ranks >= 4)
			dmirval = get_dmir(&rankmap, &set, 4);
		else if (ranks >= 2)
			dmirval = get_dmir(&rankmap, &set, 2);
		else
			dmirval = get_dmir(&rankmap, &set, 1);

		ranks -= set;

		dmirval |= rankoffset + (set * ranksize);

		rankoffset += (set * ranksize);

		printk(BIOS_DEBUG, "DMIR%d: %08x\n", (dmir - I5000_DMIR0) >> 2,
		       dmirval);
		pci_write_config32(dev, dmir, dmirval);
		dmir += 4;
	}

	for(; dmir <= I5000_DMIR4; dmir += 4) {
		printk(BIOS_DEBUG, "DMIR%d: %08x\n", (dmir - I5000_DMIR0) >> 2,
		       dmirval);
		pci_write_config32(dev, dmir, dmirval);
	}
	return rankoffset;
}

static void i5000_setup_interleave(struct i5000_fbd_setup *setup)
{
	device_t dev16 = PCI_ADDR(0, 16, 1, 0);
	u32 mir0, mir1, mir2, size0, size1, minsize, tmp;

	size0 = i5000_setup_dmir(&setup->branch[1]) >> 12;
	size1 = i5000_setup_dmir(&setup->branch[0]) >> 12;

	minsize = MIN(size0, size1);

	if (size0 > size1) {
		tmp = size1;
		size1 = size0;
		size0 = tmp;
	}

	if (size0 == size1) {
		mir0 = (size0 << 1) | 3;
		mir1 = (size0 << 1);
		mir2 = (size0 << 1);
        } else if (!size0) {
		mir0 = size1 | 1;
		mir1 = size1;
		mir2 = size1;
	} else {
		mir0 = (size0 << 1) | 3;
		mir1 = (size1 + size0) | 1;
		mir2 = size1 + size0;
	}

	printk(BIOS_DEBUG, "MIR0: %04x\n", mir0);
	printk(BIOS_DEBUG, "MIR1: %04x\n", mir1);;
	printk(BIOS_DEBUG, "MIR2: %04x\n", mir2);;

	pci_write_config16(dev16, I5000_MIR0, mir0);
	pci_write_config16(dev16, I5000_MIR1, mir1);
	pci_write_config16(dev16, I5000_MIR2, mir2);
}

static int i5000_dram_timing_init(struct i5000_fbd_setup *setup)
{
	device_t dev16 = PCI_ADDR(0, 16, 1, 0);
	u32 tolm, drta, drtb, mc, mca;
	int t_wrc, bl2;

	bl2 = (setup->bl & BL_BL8) ? 4 :2;
	t_wrc = setup->t_rcd + (setup->t_cl - 1) + bl2 +
		setup->t_wr + setup->t_rp;

	drta = (setup->t_ref & 0x0f) |
		((setup->t_rrd & 0x0f) << 4) |
		((setup->t_rfc & 0xff) << 8) |
		((setup->t_rc & 0x3f) << 16) |
		((t_wrc & 0x3f) << 22) |
		(setup->t_al & 0x07) << 28;

	drtb = (bl2) |
		(((1 + bl2 + setup->t_r2r) & 0x0f) << 4) |
		(((setup->t_cl - 1 + bl2 + setup->t_wtr) & 0x0f) << 8) |
		(((2 + bl2 + setup->t_r2w) & 0x0f) << 12) |
		(((bl2 + setup->t_w2rdr) & 0x07) << 16);

	mc = (1 << 30) | /* enable retry */
		(3 << 25) | /* bad RAM threshold */
		(1 << 21) | /* INITDONE */
		(1 << 20) | /* FSB enable */
		/* Electrical throttling: 20 clocks */
		((setup->ddr_speed == DDR_667MHZ ? 1 : 0) << 18) |
		(1 << 8) | /* enhanced scrub mode */
		(1 << 7) | /* enable patrol scrub */
		(1 << 6) | /* enable demand scrubing */
		(1 << 5); /* enable northbound error detection */

	printk(BIOS_DEBUG, "DRTA: 0x%08x DRTB: 0x%08x MC: 0x%08x\n", drta, drtb, mc);
	pci_write_config32(dev16, I5000_DRTA, drta);
	pci_write_config32(dev16, I5000_DRTB, drtb);
	pci_write_config32(dev16, I5000_MC, mc);

	mca = pci_read_config32(dev16, I5000_MCA);

	mca |= (7 << 28);
	if (setup->single_channel)
		mca |= (1 << 14);
	else
		mca &= ~(1 << 14);
	printk(BIOS_DEBUG, "MCA: 0x%08x\n", mca);
	pci_write_config32(dev16, I5000_MCA, mca);

	pci_write_config32(dev16, I5000_ERRPERR, 0xffffffff);

	i5000_program_mtr(&setup->branch[0].channel[0], I5000_MTR0);
	i5000_program_mtr(&setup->branch[0].channel[1], I5000_MTR1);
	i5000_program_mtr(&setup->branch[1].channel[0], I5000_MTR0);
	i5000_program_mtr(&setup->branch[1].channel[1], I5000_MTR1);

	i5000_setup_interleave(setup);

	if ((tolm = MIN(setup->totalmem, 0xd00)) > 0xd00)
		tolm = 0xd00;

	tolm <<= 4;
	printk(BIOS_DEBUG, "TOLM: 0x%04x\n", tolm);
	pci_write_config16(dev16, I5000_TOLM, tolm);
	return 0;
}

static void i5000_init_setup(struct i5000_fbd_setup *setup)
{
	int branch, channel, dimm, i = 0;
	struct i5000_fbdimm *d;
	struct i5000_fbd_channel *c;
	struct i5000_fbd_branch *b;

	setup->bl = 3;
	/* default to highest memory frequency. If a module doesn't
	   support it, it will decrease this setting in spd_read */
	setup->ddr_speed = DDR_667MHZ;

	for(branch = 0; branch < I5000_MAX_BRANCH; branch++) {
		b = setup->branch + branch;
		b->branchdev = PCI_ADDR(0, branch ? 22 : 21, 0, 0);
		b->setup = setup;
		b->num = branch;

		for(channel = 0; channel < I5000_MAX_CHANNEL; channel++) {
			c = b->channel + channel;
			c->branch = b;
			c->setup = setup;
			c->num = channel;

			for(dimm = 0; dimm < I5000_MAX_DIMM_PER_CHANNEL; dimm++) {
				d = c->dimm + dimm;
				setup->dimms[i++] = d;
				d->channel = c;
				d->branch = b;
				d->setup = setup;
				d->num = dimm;
				d->ambase = (b->num << 16) | (c->num << 15) | (dimm << 11);
			}
		}
	}
}

static void i5000_reserved_register_init(struct i5000_fbd_setup *setup)
{
	/* register write captured from vendor BIOS, but undocument by Intel */
	pci_write_config32(PCI_ADDR(0, 16, 0, 0), I5000_PROCENABLE, 0x487f7c);

	pci_write_config32(PCI_ADDR(0, 16, 0, 0), 0xf4, 0x1588106);
	pci_write_config32(PCI_ADDR(0, 16, 0, 0), 0xfc, 0x80);
	pci_write_config32(PCI_ADDR(0, 16, 1, 0), 0x5c, 0x08);
	pci_write_config32(PCI_ADDR(0, 16, 0, 0), 0x70, 0xfe2c08d);
	pci_write_config32(PCI_ADDR(0, 16, 0, 0), 0x78, 0xfe2c08d);

	pci_write_config32(PCI_ADDR(0, 16, 0, 0), 0x140, 0x18000000);
	pci_write_config32(PCI_ADDR(0, 16, 0, 0), 0x440, 0x18000000);
	pci_write_config32(PCI_ADDR(0, 17, 0, 0), 0x18c, 0x18000000);
	pci_write_config32(PCI_ADDR(0, 16, 1, 0), 0x180, 0x18000000);
	pci_write_config32(PCI_ADDR(0, 17, 0, 0), 0x180, 0x87ffffff);

	pci_write_config32(PCI_ADDR(0, 0, 0, 0), 0x200, 0x18000000);
	pci_write_config32(PCI_ADDR(0, 4, 0, 0), 0x200, 0x18000000);
	pci_write_config32(PCI_ADDR(0, 0, 0, 0), 0x208, 0x18000000);
	pci_write_config32(PCI_ADDR(0, 4, 0, 0), 0x208, 0x18000000);

	pci_write_config32(PCI_ADDR(0, 17, 0, 0), 0x184, 0x01249249);
	pci_write_config32(PCI_ADDR(0, 17, 0, 0), 0x154, 0x00000000);
	pci_write_config32(PCI_ADDR(0, 17, 0, 0), 0x158, 0x02492492);
	pci_write_config32(PCI_ADDR(0, 17, 0, 0), 0x15c, 0x00000000);
	pci_write_config32(PCI_ADDR(0, 17, 0, 0), 0x160, 0x00000000);

	pci_write_config16(PCI_ADDR(0, 19, 0, 0), 0x0090, 0x00000007);
	pci_write_config16(PCI_ADDR(0, 19, 0, 0), 0x0092, 0x0000000f);

	pci_write_config8(PCI_ADDR(0, 16, 0, 0), 0x0154, 0x10);
	pci_write_config8(PCI_ADDR(0, 16, 0, 0), 0x0454, 0x10);

	pci_write_config32(PCI_ADDR(0, 19, 0, 0), 0x007C, 0x00000001);
	pci_write_config32(PCI_ADDR(0, 19, 0, 0), 0x007C, 0x00000000);
	pci_write_config32(PCI_ADDR(0, 17, 0, 0), 0x0108, 0x000003F0);
	pci_write_config32(PCI_ADDR(0, 17, 0, 0), 0x010C, 0x00000042);
	pci_write_config16(PCI_ADDR(0, 17, 0, 0), 0x0112, 0x0000);
	pci_write_config32(PCI_ADDR(0, 17, 0, 0), 0x0114, 0x00A0494C);
	pci_write_config32(PCI_ADDR(0, 17, 0, 0), 0x0118, 0x0002134C);
	pci_write_config32(PCI_ADDR(0, 17, 0, 0), 0x013C, 0x0C008000);
	pci_write_config32(PCI_ADDR(0, 17, 0, 0), 0x0140, 0x0C008000);
	pci_write_config32(PCI_ADDR(0, 17, 0, 0), 0x0144, 0x00008000);
	pci_write_config32(PCI_ADDR(0, 17, 0, 0), 0x0148, 0x00008000);
	pci_write_config32(PCI_ADDR(0, 19, 0, 0), 0x007C, 0x00000002);
	pci_write_config32(PCI_ADDR(0, 16, 1, 0), 0x01F4, 0x00000800);

	if (setup->branch[0].channel[0].used)
		pci_write_config32(PCI_ADDR(0, 21, 0, 0), 0x010C, 0x004C0C10);

	if (setup->branch[0].channel[1].used)
		pci_write_config32(PCI_ADDR(0, 21, 0, 0), 0x020C, 0x004C0C10);

	if (setup->branch[1].channel[0].used)
		pci_write_config32(PCI_ADDR(0, 22, 0, 0), 0x010C, 0x004C0C10);

	if (setup->branch[1].channel[1].used)
		pci_write_config32(PCI_ADDR(0, 22, 0, 0), 0x020C, 0x004C0C10);
}
static void i5000_dump_error_registers(void)
{
	device_t dev = PCI_ADDR(0, 16, 1, 0);

	printk(BIOS_ERR, "Dump of FBD error registers:\n"
	       "FERR_FAT_FBD: 0x%08x NERR_FAT_FBD: 0x%08x\n"
	       "FERR_NF_FBD:  0x%08x NERR_NF_FBD:  0x%08x\n"
	       "EMASK_FBD:    0x%08x\n"
	       "ERR0_FBD:     0x%08x\n"
	       "ERR1_FBD:     0x%08x\n"
	       "ERR2_FBD:     0x%08x\n"
	       "MC_ERR_FBD:   0x%08x\n",
	       pci_read_config32(dev, I5000_FERR_FAT_FBD),
	       pci_read_config32(dev, I5000_NERR_FAT_FBD),
	       pci_read_config32(dev, I5000_FERR_NF_FBD),
	       pci_read_config32(dev, I5000_NERR_NF_FBD),
	       pci_read_config32(dev, I5000_EMASK_FBD),
	       pci_read_config32(dev, I5000_ERR0_FBD),
	       pci_read_config32(dev, I5000_ERR1_FBD),
	       pci_read_config32(dev, I5000_ERR2_FBD),
	       pci_read_config32(dev, I5000_MCERR_FBD));

	printk(BIOS_ERR, "Non recoverable error registers:\n"
	       "NRECMEMA:     0x%08x NRECMEMB:    0x%08x\n"
	       "NRECFGLOG:    0x%08x\n",
	       pci_read_config32(dev, I5000_NRECMEMA),
	       pci_read_config32(dev, I5000_NRECMEMB),
	       pci_read_config32(dev, I5000_NRECFGLOG));

	printk(BIOS_ERR, "Packet data:\n"
	       "NRECFBDA: 0x%08x\n"
	       "NRECFBDB: 0x%08x\n"
	       "NRECFBDC: 0x%08x\n"
	       "NRECFBDD: 0x%08x\n"
	       "NRECFBDE: 0x%08x\n",
	       pci_read_config32(dev, I5000_NRECFBDA),
	       pci_read_config32(dev, I5000_NRECFBDB),
	       pci_read_config32(dev, I5000_NRECFBDC),
	       pci_read_config32(dev, I5000_NRECFBDD),
	       pci_read_config32(dev, I5000_NRECFBDE));

	printk(BIOS_ERR, "recoverable error registers:\n"
	       "RECMEMA:     0x%08x RECMEMB:    0x%08x\n"
	       "RECFGLOG:    0x%08x\n",
	       pci_read_config32(dev, I5000_RECMEMA),
	       pci_read_config32(dev, I5000_RECMEMB),
	       pci_read_config32(dev, I5000_RECFGLOG));

	printk(BIOS_ERR, "Packet data:\n"
	       "RECFBDA: 0x%08x\n"
	       "RECFBDB: 0x%08x\n"
	       "RECFBDC: 0x%08x\n"
	       "RECFBDD: 0x%08x\n"
	       "RECFBDE: 0x%08x\n",
	       pci_read_config32(dev, I5000_RECFBDA),
	       pci_read_config32(dev, I5000_RECFBDB),
	       pci_read_config32(dev, I5000_RECFBDC),
	       pci_read_config32(dev, I5000_RECFBDD),
	       pci_read_config32(dev, I5000_RECFBDE));

}

static void i5000_try_restart(const char *msg)
{
	printk(BIOS_INFO, msg);
	i5000_dump_error_registers();
	outb(0x06, 0xcf9);
	for(;;) asm volatile("hlt");
}

static void i5000_pam_setup(void)
{
	pci_write_config8(PCI_ADDR(0, 16, 0, 0), 0x59, 0x30);
	pci_write_config8(PCI_ADDR(0, 16, 0, 0), 0x5a, 0x33);
	pci_write_config8(PCI_ADDR(0, 16, 0, 0), 0x5b, 0x33);
	pci_write_config8(PCI_ADDR(0, 16, 0, 0), 0x5c, 0x33);
	pci_write_config8(PCI_ADDR(0, 16, 0, 0), 0x5d, 0x33);
	pci_write_config8(PCI_ADDR(0, 16, 0, 0), 0x5e, 0x33);
	pci_write_config8(PCI_ADDR(0, 16, 0, 0), 0x5f, 0x33);
}

static int i5000_setup_clocking(struct i5000_fbd_setup *setup)
{
	int fbd, fsb, ddrfrq, ddrfrqnow;
	msr_t msr;
	device_t dev = PCI_ADDR(0, 16, 1, 0);

	switch(setup->ddr_speed) {
	case DDR_667MHZ:
		fbd = 667;
		break;
	case DDR_533MHZ:
		fbd = 533;
		break;
	default:
		printk(BIOS_ERR, "%s: unsupported FBD speed\n", __func__);
		return 1;
	}

	/* mainboard specific callback */
	if (mainboard_set_fbd_clock(fbd)) {
		printk(BIOS_ERR, "%s: failed to set FBD speed\n", __func__);
		return 1;
	}

	msr = rdmsr(MSR_FSB_FREQ);

	switch(msr.lo & 7) {
	case 1:
		fsb = 533;
		break;
	case 4:
		fsb = 667;
		break;
	default:
		printk(BIOS_ERR, "%s: unsupported FSB speed: %d\n", __func__, msr.lo & 7);
		return 1;
	}


	ddrfrq = pci_read_config8(PCI_ADDR(0, 16, 1, 0), 0x56);
	ddrfrqnow = ddrfrq;
	ddrfrq &= ~0x3;

	if (fsb < fbd)
		ddrfrq |= 2;
	else if (fsb > fbd)
		ddrfrq |= 3;

	switch((ddrfrq >> 4) & 3) {
	case 0: /* 1:1 mapping */
		pci_write_config32(dev, I5000_FBDTOHOSTGRCFG0, 0xffffffff);
		pci_write_config32(dev, I5000_FBDTOHOSTGRCFG1, 0x00000000);
		pci_write_config32(dev, I5000_HOSTTOFBDGRCFG, 0xffffffff);
		pci_write_config8(dev, I5000_GRFBDLVLDCFG, 0x00);
		pci_write_config8(dev, I5000_GRHOSTFULLCFG, 0x00);
		pci_write_config8(dev, I5000_GRBUBBLECFG, 0x00);
		pci_write_config8(dev, I5000_GRFBDTOHOSTDBLCFG, 0x00);
		break;
	case 2: /* 4:5 mapping */
		pci_write_config32(dev, I5000_FBDTOHOSTGRCFG0, 0x00002323);
		pci_write_config32(dev, I5000_FBDTOHOSTGRCFG1, 0x00000400);
		pci_write_config32(dev, I5000_HOSTTOFBDGRCFG, 0x23023);
		pci_write_config8(dev, I5000_GRFBDLVLDCFG, 0x04);
		pci_write_config8(dev, I5000_GRHOSTFULLCFG, 0x08);
		pci_write_config8(dev, I5000_GRBUBBLECFG, 0x00);
		pci_write_config8(dev, I5000_GRFBDTOHOSTDBLCFG, 0x04);
		break;
	case 3:
		/* 5:4 mapping */
		pci_write_config32(dev, I5000_FBDTOHOSTGRCFG0, 0x00023230);
		pci_write_config32(dev, I5000_FBDTOHOSTGRCFG1, 0x00000000);
		pci_write_config32(dev, I5000_HOSTTOFBDGRCFG, 0x4323);
		pci_write_config8(dev, I5000_GRFBDLVLDCFG, 0x00);
		pci_write_config8(dev, I5000_GRHOSTFULLCFG, 0x02);
		pci_write_config8(dev, I5000_GRBUBBLECFG, 0x10);
		pci_write_config8(dev, I5000_GRFBDTOHOSTDBLCFG, 0x00);
		break;
	default:
		printk(BIOS_DEBUG, "invalid DDRFRQ: %02x\n", ddrfrq);
		return -1;
	}

	if (ddrfrq != ddrfrqnow) {
		printk(BIOS_DEBUG, "old DDRFRQ: 0x%02x new DDRFRQ: 0x%02x\n",
		       ddrfrqnow, ddrfrq);
		pci_write_config8(PCI_ADDR(0, 16, 1, 0), 0x56, ddrfrq);
		/* FSB:FBD mapping changed, needs hard reset */
		outb(0x06, 0xcf9);
		for(;;) asm volatile("hlt");
	}
	return 0;
}

void i5000_fbdimm_init(void)
{
	struct i5000_fbd_setup setup;
	u32 mca, mc;

	memset(&setup, 0, sizeof(setup));

	pci_write_config16(PCI_ADDR(0, 0, 0, 0), 0x4, 0x144);

	i5000_init_setup(&setup);

	pci_write_config32(PCI_DEV(0, 16, 0), 0xf0,
			   pci_read_config32(PCI_ADDR(0, 16, 0, 0), 0xf0) | 0x8000);

	i5000_clear_fbd_errors();

	printk(BIOS_INFO, "detecting memory modules\n");
	if (i5000_for_each_dimm(&setup, i5000_read_spd_data)) {
		printk(BIOS_ERR, "%s: failed to read SPD data\n", __func__);
		return;
	}

	if (i5000_setup_clocking(&setup)) {
		printk(BIOS_ERR, "%s: failed to set FBD clock\n", __func__);
		return;
	}

	/* posted CAS requires t_AL = t_RCD - 1 */
	setup.t_al = setup.t_rcd - 1;

	printk(BIOS_DEBUG, "global timing parameters:\n"
	       "CL: %d RAS: %d WRC: %d RC: %d RFC: %d RRD: %d REF: %d W2RDR: %d\n"
	       "R2W: %d W2R: %d R2R: %d W2W: %d WTR: %d RCD: %d RP %d WR: %d RTP: %d AL: %d\n",
	       setup.t_cl, setup.t_ras, setup.t_wrc, setup.t_rc, setup.t_rfc,
	       setup.t_rrd, setup.t_ref, setup.t_w2rdr, setup.t_r2w, setup.t_w2r,
	       setup.t_r2r, setup.t_w2w, setup.t_wtr, setup.t_rcd,
	       setup.t_rp, setup.t_wr, setup.t_rtp, setup.t_al);

	setup.single_channel = (!(setup.branch[0].channel[1].used ||
				  setup.branch[1].channel[0].used ||
				  setup.branch[1].channel[1].used));

	pci_write_config32(PCI_ADDR(0, 16, 1, 0), 0x019C, 0x8010c);
	pci_write_config32(PCI_ADDR(0, 16, 1, 0), 0x01F4, 0);

	/* enable or disable single channel mode */
	mca = pci_read_config32(PCI_ADDR(0, 16, 1, 0), I5000_MCA);
	if (setup.single_channel)
		mca |= (1 << 14);
	else
		mca &= ~(1 << 14);
	pci_write_config32(PCI_ADDR(0, 16, 1, 0), I5000_MCA, mca);

	/*
	 * i5000 supports burst length 8 only in single channel mode
	 * so strip BL_BL8 if we're operating in multichannel mode
	 */

	if (!setup.single_channel)
		setup.bl &= ~BL_BL8;

	if (!setup.bl)
		die("No supported burst length found\n");

	mc = pci_read_config32(PCI_ADDR(0, 16, 1, 0), I5000_MC);
	/* disable error checking for training */
	pci_write_config32(PCI_ADDR(0, 16, 1, 0), I5000_MC, mc & ~0x20);

	printk(BIOS_INFO, "performing fbd link initialization...");
	if (i5000_for_each_branch(&setup, i5000_branch_reset) ||
	    i5000_for_each_dimm_present(&setup, i5000_amb_preinit) ||
	    i5000_for_each_branch(&setup, i5000_link_training0) ||
	    i5000_for_each_dimm_present(&setup, i5000_amb_check) ||
	    i5000_for_each_dimm_present(&setup, i5000_amb_postinit) ||
	    i5000_for_each_branch(&setup, i5000_link_training1)) {
		i5000_try_restart("failed\n");
	}
	printk(BIOS_INFO, "done\n");
	printk(BIOS_INFO, "initializing memory...");

	if (i5000_for_each_dimm_present(&setup, i5000_ddr_init) ||
	    i5000_for_each_dimm_present(&setup, i5000_amb_dram_timing_init) ||
	    i5000_for_each_dimm_present(&setup, i5000_ddr_calibration)) {
		i5000_try_restart("failed\n");
	}
	printk(BIOS_INFO,"done\n");
	printk(BIOS_INFO, "clearing memory...");

	if (i5000_membist(&setup))
		i5000_try_restart("failed\n");
	else
		printk(BIOS_INFO, "done\n");

	if (i5000_for_each_dimm_present(&setup, i5000_enable_mc_autorefresh))
		i5000_try_restart("failed to enable auto refresh\n");

	i5000_fbd_next_state(&setup.branch[0], I5000_FBDHPC_STATE_INIT);
	i5000_fbd_next_state(&setup.branch[1], I5000_FBDHPC_STATE_INIT);

	if (i5000_for_each_branch(&setup, i5000_link_training0))
		i5000_try_restart("Channel training failed\n");

	if (setup.branch[0].used)
		i5000_fbd_next_state(&setup.branch[0], I5000_FBDHPC_STATE_READY);

	if (setup.branch[1].used)
		i5000_fbd_next_state(&setup.branch[1], I5000_FBDHPC_STATE_READY);

	i5000_clear_fbd_errors();

	/* enable error checking */
	pci_write_config32(PCI_ADDR(0, 16, 1, 0), I5000_MC, mc | 0x20);

	i5000_dram_timing_init(&setup);

	i5000_reserved_register_init(&setup);

	i5000_pam_setup();

	if (i5000_for_each_dimm_present(&setup, i5000_amb_clear_error_status))
		i5000_try_restart("failed to clear error status\n");

	if (setup.branch[0].used)
		i5000_fbd_next_state(&setup.branch[0], I5000_FBDHPC_STATE_ACTIVE);

	if (setup.branch[1].used)
		i5000_fbd_next_state(&setup.branch[1], I5000_FBDHPC_STATE_ACTIVE);

#if CONFIG_NORTHBRIDGE_INTEL_I5000_RAM_CHECK
	if (ram_check_nodie(0x000000, 0x0a0000) ||
	    ram_check_nodie(0x100000, MIN(setup.totalmem * 1048576, 0xd0000000))) {
		i5000_try_restart("RAM verification failed");

	}
#endif

	printk(BIOS_INFO, "Memory initialization finished\n");
}
