/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2005 Eric W. Biederman and Tom Zimmerman
 * Copyright (C) 2008 Arastra, Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
 *
 */

#include <cpu/x86/mtrr.h>
#include <cpu/x86/cache.h>
#include <cpu/intel/speedstep.h>
#include <stdlib.h>
#include "raminit.h"
#include "i3100.h"

/* DDR2 memory controller register space */
#define MCBAR 0x90000000

static void sdram_set_registers(const struct mem_controller *ctrl)
{
	static const u32 register_values[] = {

		/* CKDIS 0x8c disable clocks */
	PCI_ADDR(0, 0x00, 0, CKDIS), 0xffff0000, 0x0000ffff,

		/* 0x9c Device present and extended RAM control
		 * DEVPRES is very touchy, hard code the initialization
		 * of PCI-E ports here.
		 */
	PCI_ADDR(0, 0x00, 0, DEVPRES), 0x00000000, 0x07020801 | DEVPRES_CONFIG,

		/* 0xc8 Remap RAM base and limit off */
	PCI_ADDR(0, 0x00, 0, REMAPLIMIT), 0x00000000, 0x03df0000,

		/* ??? */
	PCI_ADDR(0, 0x00, 0, 0xd8), 0x00000000, 0xb5930000,
	PCI_ADDR(0, 0x00, 0, 0xe8), 0x00000000, 0x00004a2a,

		/* 0x50 scrub */
	PCI_ADDR(0, 0x00, 0, MCHCFG0), 0xfce0ffff, 0x00006000, /* 6000 */

		/* 0x58 0x5c PAM */
	PCI_ADDR(0, 0x00, 0, PAM-1), 0xcccccc7f, 0x33333000,
	PCI_ADDR(0, 0x00, 0, PAM+3), 0xcccccccc, 0x33333333,

		/* 0xf4 */
	PCI_ADDR(0, 0x00, 0, DEVPRES1), 0xffbffff, (1<<22)|(6<<2) | DEVPRES1_CONFIG,

		/* 0x14 */
	PCI_ADDR(0, 0x00, 0, IURBASE), 0x00000fff, MCBAR |0,
	};
	int i;
	int max;

	max = ARRAY_SIZE(register_values);
	for(i = 0; i < max; i += 3) {
		device_t dev;
		u32 where;
		u32 reg;
		dev = (register_values[i] & ~0xff) - PCI_DEV(0, 0x00, 0) + ctrl->f0;
		where = register_values[i] & 0xff;
		reg = pci_read_config32(dev, where);
		reg &= register_values[i+1];
		reg |= register_values[i+2];
		pci_write_config32(dev, where, reg);
	}
	print_spew("done.\n");
}

struct dimm_size {
	u32 side1;
	u32 side2;
};

static struct dimm_size spd_get_dimm_size(u16 device)
{
	/* Calculate the log base 2 size of a DIMM in bits */
	struct dimm_size sz;
	int value, low;
	sz.side1 = 0;
	sz.side2 = 0;

	/* Note it might be easier to use byte 31 here, it has the DIMM size as
	 * a multiple of 4MB.  The way we do it now we can size both
	 * sides of an assymetric dimm.
	 */
	value = spd_read_byte(device, 3);	/* rows */
	if (value < 0) goto hw_err;
	if ((value & 0xf) == 0) goto val_err;
	sz.side1 += value & 0xf;

	value = spd_read_byte(device, 4);	/* columns */
	if (value < 0) goto hw_err;
	if ((value & 0xf) == 0) goto val_err;
	sz.side1 += value & 0xf;

	value = spd_read_byte(device, 17);	/* banks */
	if (value < 0) goto hw_err;
	if ((value & 0xff) == 0) goto val_err;
	sz.side1 += log2(value & 0xff);

	/* Get the module data width and convert it to a power of two */
	value = spd_read_byte(device, 7);	/* (high byte) */
	if (value < 0) goto hw_err;
	value &= 0xff;
	value <<= 8;

	low = spd_read_byte(device, 6);	/* (low byte) */
	if (low < 0) goto hw_err;
	value = value | (low & 0xff);
	if ((value != 72) && (value != 64)) goto val_err;
	sz.side1 += log2(value);

	/* side 2 */
	value = spd_read_byte(device, 5);	/* number of physical banks */

	if (value < 0) goto hw_err;
	value &= 7;
	value++;
	if (value == 1) goto out;
	if (value != 2) goto val_err;

	/* Start with the symmetrical case */
	sz.side2 = sz.side1;

	value = spd_read_byte(device, 3);	/* rows */
	if (value < 0) goto hw_err;
	if ((value & 0xf0) == 0) goto out;	/* If symmetrical we are done */
	sz.side2 -= (value & 0x0f);		/* Subtract out rows on side 1 */
	sz.side2 += ((value >> 4) & 0x0f);	/* Add in rows on side 2 */

