/* Turn off machine check triggers when reading
 * pci space where there are no devices.
 * This is necessary when scaning the bus for
 * devices which is done by the kernel */

#include <console/console.h>
#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <device/pci_ops.h>
#include "./cpu_rev.c"

static cpu_reset_count = 0; //By LYH
static void misc_control_init(struct device *dev)
{
	uint32_t cmd;
	
	printk_debug("NB: Function 3 Misc Control.. ");
	
	/* disable error reporting */
	cmd = pci_read_config32(dev, 0x44);
	cmd |= (1<<6) | (1<<25);
	pci_write_config32(dev, 0x44, cmd );
	if (is_cpu_pre_c0()) {
		/* errata 58 */
		cmd = pci_read_config32(dev, 0x80);
		cmd &= ~(1<<0);
		pci_write_config32(dev, 0x80, cmd );
		cmd = pci_read_config32(dev, 0x84);
		cmd &= ~(1<<24);
		cmd &= ~(1<<8);
		pci_write_config32(dev, 0x84, cmd );
		/* errata 66 */
		cmd = pci_read_config32(dev, 0x70);
		cmd &= ~(1<<0);
		cmd |= (1<<1);
		pci_write_config32(dev, 0x70, cmd );
		cmd = pci_read_config32(dev, 0x7c);
		cmd &= ~(3<<4);
		pci_write_config32(dev, 0x7c, cmd );
	}
	else {
		/* errata 98 */
#if 0		
		cmd = pci_read_config32(dev, 0xd4);
		if(cmd != 0x04e20707) {
			cmd = 0x04e20707;
			pci_write_config32(dev, 0xd4, cmd );
			hard_reset();
		}
#endif

		cmd = 0x04e20707;
		pci_write_config32(dev, 0xd4, cmd );
	}
#if 1
#if HAVE_HARD_RESET==1
	cpu_reset_count++;  //by LYH
	cmd = pci_read_config32(dev, 0xdc);
	if((cmd & 0x0000ff00) != 0x02500) {
		cmd &= 0xffff00ff;
		cmd |= 0x00002500;
		pci_write_config32(dev, 0xdc, cmd );
	        if(cpu_reset_count==CONFIG_MAX_CPUS) { //By LYH
			printk_debug("resetting cpu\n");
			hard_reset();
		} //By LYH
	} 
#endif
#endif	
	printk_debug("done.\n");
}

static struct device_operations mcf3_ops  = {
	.read_resources   = pci_dev_read_resources,
	.set_resources    = pci_dev_set_resources,
	.enable_resources = pci_dev_enable_resources,
	.init             = misc_control_init,
	.scan_bus         = 0,
};

static struct pci_driver mcf3_driver __pci_driver = {
	.ops    = &mcf3_ops,
	.vendor = PCI_VENDOR_ID_AMD,
	.device = 0x1103,
};

