#include <console/console.h>
#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <device/pci_ops.h>
#include <pc80/mc146818rtc.h>
#include "82870.h"

static int ioapic_no = 0;

static void p64h2_ioapic_enable(device_t dev)
{
	uint32_t dword;
	uint16_t word;

	        /* We have to enable MEM and Bus Master for IOAPIC */
		word = 0x0146;
		pci_write_config16(dev, PCICMD, word);
		dword = 0x358015d9;
		pci_write_config32(dev, SUBSYS, dword);


}

static void p64h2_ioapic_init(device_t dev)
{
        uint32_t dword;
        uint16_t word;
        int i, addr;

        volatile uint32_t *ioapic_a;    /* io apic io memory space command address */
        volatile uint32_t *ioapic_d;    /* io apic io memory space data address */

        i = ioapic_no++;

                if(i<3)                 /* io apic address numbers are 3,4,5,&8 */
                        addr=i+3;
                else
                        addr=i+5;
                /* Read the MBAR address for setting up the io apic in io memory space */
                dword = pci_read_config32(dev, PCI_BASE_ADDRESS_0);
                ioapic_a = (uint32_t *) dword;
                ioapic_d = ioapic_a +0x04;
                printk_debug("IOAPIC %d at %02x:%02x.%01x  MBAR = %x DataAddr = %x\n",
                        addr, dev->bus->secondary,
                        PCI_SLOT(dev->path.u.pci.devfn), PCI_FUNC(dev->path.u.pci.devfn),
                        ioapic_a, ioapic_d);

#if 0
                dword = (u32)ioapic_a;
                word = 0x8000 + ((dword >>8)&0x0fff);
                pci_write_config_word(dev, ABAR, word);
#endif
                /* Set up the io apic for the p64h2 - 1461 */
                *ioapic_a=0;
                *ioapic_d=(addr<<24); /* Set the address number */
                *ioapic_a=3;
                *ioapic_d=1;    /* Enable the io apic */

                /* This code test the setup to see if we really found the io apic */
                *ioapic_a=0;
                dword=*ioapic_d;
                printk_debug("PCI %d apic id = %x\n",addr,dword);
                if(dword!=(addr<<24))
                        for(;;);
                *ioapic_a=3;
                dword=*ioapic_d;
                printk_debug("PCI %d apic DT = %x\n",addr,dword);
                if(dword!=1)
                        for(;;);


}

static struct device_operations ioapic_ops = {
        .read_resources   = pci_dev_read_resources,
        .set_resources    = pci_dev_set_resources,
        .enable_resources = pci_dev_enable_resources,
        .init     = p64h2_ioapic_init,
        .scan_bus = 0,
        .enable   = p64h2_ioapic_enable,
};

static struct pci_driver ioapic_driver __pci_driver = {
        .ops    = &ioapic_ops,
        .vendor = PCI_VENDOR_ID_INTEL,
        .device = PCI_DEVICE_ID_INTEL_82870_1E0,

};