	value = spd_read_byte(device, 4);	/* columns */
	if (value < 0) goto hw_err;
	if ((value & 0xff) == 0) goto val_err;
	sz.side2 -= (value & 0x0f);		/* Subtract out columns on side 1 */
	sz.side2 += ((value >> 4) & 0x0f);	/* Add in columns on side 2 */
	goto out;

 val_err:
	die("Bad SPD value\n");
	/* If an hw_error occurs report that I have no memory */
 hw_err:
	sz.side1 = 0;
	sz.side2 = 0;
out:
	return sz;

}

static long spd_set_ram_size(const struct mem_controller *ctrl, long dimm_mask)
{
	int i;
	int cum;

	for(i = cum = 0; i < DIMM_SOCKETS; i++) {
		struct dimm_size sz;
		if (dimm_mask & (1 << i)) {
			sz = spd_get_dimm_size(ctrl->channel0[i]);
			if (sz.side1 < 29) {
				return -1; /* Report SPD error */
			}
			/* convert bits to multiples of 64MB */
			sz.side1 -= 29;
			cum += (1 << sz.side1);
			/* DRB = 0x60 */
			pci_write_config8(ctrl->f0, DRB + (i*2), cum);
			if( sz.side2 > 28) {
				sz.side2 -= 29;
				cum += (1 << sz.side2);
			}
			pci_write_config8(ctrl->f0, DRB+1 + (i*2), cum);
		}
		else {
			pci_write_config8(ctrl->f0, DRB + (i*2), cum);
			pci_write_config8(ctrl->f0, DRB+1 + (i*2), cum);
		}
	}
	cum >>= 1;
	/* set TOM top of memory 0xcc */
	pci_write_config16(ctrl->f0, TOM, cum);
	/* set TOLM top of low memory */
	if(cum > 0x18) {
		cum = 0x18;
	}
	cum <<= 11;
	/* 0xc4 TOLM */
	pci_write_config16(ctrl->f0, TOLM, cum);
	return 0;
}


static u32 spd_detect_dimms(const struct mem_controller *ctrl)
{
	u32 dimm_mask;
	int i;
	dimm_mask = 0;
	for(i = 0; i < DIMM_SOCKETS; i++) {
		int byte;
		u16 device;
		device = ctrl->channel0[i];
		if (device) {
			byte = spd_read_byte(device, 2);  /* Type */
			if (byte == 8) {
				dimm_mask |= (1 << i);
			}
		}
		device = ctrl->channel1[i];
		if (device) {
			byte = spd_read_byte(device, 2);
			if (byte == 8) {
				dimm_mask |= (1 << (i + DIMM_SOCKETS));
			}
		}
	}
	return dimm_mask;
}


static int spd_set_row_attributes(const struct mem_controller *ctrl,
		long dimm_mask)
{
	int value;
	int reg;
	int dra;
	int cnt;

	dra = 0;
	for(cnt=0; cnt < 4; cnt++) {
		if (!(dimm_mask & (1 << cnt))) {
			continue;
		}
		reg =0;
		value = spd_read_byte(ctrl->channel0[cnt], 3);	/* rows */
		if (value < 0) goto hw_err;
		if ((value & 0xf) == 0) goto val_err;
		reg += value & 0xf;

		value = spd_read_byte(ctrl->channel0[cnt], 4);	/* columns */
		if (value < 0) goto hw_err;
		if ((value & 0xf) == 0) goto val_err;
		reg += value & 0xf;

		value = spd_read_byte(ctrl->channel0[cnt], 17);	/* banks */
		if (value < 0) goto hw_err;
		if ((value & 0xff) == 0) goto val_err;
		reg += log2(value & 0xff);

		/* Get the device width and convert it to a power of two */
		value = spd_read_byte(ctrl->channel0[cnt], 13);
		if (value < 0) goto hw_err;
		value = log2(value & 0xff);
		reg += value;
		if(reg < 27) goto hw_err;
		reg -= 27;
		reg += (value << 2);

		dra += reg << (cnt*8);
		value = spd_read_byte(ctrl->channel0[cnt], 5);
		if (value & 2)
			dra += reg << ((cnt*8)+4);
	}

	/* 0x70 DRA */
	pci_write_config32(ctrl->f0, DRA, dra);
	goto out;

 val_err:
	die("Bad SPD value\n");
	/* If an hw_error occurs report that I have no memory */
 hw_err:
	dra = 0;
out:
	return dra;

}


