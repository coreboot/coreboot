#include <mem.h>
#include <pci.h>
#include <arch/io.h>
#include <printk.h>
#

void ich3_pci_parity_enable(void)
{
        uint8_t reg;

        /* 2SERREN - SERR enable for bridge control */
        /* 2PEREN  - Parity error response enable   */
        pcibios_read_config_byte(0, ((30 << 3) + (0 << 0)), 0x3e, &reg);
        reg |= ((1 << 1) + (1 << 0));
        pcibios_write_config_byte(0, ((30 << 3) + (0 << 0)), 0x3e, reg);

        return;
}


