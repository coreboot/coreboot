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
#include <stdint.h>
#include <bsp.h>
#include <ppc.h>
#include <device/pci.h>
#include <mem.h>
#include <string.h>
#include <console/console.h>
#include <arch/io.h>
#include "i2c.h"
#include "mpc107.h"
#include <timer.h>

#define NUM_DIMMS	1
#define NUM_BANKS	2

struct mem_range *
sizeram(void)
{
    int	i;
    sdram_dimm_info dimm[NUM_DIMMS];
    sdram_bank_info bank[NUM_BANKS];
    static struct mem_range	meminfo;

    hostbridge_probe_dimms(NUM_DIMMS, dimm, bank);

    meminfo.basek = 0;
    meminfo.sizek = 0;

    for (i = 0; i < NUM_BANKS; i++) {
	    meminfo.sizek += bank[i].size;
    }

    meminfo.sizek >>= 10;

    return &meminfo;
}

/*
 * Memory is already turned on, but with pessimistic settings. Now
 * we optimize settings to the actual memory configuration.
 */
unsigned 
mpc107_config_memory(void)
{
    sdram_dimm_info sdram_dimms[NUM_DIMMS];
    sdram_bank_info sdram_banks[NUM_BANKS];

    hostbridge_probe_dimms(NUM_DIMMS, sdram_dimms, sdram_banks);
    return hostbridge_config_memory(NUM_BANKS, sdram_banks, 2);
}

/*
 * Configure memory settings.
 */
