/* Table for DLL Clock Control Register (0x8c - 0x8f), these
   register values are very Mainboard specific */

#	High Byte -> Register	Low Byte -> Value
#ifndef SIS630S
	.word	0x8c66
	.word	0x8d66
	.word	0x8e03
	.word	0x8f55
#else /* SIS630S */
        .word	0x8c27	# set Clock DLL control register
        .word	0x8d77	# 0x8c ~ 0x8f,
        .word	0x8e01	# these values are very M/B
        .word	0x8f07	# specific
#endif /* SIS630S */