#include <mem.h>
#include <pci.h>
#include <arch/io.h>
#include <printk.h>
#

void e7500_pci_parity_enable(void)
{
	uint8_t reg;

	/* Generate an SERR when a HI_A parity error is detected */
        pcibios_read_config_byte(0, ((0 << 3) + (1 << 0)), 0x5c, &reg);
        reg |= ((1 << 4) + (1 << 0)); 
        pcibios_write_config_byte(0, ((0 << 3) + (1 << 0)), 0x5c, reg);
        
        /* Enable SERR on system bus errors */
        pcibios_read_config_byte(0, ((0 << 3) + (1 << 0)), 0x6c, &reg);
        reg |= ((1 << 3) + (1 << 2) + (1 << 1) + (1 << 0));  
        pcibios_write_config_byte(0, ((0 << 3) + (1 << 0)), 0x6c, reg);

        /* 2SERREN - SERR message forwarding from HI_B to HI_A */
        /* 2PEREN  - Parity error response enable HI_B  */
        pcibios_read_config_byte(0, ((2 << 3) + (0 << 0)), 0x3e, &reg);
        reg |= ((1 << 1) + (1 << 0)); 
        pcibios_write_config_byte(0, ((2 << 3) + (0 << 0)), 0x3e, reg);

	return;
}
