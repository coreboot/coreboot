#include <arch/io.h>
#include <reset.h>
#include <device/pci_def.h>
#include <device/pci_ids.h>
#if !defined (__ROMCC__) && !defined (__PRE_RAM__)
#include <device/pci.h>
#define PCI_ID(VENDOR_ID, DEVICE_ID) \
	((((DEVICE_ID) & 0xFFFF) << 16) | ((VENDOR_ID) & 0xFFFF))
#define PCI_DEV_INVALID 0

static inline device_t pci_locate_device(unsigned pci_id, device_t from)
{
	return dev_find_device(pci_id >> 16, pci_id & 0xffff, from);
}
#else
#include <arch/romcc_io.h>
#endif

void soft_reset(void)
{
        outb(0x04, 0xcf9);
}
void hard_reset(void)
{
        outb(0x02, 0xcf9);
        outb(0x06, 0xcf9);
}
