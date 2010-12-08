#include <console/console.h>
#include <watchdog.h>
#include <arch/io.h>
#include <device/device.h>
#include <device/pci.h>

void watchdog_off(void)
{
        device_t dev;
        unsigned long value,base;

	/* turn off the ICH5 watchdog */
        dev = dev_find_slot(0, PCI_DEVFN(0x1f,0));
        /* Enable I/O space */
        value = pci_read_config16(dev, 0x04);
        value |= (1 << 10);
        pci_write_config16(dev, 0x04, value);
        /* Get TCO base */
        base = (pci_read_config32(dev, 0x40) & 0x0fffe) + 0x60;
        /* Disable the watchdog timer */
        value = inw(base + 0x08);
        value |= 1 << 11;
        outw(value, base + 0x08);
        /* Clear TCO timeout status */
        outw(0x0008, base + 0x04);
        outw(0x0002, base + 0x06);
        printk(BIOS_DEBUG, "Watchdog ICH5 disabled\n");
}

