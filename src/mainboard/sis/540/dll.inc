 /* Table for DLL Clock Control Register (0x8c - 0x8f), these
   register values are very Mainboard specific */

#	High Byte -> Register	Low Byte -> Value
        .word	0x8c44	# set Clock DLL control register
        .word	0x8d86	# 0x8c ~ 0x8f,
        .word	0x8ec1	# these values are very M/B
        .word	0x8f46	# specific
