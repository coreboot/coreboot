#include <stdio.h>
#include "test.h"
#include "pci-userspace.h"

#define DEBUG_INT1A

#define SUCCESSFUL              0x00
#define DEVICE_NOT_FOUND        0x86
#define BAD_REGISTER_NUMBER     0x87

void x86emu_dump_xregs(void);
extern int verbose;


int int1A_handler()
{
	PCITAG tag;
	pciVideoPtr pvp = NULL;

	if (verbose) {
		printf("\nint1a encountered.\n");
		//x86emu_dump_xregs();
	}

	switch (X86_AX) {
	case 0xb101:
		X86_EAX = 0x00;	/* no config space/special cycle support */
		X86_AL = 0x01;	/* config mechanism 1 */
		X86_EDX = 0x20494350;	/* " ICP" */
		X86_EBX = 0x0210;	/* Version 2.10 */
		X86_ECX &= 0xFF00;
		X86_ECX |= (pciNumBuses & 0xFF);	/* Max bus number in system */
		X86_EFLAGS &= ~((unsigned long) 0x01);	/* clear carry flag */
#ifdef DEBUG_INT1A
		if (verbose)
			printf("PCI bios present.\n");
#endif
		return 1;
	case 0xb102:
		if (X86_DX == pvp->vendor_id && X86_CX == pvp->device_id && X86_ESI == 0) {
			X86_EAX = X86_AL | (SUCCESSFUL << 8);
			X86_EFLAGS &= ~((unsigned long) 0x01);	/* clear carry flag */
			X86_EBX = pciSlotBX(pvp);
		}
#ifdef SHOW_ALL_DEVICES
		else if ((pvp = xf86FindPciDeviceVendor(X86_EDX, X86_ECX, X86_ESI, pvp))) {
			X86_EAX = X86_AL | (SUCCESSFUL << 8);
			X86_EFLAGS &= ~((unsigned long) 0x01);	/* clear carry flag */
			X86_EBX = pciSlotBX(pvp);
		}
#endif
		else {
			X86_EAX = X86_AL | (DEVICE_NOT_FOUND << 8);
			X86_EFLAGS |= ((unsigned long) 0x01);	/* set carry flag */
		}
#ifdef DEBUG_INT1A
		printf("eax=0x%x ebx=0x%x eflags=0x%x\n", X86_EAX, X86_EBX, X86_EFLAGS);
#endif
		return 1;
	case 0xb103:
#if 0
		if (X86_CL == pvp->interface &&
		    X86_CH == pvp->subclass &&
		    ((X86_ECX & 0xFFFF0000) >> 16) == pvp->class) {
			X86_EAX = X86_AL | (SUCCESSFUL << 8);
			X86_EBX = pciSlotBX(pvp);
			X86_EFLAGS &= ~((unsigned long) 0x01);	/* clear carry flag */
		}
#else
		/* FIXME: dirty hack */
		if (0);
#endif
#ifdef SHOW_ALL_DEVICES
		else if ((pvp = FindPciClass(X86_CL, X86_CH,
					     (X86_ECX & 0xffff0000) >> 16,
					     X86_ESI, pvp))) {
			X86_EAX = X86_AL | (SUCCESSFUL << 8);
			X86_EFLAGS &= ~((unsigned long) 0x01);	/* clear carry flag */
			X86_EBX = pciSlotBX(pvp);
		}
#endif
		else {
			X86_EAX = X86_AL | (DEVICE_NOT_FOUND << 8);
			X86_EFLAGS |= ((unsigned long) 0x01);	/* set carry flag */
		}
#ifdef DEBUG_INT1A
		printf("eax=0x%x eflags=0x%x\n", X86_EAX, X86_EFLAGS);
#endif
		return 1;
	case 0xb108:
		if ((tag = findPci(X86_EBX))) {
			X86_CL = pciReadByte(tag, X86_EDI);
			X86_EAX = X86_AL | (SUCCESSFUL << 8);
			X86_EFLAGS &= ~((unsigned long) 0x01);	/* clear carry flag */
		} else {
			X86_EAX = X86_AL | (BAD_REGISTER_NUMBER << 8);
			X86_EFLAGS |= ((unsigned long) 0x01);	/* set carry flag */
		}
#ifdef DEBUG_INT1A
		printf("eax=0x%x ecx=0x%x eflags=0x%x\n", X86_EAX, X86_ECX, X86_EFLAGS);
#endif
		return 1;
	case 0xb109:
		if ((tag = findPci(X86_EBX))) {
			X86_CX = pciReadWord(tag, X86_EDI);
			X86_EAX = X86_AL | (SUCCESSFUL << 8);
			X86_EFLAGS &= ~((unsigned long) 0x01);	/* clear carry flag */
		} else {
			X86_EAX = X86_AL | (BAD_REGISTER_NUMBER << 8);
			X86_EFLAGS |= ((unsigned long) 0x01);	/* set carry flag */
		}
#ifdef DEBUG_INT1A
		printf("eax=0x%x ecx=0x%x eflags=0x%x\n", X86_EAX, X86_ECX, X86_EFLAGS);
#endif
		return 1;
	case 0xb10a:
		if ((tag = findPci(X86_EBX))) {
			X86_ECX = pciReadLong(tag, X86_EDI);
			X86_EAX = X86_AL | (SUCCESSFUL << 8);
			X86_EFLAGS &= ~((unsigned long) 0x01);	/* clear carry flag */
		} else {
			X86_EAX = X86_AL | (BAD_REGISTER_NUMBER << 8);
			X86_EFLAGS |= ((unsigned long) 0x01);	/* set carry flag */
		}
#ifdef DEBUG_INT1A
		printf("eax=0x%x ecx=0x%x eflags=0x%x\n", X86_EAX, X86_ECX, X86_EFLAGS);
#endif
		return 1;
	case 0xb10b:
		if ((tag = findPci(X86_EBX))) {
			pciWriteByte(tag, X86_EDI, X86_CL);
			X86_EAX = X86_AL | (SUCCESSFUL << 8);
			X86_EFLAGS &= ~((unsigned long) 0x01);	/* clear carry flag */
		} else {
			X86_EAX = X86_AL | (BAD_REGISTER_NUMBER << 8);
			X86_EFLAGS |= ((unsigned long) 0x01);	/* set carry flag */
		}
#ifdef DEBUG_INT1A
		printf("eax=0x%x eflags=0x%x\n", X86_EAX, X86_EFLAGS);
#endif
		return 1;
	case 0xb10c:
		if ((tag = findPci(X86_EBX))) {
			pciWriteWord(tag, X86_EDI, X86_CX);
			X86_EAX = X86_AL | (SUCCESSFUL << 8);
			X86_EFLAGS &= ~((unsigned long) 0x01);	/* clear carry flag */
		} else {
			X86_EAX = X86_AL | (BAD_REGISTER_NUMBER << 8);
			X86_EFLAGS |= ((unsigned long) 0x01);	/* set carry flag */
		}
#ifdef DEBUG_INT1A
		printf("eax=0x%x eflags=0x%x\n", X86_EAX, X86_EFLAGS);
#endif
		return 1;
	case 0xb10d:
		if ((tag = findPci(X86_EBX))) {
			pciWriteLong(tag, X86_EDI, X86_ECX);
			X86_EAX = X86_AL | (SUCCESSFUL << 8);
			X86_EFLAGS &= ~((unsigned long) 0x01);	/* clear carry flag */
		} else {
			X86_EAX = X86_AL | (BAD_REGISTER_NUMBER << 8);
			X86_EFLAGS |= ((unsigned long) 0x01);	/* set carry flag */
		}
#ifdef DEBUG_INT1A
		printf("eax=0x%x eflags=0x%x\n", X86_EAX, X86_EFLAGS);
#endif
		return 1;
	default:
		printf("int1a: subfunction not implemented.\n");
		return 0;
	}
}
