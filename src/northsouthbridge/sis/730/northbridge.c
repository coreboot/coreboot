
/*
 * Bootstrap code for the INTEL 
 * $Id$
 *
 */

#ifndef lint
static char rcsid[] =
"$Id$";
#endif


#include <printk.h>
#include <pciconf.h>
#include <subr.h>
#include <pci.h>
#include <pci_ids.h>

/* these functions query the hardware to figure out how much ram is in
 * the machine. They then place that information in the parameter block. 
 * This is kind of schizophrenic, but we do it this way because the 
 * functions that actually set the registers are kind of hairy, and 
 * we're not sure they can carry the full burden of also passing on 
 * the size information to the rest of the bootstrap. Besides, querying
 * hardware for ram sizes is trivial. 
 */

/* table for calculate the DRAM size, the unit is Mega Bytes */
const static int ramsizes[16] =
{
	8, 32, 32, 64, 16, 64, 64, 128,
	32, 128, 128, 256, 16, 256, 256, 512
};
#define SIS630_BANKENABLE 0x63
#define SIS630_BANK0 0x60
#define SIS630_BANK1 0x61
#define SIS630_BANK2 0x62
#define SIS630_DIMM_LOCATION_FOR_SMA 0x65
#define MAX_DIMM_SLOTS 3
// We thought about a function but this is easier. 
// feel free to improve on this idea. 
// this array has all the DIMM slot size in Kbytes. 
unsigned long slotsizeM[MAX_DIMM_SLOTS];

unsigned long sizeram()
{
	struct pci_dev *pcidev;
	unsigned int dimm_slot, dimm_reg, sides;
	unsigned long total_size;
	u8 dram_status, dimm_status;
	u8 sma_enable, sma_size, sma_size_bits, sma_location;

	if ((pcidev = pci_find_device(PCI_VENDOR_ID_SI, PCI_DEVICE_ID_SI_730, NULL)) == NULL)
		return 0;

	pci_read_config_byte(pcidev, SIS630_BANKENABLE, &dram_status);
	dimm_status = dram_status & 0x7;
	sma_enable = dram_status & 0x80;

	pci_read_config_byte(pcidev, SIS630_DIMM_LOCATION_FOR_SMA, &sma_location);
	sma_location &= 0x03;

	/* compute Shared Menory Area (SMA) size in Mega Bytes */
	sma_size_bits = (dram_status >> 4) & 0x7;
	if (sma_size_bits > 5)
		// this is invalid!
		sma_size = 0;
	else {
		sma_size = (2 << sma_size_bits);
	}

	for (dimm_slot = 0, total_size = 0, dimm_reg = SIS630_BANK0;
	     dimm_reg <= SIS630_BANK2; dimm_slot++, dimm_reg++) {
		u8 regval;
		slotsizeM[dimm_slot] = 0;
		if ((dimm_status & (1 << dimm_slot)) == 0)
			/* this DIMM slot does not have SDRAM installed */
			continue;

		pci_read_config_byte(pcidev, dimm_reg, &regval);

		/* is this DIMM single or double sided  ?? */
		if (regval & 0x20)
			sides = 2;
		else
			sides = 1;

		/* low-order 4 bits are a ram size */
		slotsizeM[dimm_slot] = (ramsizes[regval & 0xf] * sides);
		total_size += slotsizeM[dimm_slot];
	}

	/* the total memory visible to the system is physical memory installed minus the
	   amount used by VGA (sma_size) */
	total_size -= sma_size;

	/* return the memory size in KB */
	total_size *= 1024;
	return total_size;
}

#ifdef HAVE_FRAMEBUFFER
void framebuffer_on()
{
	struct pci_dev *pcidev;
	u16 command;

	if ((pcidev = pci_find_device(PCI_VENDOR_ID_SI, PCI_DEVICE_ID_SI_5591_AGP, NULL)) == NULL)
		return 0;

	pci_read_config_word(pcidev, 0x04, &command);
	command |= 0x20;
	pci_write_config_word(pcidev, 0x04, command);

	/* enable legacy VGA IO (0x3B0 - 0x3BB, 0x3C0 - 0x3DF) and MEM (0xA0000 - 0xBFFFF),
	   needed for XFree86 3.3.6 */
	pci_read_config_word(pcidev, 0x3e, &command);
	command |= 0x08;
	pci_write_config_word(pcidev, 0x3e, command);
}
#endif	/* HAVE_FRAMEBUFFER */