static int spd_set_drt_attributes(const struct mem_controller *ctrl,
		long dimm_mask, u32 drc)
{
	int value;
	int reg;
	u32 drt;
	int cnt;
	int first_dimm;
	int cas_latency=0;
	int latency;
	u32 index = 0;
	u32 index2 = 0;
	static const u8 cycle_time[3] = { 0x75, 0x60, 0x50 };
	static const u8 latency_indicies[] = { 26, 23, 9 };

	/* 0x78 DRT */
	drt = pci_read_config32(ctrl->f0, DRT);
	drt &= 3;  /* save bits 1:0 */

	for(first_dimm = 0; first_dimm < 4; first_dimm++) {
		if (dimm_mask & (1 << first_dimm))
			break;
	}

	drt |= (1<<6); /* back to back write turn around */

	drt |= (3<<18);  /* Trasmax */

	for(cnt=0; cnt < 4; cnt++) {
		if (!(dimm_mask & (1 << cnt))) {
			continue;
		}
		reg = spd_read_byte(ctrl->channel0[cnt], 18); /* CAS Latency */
		/* Compute the lowest cas latency supported */
		latency = log2(reg) -2;

		/* Loop through and find a fast clock with a low latency */
		for(index = 0; index < 3; index++, latency++) {
			if ((latency < 2) || (latency > 4) ||
				(!(reg & (1 << latency)))) {
				continue;
			}
			value = spd_read_byte(ctrl->channel0[cnt],
					latency_indicies[index]);

			if(value <= cycle_time[drc&3]) {
				if( latency > cas_latency) {
					cas_latency = latency;
				}
				break;
			}
		}
	}
	index = (cas_latency-2);
	if((index)==0) cas_latency = 20;
	else if((index)==1) cas_latency = 25;
	else cas_latency = 30;

	for(cnt=0;cnt<4;cnt++) {
		if (!(dimm_mask & (1 << cnt))) {
			continue;
		}
		reg = spd_read_byte(ctrl->channel0[cnt], 27)&0x0ff;
		if(((index>>8)&0x0ff)<reg) {
			index &= ~(0x0ff << 8);
			index |= (reg << 8);
		}
		reg = spd_read_byte(ctrl->channel0[cnt], 28)&0x0ff;
		if(((index>>16)&0x0ff)<reg) {
			index &= ~(0x0ff << 16);
			index |= (reg<<16);
		}
		reg = spd_read_byte(ctrl->channel0[cnt], 29)&0x0ff;
		if(((index2>>0)&0x0ff)<reg) {
			index2 &= ~(0x0ff << 0);
			index2 |= (reg<<0);
		}
		reg = spd_read_byte(ctrl->channel0[cnt], 41)&0x0ff;
		if(((index2>>8)&0x0ff)<reg) {
			index2 &= ~(0x0ff << 8);
			index2 |= (reg<<8);
		}
		reg = spd_read_byte(ctrl->channel0[cnt], 42)&0x0ff;
		if(((index2>>16)&0x0ff)<reg) {
			index2 &= ~(0x0ff << 16);
			index2 |= (reg<<16);
		}
	}

	/* get dimm speed */
	value = cycle_time[drc&3];
	if(value <= 0x50) {  /* 200 MHz */
		if((index&7) > 2) {
			drt |= (2<<2);  /* CAS latency 4 */
			cas_latency = 40;
		} else {
			drt |= (1<<2);  /* CAS latency 3 */
			cas_latency = 30;
		}
		if((index&0x0ff00)<=0x03c00) {
			drt |= (1<<8);  /* Trp RAS Precharg */
		} else {
			drt |= (2<<8);  /* Trp RAS Precharg */
		}

		/* Trcd RAS to CAS delay */
		if((index2&0x0ff)<=0x03c) {
			drt |= (0<<10);
		} else {
			drt |= (1<<10);
		}

		/* Tdal Write auto precharge recovery delay */
		drt |= (1<<12);

		/* Trc TRS min */
		if((index2&0x0ff00)<=0x03700)
			drt |= (0<<14);
		else if((index2&0xff00)<=0x03c00)
			drt |= (1<<14);
		else
			drt |= (2<<14); /* spd 41 */

		drt |= (2<<16);  /* Twr not defined for DDR docs say use 2 */

		/* Trrd Row Delay */
		if((index&0x0ff0000)<=0x0140000) {
			drt |= (0<<20);
		} else if((index&0x0ff0000)<=0x0280000) {
			drt |= (1<<20);
		} else if((index&0x0ff0000)<=0x03c0000) {
			drt |= (2<<20);
		} else {
			drt |= (3<<20);
		}

		/* Trfc Auto refresh cycle time */
		if((index2&0x0ff0000)<=0x04b0000) {
			drt |= (0<<22);
		} else if((index2&0x0ff0000)<=0x0690000) {
			drt |= (1<<22);
		} else {
			drt |= (2<<22);
		}
		/* Docs say use 55 for all 200Mhz */
		drt |= (0x055<<24);
	}
	else if(value <= 0x60) { /* 167 Mhz */
		/* according to new documentation CAS latency is 00
		 * for bits 3:2 for all 167 Mhz
		drt |= ((index&3)<<2); */  /* set CAS latency */
		if((index&0x0ff00)<=0x03000) {
			drt |= (1<<8);  /* Trp RAS Precharg */
		} else {
			drt |= (2<<8);  /* Trp RAS Precharg */
		}

		/* Trcd RAS to CAS delay */
		if((index2&0x0ff)<=0x030) {
			drt |= (0<<10);
		} else {
			drt |= (1<<10);
		}

		/* Tdal Write auto precharge recovery delay */
		drt |= (2<<12);

		/* Trc TRS min */
		drt |= (2<<14); /* spd 41, but only one choice */

		drt |= (2<<16);  /* Twr not defined for DDR docs say 2 */

		/* Trrd Row Delay */
		if((index&0x0ff0000)<=0x0180000) {
			drt |= (0<<20);
		} else if((index&0x0ff0000)<=0x0300000) {
			drt |= (1<<20);
		} else {
			drt |= (2<<20);
		}

		/* Trfc Auto refresh cycle time */
		if((index2&0x0ff0000)<=0x0480000) {
			drt |= (0<<22);
		} else if((index2&0x0ff0000)<=0x0780000) {
			drt |= (2<<22);
		} else {
			drt |= (2<<22);
		}
		/* Docs state to use 99 for all 167 Mhz */
		drt |= (0x099<<24);
	}
	else if(value <= 0x75) { /* 133 Mhz */
		drt |= ((index&3)<<2);  /* set CAS latency */
		if((index&0x0ff00)<=0x03c00) {
			drt |= (1<<8);  /* Trp RAS Precharg */
		} else {
			drt |= (2<<8);  /* Trp RAS Precharg */
		}

		/* Trcd RAS to CAS delay */
		if((index2&0x0ff)<=0x03c) {
			drt |= (0<<10);
		} else {
			drt |= (1<<10);
		}

		/* Tdal Write auto precharge recovery delay */
		drt |= (1<<12);

		/* Trc TRS min */
		drt |= (2<<14); /* spd 41, but only one choice */

		drt |= (1<<16);  /* Twr not defined for DDR docs say 1 */

		/* Trrd Row Delay */
		if((index&0x0ff0000)<=0x01e0000) {
			drt |= (0<<20);
		} else if((index&0x0ff0000)<=0x03c0000) {
			drt |= (1<<20);
		} else {
			drt |= (2<<20);
		}

		/* Trfc Auto refresh cycle time */
		if((index2&0x0ff0000)<=0x04b0000) {
			drt |= (0<<22);
		} else if((index2&0x0ff0000)<=0x0780000) {
			drt |= (2<<22);
		} else {
			drt |= (2<<22);
		}

		/* Based on CAS latency */
		if(index&7)
			drt |= (0x099<<24);
		else
			drt |= (0x055<<24);

	}
	else {
		die("Invalid SPD 9 bus speed.\n");
	}

	/* 0x78 DRT */
	pci_write_config32(ctrl->f0, DRT, drt);

	return(cas_latency);
}