unsigned long 
hostbridge_config_memory(int no_banks, sdram_bank_info * bank, int for_real)
{
    int i, j;
    char ignore[8];
    /* Convert bus clock to cycle time in 100ns units */
    unsigned cycle_time = 10 * (2500000000U / bsp_clock_speed());
    /* Approximate */
    unsigned access_time = cycle_time - 300;
    unsigned cas_latency = 0;
    unsigned rdlat;
    unsigned refint;
    unsigned refrec;
    unsigned acttorw, acttopre;
    unsigned pretoact, bstopre;
    enum sdram_error_detect error_detect;
    uint32_t mccr1;
    uint32_t mccr2;
    uint32_t mccr3;
    uint32_t mccr4;
    uint8_t bank_enable;
    uint32_t memstart1, memstart2;
    uint32_t extmemstart1, extmemstart2;
    uint32_t memend1, memend2;
    uint32_t extmemend1, extmemend2;
    uint32_t address;
    struct device *dev;

    if ((dev = dev_find_slot(0, 0)) == NULL )
	return 0;

    /* Set up the ignore mask */
    for(i = 0; i < no_banks; i++)
	ignore[i] = (bank[i].size == 0);
	    
    /* Pick best CAS latency possible */
    for (i = 0; i < no_banks; i++)
    {
	if (! ignore[i])
    	{
	    for (j = 0; j < 3; j++) 
	    {
	    	if (cycle_time >= bank[i].cycle_time[j] &&
		    access_time >= bank[i].access_time[j]) 
		{
		    cas_latency = bank[i].cas_latency[j];
		    break;
		}
	    }
	}
    }
    if (!cas_latency)
	return 0;

    /* For various parameters there is a risk of clashing between banks */
    error_detect = (for_real > 1) ? ERRORS_ECC : ERRORS_NONE;
    for (i = 0; i < no_banks; i++)
    {
	if (! ignore[i])
	{
	    {
		for (j = 0; j < 3; j++)
		    if (bank[i].cas_latency[j] == cas_latency)
			break;
		if (j == 3)
	        {
		    ignore[i] = 1;
		    if (! for_real)
		    	printk_info("Disabling memory bank %d (cas latency)\n", i);
		}
		if (bank[i].error_detect < error_detect) 
		    error_detect = bank[i].error_detect;
	    }
	}
    }

    /* Read in configuration of port X */
    mccr1 = pci_read_config32(dev, 0xf0);
    mccr2 = pci_read_config32(dev, 0xf4);
    mccr4 = pci_read_config32(dev, 0xfc);
    mccr1 &= 0xfff00000;
    mccr2 &= 0xffe00000;
    mccr3 = 0;
    mccr4 &= 0x00230000;

    pretoact = 0;
    acttorw = 0;
    acttopre = 0;
    for (i = 0; i < no_banks; i++)
    	if (! ignore[i])
        {
	int rowcode = -1;
	if (for_real)
	{
            bank[i].actual_detect = error_detect;
	    bank[i].actual_cas = cas_latency;
	}
	
	switch (bank[i].row_bits) {
	case 13:
	    if (bank[i].internal_banks == 4)
		rowcode = 2;
	    else if (bank[i].internal_banks == 2)
		rowcode = 1;
	    break;
	case 12:
	    if (bank[i].internal_banks == 4)
		rowcode = 0;
	    else if (bank[i].internal_banks == 2)
		rowcode = 1;
	    break;
	case 11:
	    if (bank[i].internal_banks == 4)
		rowcode = 0;
	    else if (bank[i].internal_banks == 2)
		rowcode = 3;
	    break;
	}
	if (rowcode == -1) {
	    ignore[i] = 1;
	    if (! for_real)
		printk_info("Memory bank %d disabled: row bits %d and banks %d not supported\n", i, bank[i].row_bits, bank[i].internal_banks);
	} else
	    mccr1 |= rowcode << (2 * i);

	/* Update worst case settings */
	if (! ignore[i]) {
	    if (bank[i].min_row_precharge > pretoact)
		pretoact = bank[i].min_row_precharge;
	    if (bank[i].min_ras_to_cas > acttorw)
		acttorw = bank[i].min_ras_to_cas;
	    if (bank[i].min_ras > acttopre)
		acttopre = bank[i].min_ras;
	}
    }

    /* Now convert to clock cycles, rounding up */
    pretoact = (100 * pretoact + cycle_time - 1) / cycle_time;
    acttopre = (100 * acttopre + cycle_time - 1) / cycle_time;
    acttorw = (100 * acttorw + cycle_time - 1) / cycle_time;
    refrec = acttopre;
    bstopre = 0x240;		/* Set conservative values, because we can't derive */
    refint = 1000;

    if (error_detect == ERRORS_ECC)
    {
	rdlat = cas_latency + 2;
    	mccr4 |= 0x00400000;
	mccr2 |= 0x000c0001;
    }
    else
    {
    	rdlat = cas_latency + 1;
	mccr4 |= 0x00100000;
    }
    
    if (pretoact > 16 || acttopre > 16 || acttorw > 16)
	if (! for_real)
	    printk_info("Timings out of range\n");
    mccr4 |= ((pretoact & 0x0f) << 28) | ((acttopre & 0xf) << 24) |
	((acttorw & 0x0f) << 4) |
	((bstopre & 0x003) << 18) | ((bstopre & 0x3c0) >> 6) |
	(cas_latency << 12) | 0x00000200 /* burst length */ ;
    mccr3 |= ((bstopre & 0x03c) << 26) |
	((refrec & 0x0f) << 24) | (rdlat << 20);
    mccr2 |= refint << 2;
    mccr1 |= 0x00080000;	/* memgo */

    address = 0;
    memstart1 = memstart2 = 0;
    extmemstart1 = extmemstart2 = 0;
    memend1 = memend2 = 0;
    extmemend1 = extmemend2 = 0;
    bank_enable = 0;
    for (i = 0; i < no_banks; i++) {
	if (! ignore[i]) {
	    uint32_t end = address + bank[i].size - 1;
	    bank_enable |= 1 << i;
	    if (i < 4) {
		memstart1 |= ((address >> 20) & 0xff) << (8 * i);
		extmemstart1 |= ((address >> 28) & 0x03) << (8 * i);
		memend1 |= ((end >> 20) & 0xff) << (8 * i);
		extmemend1 |= ((end >> 28) & 0x03) << (8 * i);
	    } else {
		int k = i - 4;
		memstart2 |= ((address >> 20) & 0xff) << (8 * k);
		extmemstart2 |= ((address >> 28) & 0x03) << (8 * k);
		memend2 |= ((end >> 20) & 0xff) << (8 * k);
		extmemend2 |= ((end >> 28) & 0x03) << (8 * k);
	    }
	    address += bank[i].size;
	}
    }

    if (for_real)
    {
	pci_write_config8(dev, 0xa0, bank_enable);
	pci_write_config32(dev, 0x80, memstart1);
	pci_write_config32(dev, 0x84, memstart2);
	pci_write_config32(dev, 0x88, extmemstart1);
	pci_write_config32(dev, 0x8c, extmemstart2);
	pci_write_config32(dev, 0x90, memend1);
	pci_write_config32(dev, 0x94, memend2);
	pci_write_config32(dev, 0x98, extmemend1);
	pci_write_config32(dev, 0x9c, extmemend2);

	pci_write_config32(dev, 0xfc, mccr4);
	pci_write_config32(dev, 0xf8, mccr3);
	pci_write_config32(dev, 0xf4, mccr2);
	pci_write_config32(dev, 0xf0, mccr1);
    }
    
    return address;
}

