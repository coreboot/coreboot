/* Table for DLL Clock Control Register (0x8c - 0x8f), these
   for SiS630 and pcchips M787cl+
   register values are very Mainboard specific */

#	High Byte -> Register	Low Byte -> Value
/*
        .word	0x8c27	# set Clock DLL control register
        .word	0x8d77	# 0x8c ~ 0x8f,
        .word	0x8e01	# these values are very M/B
        .word	0x8f07	# specific
*/
        .word	0x8c78	# set Clock DLL control register
        .word	0x8d8a	# 0x8c ~ 0x8f,
        .word	0x8e01	# these values are very M/B
        .word	0x8f75	# specific
