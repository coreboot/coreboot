#include <mem.h>
#include <part/sizeram.h>
#include <pci.h>
#include <cpu/p5/io.h>
#include <printk.h>

struct mem_range *sizeram(void)
{
	static struct mem_range mem[3];
	//unsigned long totalmemKB = 1024*1024;
	struct pci_dev *pcidev;
	unsigned long totalmemKB = 0 ,memfound = 0;
	unsigned int datawidth;
	unsigned int value,addressingtype;
	unsigned int bit4_5, bit2_3,i;
	
	if((pcidev = pci_find_device(0x1344,0x3321,NULL)) == NULL)
		return 0;
	
	
	// Read the Rank Type Registers 0-7
	// Fn 1 Offset 0x80-0x87
	pci_read_config_byte(pcidev,0x80,&value);
	
	for(i=0;(i<=7) && (value != 0);){	
		// Check addressing type
		bit4_5 = value & 0x30;
		switch(bit4_5){
		case 0 :
			addressingtype = 64*1024;	//Kb
			break;
		case 0x10 :
			addressingtype = 128*1024;	
			break;
		case 0x20 :
			addressingtype = 256*1024;
			break;
		case 0x30 :
			addressingtype = 512*1024;
			break;
		default:
			addressingtype = 0;
		}
			
		// Check data bit width
		bit2_3 = value & 0xc;
		switch(bit2_3){
		case 0 :
			datawidth = 4;
			break;
		case 0x4 :
			datawidth = 8;
			break;
		case 0x8 :
			datawidth = 16;
			break;
		default:
			datawidth = 0;
		}	

		memfound = (addressingtype << datawidth); // in Kb
		memfound = memfound >> 3; 	// in KB
		totalmemKB += memfound;
		i++;
		if(i<=7){
			value = 0;
			pci_read_config_byte(pcidev,(0x80+i),&value);
		}
	}
	
	mem[0].basek = 0;
	mem[0].sizek = 640;
	mem[1].basek = 1024;
	mem[1].sizek = totalmemKB - mem[1].basek;
	mem[2].basek = 0;
	mem[2].sizek = 0;
	return &mem;
}