static int spd_set_dram_controller_mode(const struct mem_controller *ctrl,
		long dimm_mask)
{
	int value;
	int drc;
	int cnt;
	msr_t msr;
	u8 rate = 62;
	static const u8 spd_rates[6] = {15,3,7,7,62,62};
	static const u8 drc_rates[5] = {0,15,7,62,3};
	static const u8 fsb_conversion[8] = {0,1,3,2,3,0,3,3};

	/* 0x7c DRC */
	drc = pci_read_config32(ctrl->f0, DRC);
	for(cnt=0; cnt < 4; cnt++) {
		if (!(dimm_mask & (1 << cnt))) {
			continue;
		}
		value = spd_read_byte(ctrl->channel0[cnt], 11);	/* ECC */
		if (value != 2) die("ERROR - Non ECC memory dimm\n");

		value = spd_read_byte(ctrl->channel0[cnt], 12);	/*refresh rate*/
		value &= 0x0f;    /* clip self refresh bit */
		if (value > 5) goto hw_err;
		if (rate > spd_rates[value])
			rate = spd_rates[value];

		value = spd_read_byte(ctrl->channel0[cnt], 9);	/* cycle time */
		if (value > 0x75) goto hw_err;
	}
	drc |= (1 << 20); /* enable ECC */
	for (cnt = 1; cnt < 5; cnt++)
		if (drc_rates[cnt] == rate)
			break;
	if (cnt < 5) {
		drc &= ~(7 << 8);  /* clear the rate bits */
		drc |= (cnt << 8);
	}

	drc |= (1 << 26); /* set the overlap bit - the factory BIOS does */
	drc |= (1 << 27); /* set DED retry enable - the factory BIOS does */
	drc |= (1 << 7);
	drc &= ~(1 << 5); /* enable ODT */
	drc |= (1 << 4); /* independent clocks */

	/* set front side bus speed */
	msr = rdmsr(MSR_FSB_FREQ); /* returns 0 on Pentium M 90nm */
	value = msr.lo & 0x07;
	drc &= ~(3 << 2);
	drc |= (fsb_conversion[value] << 2);

	/* set dram type to ddr2 */
	drc &= ~(3 << 0);
	drc |= (2 << 0);

	goto out;

	/* If an hw_error occurs report that I have no memory */
 hw_err:
	drc = 0;
out:
	return drc;
}

