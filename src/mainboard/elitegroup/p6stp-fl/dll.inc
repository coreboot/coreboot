/* Table for DLL Clock Control Register (0x8c - 0x8f), these
   for SiS630 and elitegroup p6stp-fl
   register values are very Mainboard specific */

#	High Byte -> Register	Low Byte -> Value
        
	.word	0x8c88	# set Clock DLL control register
        .word	0x8d88	# 0x8c ~ 0x8f,
        .word	0x8e03	# these values are very M/B
        .word	0x8f55	# specific