static int
i2c_wait(unsigned timeout, int writing)
{
    uint32_t x;
    while (((x = readl(MPC107_BASE + MPC107_I2CSR)) & (MPC107_I2C_CSR_MCF | MPC107_I2C_CSR_MIF))
	     != (MPC107_I2C_CSR_MCF | MPC107_I2C_CSR_MIF)) {
	if (ticks_since_boot() > timeout)
	    return -1;
    }

    if (x & MPC107_I2C_CSR_MAL) {
	return -1;
    }
    if (writing && (x & MPC107_I2C_CSR_RXAK)) {
	printk_info("No RXAK\n");
	/* generate stop */
    	writel(MPC107_I2C_CCR_MEN, MPC107_BASE + MPC107_I2CCR);
 	return -1;
    }
    writel(0, MPC107_BASE + MPC107_I2CSR);
    return 0;
}

static void
mpc107_i2c_start(struct i2c_bus *bus)
{
    /* Set clock */
    writel(0x1031, MPC107_BASE + MPC107_I2CFDR);
    /* Clear arbitration */
    writel(0, MPC107_BASE + MPC107_I2CSR);
}

static void 
mpc107_i2c_stop(struct i2c_bus *bus)
{
	/* After last DIMM shut down I2C */
	writel(0x0, MPC107_BASE + MPC107_I2CCR);
}

static int 
mpc107_i2c_byte_write(struct i2c_bus *bus, int target, int address, uint8_t data)
{
    	unsigned timeout = ticks_since_boot() + 3 * get_hz();

	/* Must wait here for clocks to start */
        sleep_ticks(get_hz() / 40);
	/* Start with MEN */
	writel(MPC107_I2C_CCR_MEN, MPC107_BASE + MPC107_I2CCR);
	/* Start as master */
	writel(MPC107_I2C_CCR_MEN | MPC107_I2C_CCR_MSTA | MPC107_I2C_CCR_MTX, MPC107_BASE + MPC107_I2CCR);
	/* Write target byte */
	writel(target, MPC107_BASE + MPC107_I2CDR);

	if (i2c_wait(timeout, 1) < 0)
	    return -1;

	/* Write data address byte */
	writel(address, MPC107_BASE + MPC107_I2CDR);

	if (i2c_wait(timeout, 1) < 0)
	    return -1;
	
	/* Write data byte */
    	writel(data, MPC107_BASE + MPC107_I2CDR);

	if (i2c_wait(timeout, 1) < 0)
	    return -1;
	
	/* generate stop */
    	writel(MPC107_I2C_CCR_MEN, MPC107_BASE + MPC107_I2CCR);
    return 0;
}

static int 
mpc107_i2c_master_write(struct i2c_bus *bus, int target, int address, const uint8_t *data, int length)
{
    	unsigned count;
	for(count = 0; count < length; count++)
	{
	    if (mpc107_i2c_byte_write(bus, target, address, data[count]) < 0)
		return -1;
	}
	return count;
}

#define DIMM_LENGTH 0xfff

