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
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston,
 * MA 02110-1301 USA
 */

#include <console/console.h>
#include "mpc107.h"

void 
sdram_dimm_to_bank_info(const char *data,  sdram_dimm_info *dimm)
{
    sdram_bank_info *bank1 = dimm->bank1;
    sdram_bank_info *bank2 = dimm->bank2;
    unsigned char csum = 0;
    unsigned char x;
    int i;
    int no_cas_latencies = 0;
    char latency[3];
    
    /* Mark banks initially broken */
    bank1->size = 0;
    bank2->size = 0;

    if (data[0] < 64)
    {
	printk_info("SPD data too short\n");
	return;
    }

    for(i = 0; i < 63; i++)
	csum += data[i];

    if (csum != data[63])
    {
	printk_info("Broken checksum 0x%x, should be 0x%x\n", data[63], csum);
	return;
    }
    
    if (data[2] != 0x04)
    {
	printk_info("SDRAM Only\n");
	return;
    }
    
    bank1->row_bits = data[3] & 0x0f;
    if (data[3] >> 4)
	bank2->row_bits = data[3] >> 4;
    else
	bank2->row_bits = bank1->row_bits;	
    
    bank1->internal_banks = bank2->internal_banks = data[17];
    
    bank1->col_bits = data[4] & 0x0f;
    if (data[4] >> 4)
	bank2->col_bits = data[4] >> 4;
    else
	bank2->col_bits = bank1->col_bits;
    
    if (data[7] || (data[6] != 80 && data[6] != 72 && data[6] != 64))
    {
	printk_info("Data width incorrect\n");
	return;
    }
    
    if (data[8] != 0x01)
    {
	printk_info("3.3V TTL DIMMS only\n");
	return;
    }
    
    /* Extract CAS latencies in reverse order, as we only get info on
       the highest ones. */
    x = data[18];
    for(i = 7; i > 0; i--)
    {
	if (x & 0x40)
	{
	    if (no_cas_latencies < 3)
	    	latency[no_cas_latencies] = i;
	    no_cas_latencies++;
	}
	x <<= 1;
    }
    
    /* Now fill in other timings - we're most interested in the lowest
       CAS latency, so we shuffle data to put that first. */
    for(i = no_cas_latencies; i >= 0; i--)
	bank1->cas_latency[no_cas_latencies - i - 1] = 
	    bank2->cas_latency[no_cas_latencies - i - 1] =
		 latency[i];
    for(i = no_cas_latencies; i < 3; i++)
	bank1->cas_latency[i] = bank2->cas_latency[i] = 0;
    
    /* Store values for the highest cas latency */
    bank1->cycle_time[no_cas_latencies - 1] = 
	bank2->cycle_time[no_cas_latencies- 1] = 
	    100 * (data[9] >> 4) + 10 * (data[9] & 0xf);
    bank1->access_time[no_cas_latencies - 1] = 
	bank2->access_time[no_cas_latencies - 1] =
	    100 * (data[10] >> 4) + 10 * (data[10] & 0xf);
    /* Then the second highest */
    if (no_cas_latencies > 1)
    {
	bank1->cycle_time[no_cas_latencies - 2] = 
	    bank2->cycle_time[no_cas_latencies- 2] =
		100 * (data[23] >> 4) + 10 * (data[23] & 0xf);
	bank1->access_time[no_cas_latencies - 2] = 
	    bank2->access_time[no_cas_latencies - 2] =
		100 * (data[24] >> 4) + 10 * (data[24] & 0xf);
    }
    /* Then the third highest */
    if (no_cas_latencies > 2)
    {
	bank1->cycle_time[no_cas_latencies - 3] = 
	    bank2->cycle_time[no_cas_latencies- 3] = 
		100 * (data[25] >> 2) + 25 * (data[25] & 0x3);
	bank1->access_time[no_cas_latencies - 3] = 
	    bank2->access_time[no_cas_latencies - 3] =
		100 * (data[26] >> 2) + 25 * (data[26] & 0x3);
    }

    for(i = 0; i < no_cas_latencies; i++)
	printk_debug("CL %d: cycle %dns access %dns\n",
	    bank1->cas_latency[i], bank1->cycle_time[i] / 100,
	    bank1->access_time[i] / 100);

    /* Other timings */
    bank1->min_back_to_back = bank2->min_back_to_back = data[15];
    bank1->min_row_precharge = bank2->min_row_precharge = data[27];
    bank1->min_active_to_active = bank2->min_active_to_active = data[28];
    bank1->min_ras_to_cas = bank2->min_ras_to_cas = data[29];
    bank1->min_ras = bank2->min_ras = data[30];
    
    /* Error detection type */
    bank1->error_detect = bank2->error_detect = data[11];
    
    /* Crucial row sizes - these mark the data as valid */
    for(i = 7; i >= 0; i--)
    {
	if (data[31] & (1 << i))
	{
	    bank1->size = (4*1024*1024) << i;
	    break;
	}
    }
    if (data[5] > 1)
    {
	for(i-- ; i >= 0; i--)
	{
	    if (data[31] & (1 << i))
	    {
	    	bank2->size = (4*1024*1024) << i;
	    	break;
	    }
	}
	if (! bank2->size)
	    bank2->size = bank1->size;
    }
    dimm->size = bank1->size + bank2->size;
}

void 
print_sdram_bank_info(const sdram_bank_info *bank)
{
    if (bank->size)
	printk_debug("  Bank %d: %dMB\n", bank->number, bank->size / (1024*1024));
}

static const char *error_types[] = {"", "Parity ", "ECC "};

void 
print_sdram_dimm_info(const sdram_dimm_info *dimm)
{
    printk_debug("Dimm %d: ", dimm->number);
    if (dimm->size) {
    	printk_debug("%dMB CL%d (%s): Running at CL%d %s\n", 
	     dimm->size / (1024*1024), dimm->bank1->cas_latency[0],
	     dimm->part_number,
	     dimm->bank1->actual_cas,
	     error_types[dimm->bank1->actual_detect]);
	print_sdram_bank_info(dimm->bank1);
	print_sdram_bank_info(dimm->bank2);
    } else
	printk_debug("(none)\n");
}
