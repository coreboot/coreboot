#include <stdio.h>

int intE6_handler()
{
#if 0
	pciVideoPtr pvp;

	if ((pvp = xf86GetPciInfoForEntity(pInt->entityIndex)))
		X86_AX = (pvp->bus << 8) | (pvp->device << 3) | (pvp->func & 0x7);
	pushw(X86_CS);
	pushw(X86_IP);
	X86_CS = pInt->BIOSseg;
	X86_EIP = 0x0003;
	X86_ES = 0;		/* standard pc es */
#endif
	printf("intE6 not supported right now.\n");
	return 1;
}
