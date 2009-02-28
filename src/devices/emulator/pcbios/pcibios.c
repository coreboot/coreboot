#include <console/console.h>
#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <device/pci_ops.h>
#include <x86emu/x86emu.h>

#include "pcibios.h"

int pcibios_handler(void)
{
	int  ret = 0;
	struct device *dev = 0;

	switch (X86_AX) {
	case PCI_BIOS_PRESENT:
		X86_AH	= 0x00;		/* no config space/special cycle support */
		X86_AL	= 0x01;		/* config mechanism 1 */
		X86_EDX = 'P' | 'C' << 8 | 'I' << 16 | ' ' << 24;
		X86_EBX = 0x0210;	/* Version 2.10 */
		X86_ECX = 0xFF00;	/* FixME: Max bus number */
		X86_EFLAGS &= ~FB_CF;	/* clear carry flag */
		ret = 1;
		break;
	case FIND_PCI_DEVICE:
		/* FixME: support SI != 0 */
		dev = dev_find_device(X86_DX, X86_CX, dev);
		if (dev != 0) {
			X86_BH = dev->bus->secondary;
			X86_BL = dev->path.pci.devfn;
			X86_AH = SUCCESSFUL;
			X86_EFLAGS &= ~FB_CF;	/* clear carry flag */
			ret = 1;
		} else {
			X86_AH = DEVICE_NOT_FOUND;
			X86_EFLAGS |= FB_CF;	/* set carry flag */
			ret = 0;
		}
		break;
	case FIND_PCI_CLASS_CODE:
		/* FixME: support SI != 0 */
		dev = dev_find_class(X86_ECX, dev);
		if (dev != 0) {
			X86_BH = dev->bus->secondary;
			X86_BL = dev->path.pci.devfn;
			X86_AH = SUCCESSFUL;
			X86_EFLAGS &= ~FB_CF;	/* clear carry flag */
			ret = 1;
		} else {
			X86_AH = DEVICE_NOT_FOUND;
			X86_EFLAGS |= FB_CF;	/* set carry flag */
			ret = 0;
		}
		break;
	case READ_CONFIG_BYTE:
		dev = dev_find_slot(X86_BH, X86_BL);
		if (dev != 0) {
			X86_CL = pci_read_config8(dev, X86_DI);
			X86_AH = SUCCESSFUL;
			X86_EFLAGS &= ~FB_CF;	/* clear carry flag */
			ret = 1;
		} else {
			X86_AH = DEVICE_NOT_FOUND;
			X86_EFLAGS |= FB_CF;	/* set carry flag */	
			ret = 0;
		}
		break;
	case READ_CONFIG_WORD:
		dev = dev_find_slot(X86_BH, X86_BL);
		if (dev != 0) {
			X86_CX = pci_read_config16(dev, X86_DI);
			X86_AH = SUCCESSFUL;
			X86_EFLAGS &= ~FB_CF;	/* clear carry flag */
			ret = 1;
		} else {
			X86_AH = DEVICE_NOT_FOUND;
			X86_EFLAGS |= FB_CF;	/* set carry flag */	
			ret = 0;
		}
		break;
	case READ_CONFIG_DWORD:
		dev = dev_find_slot(X86_BH, X86_BL);
		if (dev != 0) {
			X86_ECX = pci_read_config32(dev, X86_DI);
			X86_AH = SUCCESSFUL;
			X86_EFLAGS &= ~FB_CF;	/* clear carry flag */
			ret = 1;
		} else {
			X86_AH = DEVICE_NOT_FOUND;
			X86_EFLAGS |= FB_CF;	/* set carry flag */	
			ret = 0;
		}
		break;
	case WRITE_CONFIG_BYTE:
		dev = dev_find_slot(X86_BH, X86_BL);
		if (dev != 0) {
			pci_write_config8(dev, X86_DI, X86_CL);
			X86_AH = SUCCESSFUL;
			X86_EFLAGS &= ~FB_CF;	/* clear carry flag */
			ret = 1;
		} else {
			X86_AH = DEVICE_NOT_FOUND;
			X86_EFLAGS |= FB_CF;	/* set carry flag */	
			ret = 0;
		}
		break;
	case WRITE_CONFIG_WORD:
		dev = dev_find_slot(X86_BH, X86_BL);
		if (dev != 0) {
			pci_write_config16(dev, X86_DI, X86_CX);
			X86_AH = SUCCESSFUL;
			X86_EFLAGS &= ~FB_CF;	/* clear carry flag */
			ret = 1;
		} else {
			X86_AH = DEVICE_NOT_FOUND;
			X86_EFLAGS |= FB_CF;	/* set carry flag */	
			ret = 0;
		}
		break;
	case WRITE_CONFIG_DWORD:
		dev = dev_find_slot(X86_BH, X86_BL);
		if (dev != 0) {
			pci_write_config16(dev, X86_DI, X86_ECX);
			X86_AH = SUCCESSFUL;
			X86_EFLAGS &= ~FB_CF;	/* clear carry flag */
			ret = 1;
		} else {
			X86_AH = DEVICE_NOT_FOUND;
			X86_EFLAGS |= FB_CF;	/* set carry flag */	
			ret = 0;
		}
		break;
	default:
		X86_AH = FUNC_NOT_SUPPORTED;
		X86_EFLAGS |= FB_CF; 
		break;
	}

	return ret;
}
