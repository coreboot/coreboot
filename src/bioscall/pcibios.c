
#ifndef lint
static char rcsid[] = "$Id$";
#endif

#include <pci.h>
#undef __KERNEL__
#include <arch/io.h>
#include <printk.h>

enum {
	CHECK = 0xb001,
	FINDDEV = 0xb102,
	READCONFBYTE = 0xb108,
	READCONFWORD = 0xb109,
	READCONFDWORD = 0xb10a,
	WRITECONFBYTE = 0xb10b,
	WRITECONFWORD = 0xb10c,
	WRITECONFDWORD = 0xb10d
};

// errors go in AH. Just set these up so that word assigns
// will work. KISS. 
enum {
	PCIBIOS_NODEV = 0x8600,
	PCIBIOS_BADREG = 0x8700
};

int
pcibios(
	unsigned long *pedi, 
	unsigned long *pesi,
	unsigned long *pebp, 
	unsigned long *pesp, 
	unsigned long *pebx, 
	unsigned long *pedx, 
	unsigned long *pecx, 
	unsigned long *peax, 
	unsigned long *pflags
	) {
	unsigned long edi = *pedi;
	unsigned long esi = *pesi;
	unsigned long ebp = *pebp;
	unsigned long esp = *pesp;
	unsigned long ebx = *pebx;
	unsigned long edx = *pedx;
	unsigned long ecx = *pecx;
	unsigned long eax = *peax;
	unsigned long flags = *pflags;
	unsigned short func = (unsigned short) eax;
	int retval = -1;
	unsigned short devid, vendorid, devfn;
	short devindex; /* Use short to get rid of gabage in upper half of 32-bit register */
	unsigned char bus;
	struct pci_dev *dev;

	switch(func) {
	case  CHECK:
		*pedx = 0x4350;
		*pecx = 0x2049;
		retval = 0;
		break;
	case FINDDEV:
	{
		devid = *pecx;
		vendorid = *pedx;
		devindex = *pesi;
		dev = 0;
		while ((dev = pci_find_device(vendorid, devid, dev))) {
			if (devindex <= 0)
				break;
			devindex--;
		}
		if (dev) {
			unsigned short busdevfn;
			*peax = 0;
				// busnum is an unsigned char;
				// devfn is an int, so we mask it off. 
			busdevfn = (dev->bus->secondary << 8)
					| (dev->devfn & 0xff);
			printk_debug("0x%x: return 0x%x\n", func, busdevfn);
			*pebx = busdevfn;
			retval = 0;
		} else {
			*peax = PCIBIOS_NODEV;
			retval = -1;
		}
	}
	break;
	case READCONFDWORD:
	case READCONFWORD:
	case READCONFBYTE:
	case WRITECONFDWORD:
	case WRITECONFWORD:
	case WRITECONFBYTE:
	{
		unsigned long dword;
		unsigned short word;
		unsigned char byte;
		unsigned char reg;
		
		devfn = *pebx & 0xff;
		bus = *pebx >> 8;
		reg = *pedi;
		dev = pci_find_slot(bus, devfn);
		if (! dev) {
			printk_debug("0x%x: BAD DEVICE bus %d devfn 0x%x\n", func, bus, devfn);
			// idiots. the pcibios guys assumed you'd never pass a bad bus/devfn!
			*peax = PCIBIOS_BADREG;
			retval = -1;
		}
	switch(func) {
		case READCONFBYTE:
			retval = pci_read_config_byte(dev, reg, &byte);
			*pecx = byte;
			break;
		case READCONFWORD:
			retval = pci_read_config_word(dev, reg, &word);
			*pecx = word;
			break;
		case READCONFDWORD:
			retval = pci_read_config_dword(dev, reg, &dword);
			*pecx = dword;
			break;
		case WRITECONFBYTE:
			byte = *pecx;
                        retval = pci_write_config_byte(dev, reg, byte);
			break;
		case WRITECONFWORD:
			word = *pecx;
                        retval = pci_write_config_word(dev, reg, word);
			break;
		case WRITECONFDWORD:
			word = *pecx;
                        retval = pci_write_config_dword(dev, reg, dword);
			break;
                }
			
		if (retval) 
			retval = PCIBIOS_BADREG;
		printk_debug("0x%x: bus %d devfn 0x%x reg 0x%x val 0x%lx\n", func, bus, devfn, reg, *pecx);
		*peax = 0;
		retval = 0;
	}
	break;
	default:
		printk_err("UNSUPPORTED PCIBIOS FUNCTION 0x%x\n",  func);
		break;
	}

	return retval;
} 