static void sdram_set_spd_registers(const struct mem_controller *ctrl)
{
	long dimm_mask;

	/* Test if we can read the spd and if ram is ddr or ddr2 */
	dimm_mask = spd_detect_dimms(ctrl);
	if (!(dimm_mask & ((1 << DIMM_SOCKETS) - 1))) {
		print_err("No memory for this cpu\n");
		return;
	}
	return;
}

static void do_delay(void)
{
	int i;
	u8 b;
	for(i=0;i<16;i++)
		b=inb(0x80);
}

#define TIMEOUT_LOOPS 300000

#define DCALCSR  0x100
#define DCALADDR 0x104
#define DCALDATA 0x108

static void set_on_dimm_termination_enable(const struct mem_controller *ctrl)
{
	u8 c1,c2;
	u32 dimm,i;
	u32 data32;
	u32 t4;

	/* Set up northbridge values */
	/* ODT enable */
  	pci_write_config32(ctrl->f0, SDRC, 0x30000000);
	/* Figure out which slots are Empty, Single, or Double sided */
	for(i=0,t4=0,c2=0;i<8;i+=2) {
		c1 = pci_read_config8(ctrl->f0, DRB+i);
		if(c1 == c2) continue;
		c2 = pci_read_config8(ctrl->f0, DRB+1+i);
		if(c1 == c2)
			t4 |= (1 << (i*4));
		else
			t4 |= (2 << (i*4));
	}
	for(i=0;i<1;i++) {
	    if((t4&0x0f) == 1) {
		if( ((t4>>8)&0x0f) == 0 ) {
			data32 = 0x00000010; /* EEES */
			break;
		}
		if ( ((t4>>16)&0x0f) == 0 ) {
			data32 = 0x00003132; /* EESS */
			break;
		}
		if ( ((t4>>24)&0x0f)  == 0 ) {
			data32 = 0x00335566; /* ESSS */
			break;
		}
		data32 = 0x77bbddee; /* SSSS */
		break;
	    }
	    if((t4&0x0f) == 2) {
		if( ((t4>>8)&0x0f) == 0 ) {
			data32 = 0x00003132; /* EEED */
			break;
		}
		if ( ((t4>>8)&0x0f) == 2 ) {
			data32 = 0xb373ecdc; /* EEDD */
			break;
		}
		if ( ((t4>>16)&0x0f) == 0 ) {
			data32 = 0x00b3a898; /* EESD */
			break;
		}
		data32 = 0x777becdc; /* ESSD */
		break;
	    }
	    die("Error - First dimm slot empty\n");
	}

	print_debug("ODT Value = ");
	print_debug_hex32(data32);
	print_debug("\n");

  	pci_write_config32(ctrl->f0, DDR2ODTC, data32);

	for(dimm=0;dimm<8;dimm+=2) {

		write32(MCBAR+DCALADDR, 0x0b840001);
		write32(MCBAR+DCALCSR, 0x81000003 | (dimm << 20));

		for(i=0;i<1001;i++) {
			data32 = read32(MCBAR+DCALCSR);
			if(!(data32 & (1<<31)))
				break;
		}
	}
}
static void set_receive_enable(const struct mem_controller *ctrl)
{
	u32 i;
	u32 cnt;
	u32 recena=0;
	u32 recenb=0;

	{
	u32 dimm;
	u32 edge;
	int32_t data32;
	u32 dcal_data32_0;
	u32 dcal_data32_1;
	u32 dcal_data32_2;
	u32 dcal_data32_3;
	u32 work32l;
	u32 work32h;
	u32 data32r;
	int32_t recen;
	for(dimm=0;dimm<8;dimm+=1) {

		if(!(dimm&1)) {
			write32(MCBAR+DCALDATA+(17*4), 0x04020000);
			write32(MCBAR+DCALCSR, 0x81800004 | (dimm << 20));

			for(i=0;i<1001;i++) {
				data32 = read32(MCBAR+DCALCSR);
				if(!(data32 & (1<<31)))
					break;
			}
			if(i>=1000)
				continue;

			dcal_data32_0 = read32(MCBAR+DCALDATA + 0);
			dcal_data32_1 = read32(MCBAR+DCALDATA + 4);
			dcal_data32_2 = read32(MCBAR+DCALDATA + 8);
			dcal_data32_3 = read32(MCBAR+DCALDATA + 12);
		}
		else {
			dcal_data32_0 = read32(MCBAR+DCALDATA + 16);
			dcal_data32_1 = read32(MCBAR+DCALDATA + 20);
			dcal_data32_2 = read32(MCBAR+DCALDATA + 24);
			dcal_data32_3 = read32(MCBAR+DCALDATA + 28);
		}

		/* check if bank is installed */
		if((dcal_data32_0 == 0) && (dcal_data32_2 == 0))
			continue;
		/* Calculate the timing value */
		{
		u32 bit;
		for(i=0,edge=0,bit=63,cnt=31,data32r=0,
			work32l=dcal_data32_1,work32h=dcal_data32_3;
				(i<4) && bit; i++) {
			for(;;bit--,cnt--) {
				if(work32l & (1<<cnt))
					break;
				if(!cnt) {
					work32l = dcal_data32_0;
					work32h = dcal_data32_2;
					cnt = 32;
				}
				if(!bit) break;
			}
			for(;;bit--,cnt--) {
				if(!(work32l & (1<<cnt)))
					break;
				if(!cnt) {
					work32l = dcal_data32_0;
					work32h = dcal_data32_2;
					cnt = 32;
				}
				if(!bit) break;
			}
			if(!bit) {
				break;
			}
			data32 = ((bit%8) << 1);
			if(work32h & (1<<cnt))
				data32 += 1;
			if(data32 < 4) {
				if(!edge) {
					edge = 1;
				}
				else {
					if(edge != 1) {
						data32 = 0x0f;
					}
				}
			}
			if(data32 > 12) {
				if(!edge) {
					edge = 2;
				}
				else {
					if(edge != 2) {
						data32 = 0x00;
					}
				}
			}
			data32r += data32;
		}
		}
		work32l = dcal_data32_0;
		work32h = dcal_data32_2;
		recen = data32r;
		recen += 3;
		recen = recen>>2;
		for(cnt=5;cnt<24;) {
			for(;;cnt++)
				if(!(work32l & (1<<cnt)))
					break;
			for(;;cnt++) {
				if(work32l & (1<<cnt))
					break;
			}
			data32 = (((cnt-1)%8)<<1);
			if(work32h & (1<<(cnt-1))) {
				data32++;
			}
			/* test for frame edge cross overs */
			if((edge == 1) && (data32 > 12) &&
			    (((recen+16)-data32) < 3)) {
				data32 = 0;
				cnt += 2;
			}
			if((edge == 2) && (data32 < 4) &&
			    ((recen - data32) > 12))  {
				data32 = 0x0f;
				cnt -= 2;
			}
			if(((recen+3) >= data32) && ((recen-3) <= data32))
				break;
		}
		cnt--;
		cnt /= 8;
		cnt--;
		if(recen&1)
			recen+=2;
		recen >>= 1;
		recen += (cnt*8);
		recen+=2;      /* this is not in the spec, but matches
				 the factory output, and has less failure */
		recen <<= (dimm/2) * 8;
		if(!(dimm&1)) {
			recena |= recen;
		}
		else {
			recenb |= recen;
		}
	}
	}
	/* Check for Eratta problem */
	for(i=cnt=0;i<32;i+=8) {
		if (((recena>>i)&0x0f)>7) {
			cnt+= 0x101;
		}
		else {
			if((recena>>i)&0x0f) {
				cnt++;
			}
		}
	}
	if(cnt&0x0f00) {
		cnt = (cnt&0x0f) - (cnt>>16);
		if(cnt>1) {
			for(i=0;i<32;i+=8) {
				if(((recena>>i)&0x0f)>7) {
					recena &= ~(0x0f<<i);
					recena |= (7<<i);
				}
			}
		}
		else {
			for(i=0;i<32;i+=8) {
				if(((recena>>i)&0x0f)<8) {
					recena &= ~(0x0f<<i);
					recena |= (8<<i);
				}
			}
		}
	}
	for(i=cnt=0;i<32;i+=8) {
		if (((recenb>>i)&0x0f)>7) {
			cnt+= 0x101;
		}
		else {
			if((recenb>>i)&0x0f) {
				cnt++;
			}
		}
	}
	if(cnt & 0x0f00) {
		cnt = (cnt&0x0f) - (cnt>>16);
		if(cnt>1) {
			for(i=0;i<32;i+=8) {
				if(((recenb>>i)&0x0f)>7) {
					recenb &= ~(0x0f<<i);
					recenb |= (7<<i);
				}
			}
		}
		else {
			for(i=0;i<32;i+=8) {
				if(((recenb>>8)&0x0f)<8) {
					recenb &= ~(0x0f<<i);
					recenb |= (8<<i);
				}
			}
		}
	}

	print_debug("Receive enable A = ");
	print_debug_hex32(recena);
	print_debug(",  Receive enable B = ");
	print_debug_hex32(recenb);
	print_debug("\n");

	/* clear out the calibration area */
	write32(MCBAR+DCALDATA+(16*4), 0x00000000);
	write32(MCBAR+DCALDATA+(17*4), 0x00000000);
	write32(MCBAR+DCALDATA+(18*4), 0x00000000);
	write32(MCBAR+DCALDATA+(19*4), 0x00000000);

	/* No command */
	write32(MCBAR+DCALCSR, 0x0000000f);

	write32(MCBAR+0x150, recena);
	write32(MCBAR+0x154, recenb);
}


