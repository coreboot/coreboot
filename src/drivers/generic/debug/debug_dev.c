#include <console/console.h>
#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <device/pci_ops.h>
#include <cpu/x86/msr.h>
#include "chip.h"

static void print_pci_regs(struct device *dev)
{
      uint8_t byte;
      int i;

      for(i=0;i<256;i++) {
	     byte = pci_read_config8(dev, i);
   
             if((i & 0xf)==0) printk_debug("\n%02x:",i);
             printk_debug(" %02x",byte);
      }
      printk_debug("\n");

}
static void print_mem(void)
{
        unsigned int i;
	unsigned int start = 0xfffff000;
	for(i=start;i<0xffffffff;i++) {
             if((i & 0xf)==0) printk_debug("\n %08x:",i);
             printk_debug(" %02x",(unsigned char)*((unsigned char *)i));
             }
	printk_debug(" %02x\n",(unsigned char)*((unsigned char *)i));

 }
static void print_pci_regs_all(void)
{
        struct device *dev;
	unsigned char i,j,k;

	for(i=0;i<=15;i++) {
		for(j=0;j<=0x1f;j++) {
			for (k=0;k<=6;k++){
				dev = dev_find_slot(i, PCI_DEVFN(j, k));
				if(!dev) {
					continue;
				}
				if(!dev->enabled) {
					continue;
				}
			        printk_debug("\n%02x:%02x:%02x aka %s",i,j,k, dev_path(dev));
				print_pci_regs(dev);
			}
		}
	}

}

static void print_msr()
{
        msr_t msr;
        unsigned index;
        unsigned eax, ebx, ecx, edx;
        index = 0x80000007;
        printk_debug("calling cpuid 0x%08x\n", index);
        asm volatile(
                "cpuid"
                : "=a" (eax), "=b" (ebx), "=c" (ecx), "=d" (edx)
                : "a" (index)
                );
        printk_debug("cpuid[%08x]: %08x %08x %08x %08x\n",
                index, eax, ebx, ecx, edx);
        if (edx & (3 << 1)) {
                index = 0xC0010042;
                printk_debug("Reading msr: 0x%08x\n", index);
                msr = rdmsr(index);
                printk_debug("msr[0x%08x]: 0x%08x%08x\n",
                        index, msr.hi, msr.hi);
        }

}
 
static void debug_init(device_t dev)
{
	device_t parent;
	switch(dev->path.u.pnp.device) {
#if CONFIG_CHIP_NAME
	case 0:
		parent = dev->bus->dev;
		printk_debug("DEBUG: %s", dev_path(parent));
		if(parent->chip_ops && parent->chip_ops->name) {
			printk_debug(": %s\n", parent->chip_ops->name);
		} else {
			printk_debug("\n");
		}
		break;
#endif
		
	case 1:
		print_pci_regs_all();
		break;
	case 2: 
		print_mem();
		break;
	case 3:
		print_msr();
		break;
	}
}

static void debug_noop(device_t dummy)
{
}

static struct device_operations debug_operations = {
        .read_resources   = debug_noop,
        .set_resources    = debug_noop,
        .enable_resources = debug_noop,
        .init             = debug_init,
};

static void enable_dev(struct device *dev)
{
	dev->ops = &debug_operations;
}

struct chip_operations drivers_generic_debug_ops = {
	CHIP_NAME("Debug device")
	.enable_dev = enable_dev, 
};
