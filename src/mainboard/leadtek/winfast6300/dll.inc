/* Table for DLL Clock Control Register (0x8c - 0x8f), these
   register values are very Mainboard specific */

#		Reg.	Value
#ifndef SIS630S
	.byte	0x8c,	0x66
	.byte	0x8d,	0x66
	.byte	0x8e,	0x03
	.byte	0x8f,	0x55
#else /* SIS630S */
        .byte	0x8c,	0x27	# set Clock DLL control register
        .byte	0x8d,	0x77	# 0x8c ~ 0x8f,
        .byte	0x8e,	0x01	# these values are very M/B
        .byte	0x8f,	0x07	# specific
#endif /* SIS630S */