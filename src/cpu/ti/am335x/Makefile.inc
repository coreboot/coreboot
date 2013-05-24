bootblock-$(CONFIG_EARLY_CONSOLE) += uart.c
bootblock-y	+= nand.c

romstage-y	+= nand.c
romstage-y	+= uart.c