static void sdram_enable(int controllers, const struct mem_controller *ctrl)
{
	int i;
	int cs;
	int cnt;
	int cas_latency;
	long mask;
	u32 drc;
	u32 data32;
	u32 mode_reg;
	const u32 *iptr;
	u16 data16;
	static const struct {
		u32 clkgr[4];
	} gearing [] = {
		/* FSB 100 */
	{{ 0x00000010, 0x00000000, 0x00000002, 0x00000001}},
		/* FSB 133 */
	{{ 0x00000120, 0x00000000, 0x00000032, 0x00000010}},
		/* FSB 167 */
	{{ 0x00154320, 0x00000000, 0x00065432, 0x00010000}},
		/* FSB 200 DIMM 400 */
	{{ 0x00000001, 0x00000000, 0x00000001, 0x00000000}},
	};

	static const u32 dqs_data[] = {
		0xffffffff, 0xffffffff, 0x000000ff,
		0xffffffff, 0xffffffff, 0x000000ff,
		0xffffffff, 0xffffffff,	0x000000ff,
		0xffffffff, 0xffffffff, 0x000000ff,
		0xffffffff, 0xffffffff, 0x000000ff,
		0xffffffff, 0xffffffff, 0x000000ff,
		0xffffffff, 0xffffffff, 0x000000ff,
		0xffffffff, 0xffffffff, 0x000000ff};

	mask = spd_detect_dimms(ctrl);
	print_debug("Starting SDRAM Enable\n");

	/* 0x80 */
	pci_write_config32(ctrl->f0, DRM,
		0x00410000 | CONFIG_DIMM_MAP_LOGICAL);
	/* set dram type and Front Side Bus freq. */
	drc = spd_set_dram_controller_mode(ctrl, mask);
	if( drc == 0) {
		die("Error calculating DRC\n");
	}
	data32 = drc & ~(3 << 20);  /* clear ECC mode */
	data32 = data32 & ~(7 << 8);  /* clear refresh rates */
	data32 = data32 | (1 << 5);  /* temp turn off ODT */
  	/* Set gearing, then dram controller mode */
  	/* drc bits 3:2 = FSB speed */
  	for(iptr = gearing[(drc>>2)&3].clkgr,cnt=0;cnt<4;cnt++) {
  		pci_write_config32(ctrl->f0, 0xa0+(cnt*4), iptr[cnt]);
	}
	/* 0x7c DRC */
  	pci_write_config32(ctrl->f0, DRC, data32);

		/* turn the clocks on */
	/* 0x8c CKDIS */
  	pci_write_config16(ctrl->f0, CKDIS, 0x0000);

		/* 0x9a DDRCSR Take subsystem out of idle */
  	data16 = pci_read_config16(ctrl->f0, DDRCSR);
	data16 &= ~(7 << 12);
	data16 |= (1 << 12);
  	pci_write_config16(ctrl->f0, DDRCSR, data16);

		/* program row size DRB */
	spd_set_ram_size(ctrl, mask);

		/* program page size DRA */
	spd_set_row_attributes(ctrl, mask);

		/* program DRT timing values */
	cas_latency = spd_set_drt_attributes(ctrl, mask, drc);

	for(i=0;i<8;i+=2) { /* loop through each dimm to test */
		print_debug("DIMM ");
		print_debug_hex8(i);
		print_debug("\n");
		/* Apply NOP */
		do_delay();

		write32(MCBAR+DCALCSR, (0x01000000 | (i<<20)));
		write32(MCBAR+DCALCSR, (0x81000000 | (i<<20)));

		do data32 = read32(MCBAR+DCALCSR);
		while(data32 & 0x80000000);
	}

	/* Apply NOP */
	do_delay();

	for(cs=0;cs<8;cs+=2) {
		write32(MCBAR + DCALCSR, (0x81000000 | (cs<<20)));
		do data32 = read32(MCBAR+DCALCSR);
		while(data32 & 0x80000000);
	}

	/* Precharg all banks */
	do_delay();
	for(cs=0;cs<8;cs+=2) {
		write32(MCBAR+DCALADDR, 0x04000000);
		write32(MCBAR+DCALCSR, (0x81000002 | (cs<<20)));
		do data32 = read32(MCBAR+DCALCSR);
		while(data32 & 0x80000000);
	}

	/* EMRS dll's enabled */
	do_delay();
	for(cs=0;cs<8;cs+=2) {
		/* fixme hard code AL additive latency */
		write32(MCBAR+DCALADDR, 0x0b940001);
		write32(MCBAR+DCALCSR, (0x81000003 | (cs<<20)));
		do data32 = read32(MCBAR+DCALCSR);
		while(data32 & 0x80000000);
	}
	/* MRS reset dll's */
	do_delay();
	if(cas_latency == 30)
		mode_reg = 0x053a0000;
	else
		mode_reg = 0x054a0000;
	for(cs=0;cs<8;cs+=2) {
		write32(MCBAR+DCALADDR, mode_reg);
		write32(MCBAR+DCALCSR, (0x81000003 | (cs<<20)));
		do data32 = read32(MCBAR+DCALCSR);
		while(data32 & 0x80000000);
	}

	/* Precharg all banks */
	do_delay();
	do_delay();
	do_delay();
	for(cs=0;cs<8;cs+=2) {
		write32(MCBAR+DCALADDR, 0x04000000);
		write32(MCBAR+DCALCSR, (0x81000002 | (cs<<20)));
		do data32 = read32(MCBAR+DCALCSR);
		while(data32 & 0x80000000);
	}

	/* Do 2 refreshes */
	do_delay();
	for(cs=0;cs<8;cs+=2) {
		write32(MCBAR+DCALCSR, (0x81000001 | (cs<<20)));
		do data32 = read32(MCBAR+DCALCSR);
		while(data32 & 0x80000000);
	}
	do_delay();
	for(cs=0;cs<8;cs+=2) {
		write32(MCBAR+DCALCSR, (0x81000001 | (cs<<20)));
		do data32 = read32(MCBAR+DCALCSR);
		while(data32 & 0x80000000);
	}
	do_delay();
	/* for good luck do 6 more */
	for(cs=0;cs<8;cs+=2) {
		write32(MCBAR+DCALCSR, (0x81000001 | (cs<<20)));
	}
	do_delay();
	for(cs=0;cs<8;cs+=2) {
		write32(MCBAR+DCALCSR, (0x81000001 | (cs<<20)));
	}
	do_delay();
	for(cs=0;cs<8;cs+=2) {
		write32(MCBAR+DCALCSR, (0x81000001 | (cs<<20)));
	}
	do_delay();
	for(cs=0;cs<8;cs+=2) {
		write32(MCBAR+DCALCSR, (0x81000001 | (cs<<20)));
	}
	do_delay();
	for(cs=0;cs<8;cs+=2) {
		write32(MCBAR+DCALCSR, (0x81000001 | (cs<<20)));
	}
	do_delay();
	for(cs=0;cs<8;cs+=2) {
		write32(MCBAR+DCALCSR, (0x81000001 | (cs<<20)));
	}
	do_delay();
	/* MRS reset dll's normal */
	do_delay();
	for(cs=0;cs<8;cs+=2) {
		write32(MCBAR+DCALADDR, (mode_reg & ~(1<<24)));
		write32(MCBAR+DCALCSR, (0x81000003 | (cs<<20)));
		do data32 = read32(MCBAR+DCALCSR);
		while(data32 & 0x80000000);
	}

	/* Do only if DDR2  EMRS dll's enabled */
	do_delay();
	for(cs=0;cs<8;cs+=2) {
		write32(MCBAR+DCALADDR, (0x0b940001));
		write32(MCBAR+DCALCSR, (0x81000003 | (cs<<20)));
		do data32 = read32(MCBAR+DCALCSR);
		while(data32 & 0x80000000);
	}

	do_delay();
	/* No command */
	write32(MCBAR+DCALCSR, 0x0000000f);

	/* enable on dimm termination */
	set_on_dimm_termination_enable(ctrl);

	/* receive enable calibration */
	set_receive_enable(ctrl);

	/* DQS */
	pci_write_config32(ctrl->f0, 0x94, 0x3904aa00);
	for(i = 0, cnt = (MCBAR+0x200); i < 24; i++, cnt+=4) {
		write32(cnt, dqs_data[i]);
	}
	pci_write_config32(ctrl->f0, 0x94, 0x3900aa00);

	/* Enable refresh */
	/* 0x7c DRC */
	data32 = drc & ~(3 << 20);  /* clear ECC mode */
	pci_write_config32(ctrl->f0, DRC, data32);
	write32(MCBAR+DCALCSR, 0x0008000f);

	/* clear memory and init ECC */
	print_debug("Clearing memory\n");
	for(i=0;i<64;i+=4) {
		write32(MCBAR+DCALDATA+i, 0x00000000);
	}

	for(cs=0;cs<8;cs+=2) {
		write32(MCBAR+DCALCSR, (0x810831d8 | (cs<<20)));
		do data32 = read32(MCBAR+DCALCSR);
		while(data32 & 0x80000000);
	}

	/* Bring memory subsystem on line */
	data32 = pci_read_config32(ctrl->f0, 0x98);
	data32 |= (1 << 31);
	pci_write_config32(ctrl->f0, 0x98, data32);
	/* wait for completion */
	print_debug("Waiting for mem complete\n");
	while(1) {
		data32 = pci_read_config32(ctrl->f0, 0x98);
		if( (data32 & (1<<31)) == 0)
			break;
	}
	print_debug("Done\n");

	/* Set initialization complete */
	/* 0x7c DRC */
	drc |= (1 << 29);
	data32 = drc & ~(3 << 20);  /* clear ECC mode */
	pci_write_config32(ctrl->f0, DRC, data32);

	/* Set the ecc mode */
	pci_write_config32(ctrl->f0, DRC, drc);

	/* Enable memory scrubbing */
	/* 0x52 MCHSCRB */
	data16 = pci_read_config16(ctrl->f0, MCHSCRB);
	data16 &= ~0x0f;
	data16 |= ((2 << 2) | (2 << 0));
	pci_write_config16(ctrl->f0, MCHSCRB, data16);

	/* The memory is now setup, use it */
#if !CONFIG_CACHE_AS_RAM
	cache_ramstage();
#endif
}
