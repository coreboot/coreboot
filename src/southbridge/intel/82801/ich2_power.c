#include <southbridge/intel/82801.h>
#include <pci.h>
#include <pci_ids.h>
#include <printk.h>
#include "82801.h"

void ich2_power_after_power_fail(int on)
{
	struct pci_dev *dev;
	unsigned char byte;
	dev = pci_find_device(PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_82801BA_1F0, 0);	
	if (!dev) {
		return;
	}
	/* FIXME this doesn't work! */
	/* Which state do we want to goto after g3 (power restored)?
	 * 0 == S0 Full On
	 * 1 == S5 Soft Off
	 */
	pci_write_config_byte(dev, GEN_PMCON_3, on?0:1);
	printk_info("set power %s after power fail\n", on?"on":"off");
}