static int 
mpc107_i2c_master_read(struct i2c_bus *bus, int target, int address,
	    uint8_t *data, int length)
{
    	unsigned timeout = ticks_since_boot() + 3 * get_hz();
    	unsigned count;

	/* Must wait here for clocks to start */
        sleep_ticks(get_hz() / 40);
	/* Start with MEN */
	writel(MPC107_I2C_CCR_MEN, MPC107_BASE + MPC107_I2CCR);
	/* Start as master */
	writel(MPC107_I2C_CCR_MEN | MPC107_I2C_CCR_MSTA | MPC107_I2C_CCR_MTX, MPC107_BASE + MPC107_I2CCR);
	/* Write target byte */
	writel(target, MPC107_BASE + MPC107_I2CDR);

	if (i2c_wait(timeout, 1) < 0)
	    return -1;

	/* Write data address byte */
	writel(address, MPC107_BASE + MPC107_I2CDR);

	if (i2c_wait(timeout, 1) < 0)
	    return -1;

	/* Switch to read - restart */
	writel(MPC107_I2C_CCR_MEN | MPC107_I2C_CCR_MSTA | MPC107_I2C_CCR_MTX | MPC107_I2C_CCR_RSTA, MPC107_BASE + MPC107_I2CCR);
	/* Write target address byte - this time with the read flag set */
	writel(target | 1, MPC107_BASE + MPC107_I2CDR);

	if (i2c_wait(timeout, 0) < 0)
	    return -1;

	if (length == 1)
	    writel(MPC107_I2C_CCR_MEN | MPC107_I2C_CCR_MSTA | MPC107_I2C_CCR_TXAK, MPC107_BASE + MPC107_I2CCR);
	else
	    writel(MPC107_I2C_CCR_MEN | MPC107_I2C_CCR_MSTA, MPC107_BASE + MPC107_I2CCR);
    	/* Dummy read */
	readl(MPC107_BASE + MPC107_I2CDR);

	count = 0;
	while (count < length) {

	    if (i2c_wait(timeout, 0) < 0)
		return -1;

	    /* Generate txack on next to last byte */
	    if (count == length - 2)
		writel(MPC107_I2C_CCR_MEN | MPC107_I2C_CCR_MSTA | MPC107_I2C_CCR_TXAK, MPC107_BASE + MPC107_I2CCR);
	    /* Generate stop on last byte */
	    if (count == length - 1)
		writel(MPC107_I2C_CCR_MEN | MPC107_I2C_CCR_TXAK, MPC107_BASE + MPC107_I2CCR);
	    data[count] = readl(MPC107_BASE + MPC107_I2CDR);
	    if (count == 0 && length == DIMM_LENGTH) {
	    	if (data[0] == 0xff) {
		    printk_debug("I2C device not present\n");
		    length = 3;
		} else {
		    length = data[0];
		    if (length < 3)
			length = 3;
		}
	    }
	    count++;
	}
	
	/* Finish with disable master */
	writel(MPC107_I2C_CCR_MEN, MPC107_BASE + MPC107_I2CCR);
	return length;
}

i2c_fn mpc107_i2c_fn = {
    mpc107_i2c_start, mpc107_i2c_stop,
    mpc107_i2c_master_write, mpc107_i2c_master_read
};

/*
 * Find dimm information.
 */
void
hostbridge_probe_dimms(int no_dimms, sdram_dimm_info *dimms, sdram_bank_info * bank)
{
    unsigned char data[256];
    unsigned dimm;

    printk_debug("i2c testing\n");
    mpc107_i2c_start(NULL);

    for(dimm = 0; dimm < no_dimms; dimm++)
    {
	dimms[dimm].number = dimm;
	dimms[dimm].bank1 = bank + dimm*2;
	dimms[dimm].bank2 = bank + dimm*2 + 1;
	bank[dimm*2].size = 0;
	bank[dimm*2+1].size = 0;
	bank[dimm*2].number = 0;
	bank[dimm*2+1].number = 1;
    }
    
    
    for (dimm = 0; dimm < no_dimms; dimm ++) {
	unsigned limit = mpc107_i2c_master_read(NULL, 0xa0 + 2*dimm, 0,
		data, DIMM_LENGTH);

	if (limit > 3) {
	    sdram_dimm_to_bank_info(data, dimms + dimm, 0);
	    memcpy(dimms[dimm].part_number, data + 73, 18);
	    dimms[dimm].part_number[18] = 0;
	    printk_debug("Part Number: %s\n", dimms[dimm].part_number);
	}
    }

    mpc107_i2c_stop(NULL);
}
