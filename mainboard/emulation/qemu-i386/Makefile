#
# Makefile for this mainboard
#


#obj-y     = mainboard.o setup_before_car.o
obj-y     = setup_before_car.o 
#lib-y	  = linuxbios.vpd

#$(obj)/mainboard.o: $(obj)/dtc.h
#
#$(obj)/dtc.h: $(src)/dts $(obj)/dtc
#	$(obj)/dtc -O lb $(src)/dts >$(obj)/dtc.h


## this does not belong in this file.
#$(obj)/dtc:
#	$(MAKE) -C $(srctree)/util/dtc/
#	cp $(srctree)/util/dtc/dtc $(obj)

#
# This is duplicate in arch/x86/ where it does not belong. 
# How can we cross reference?
#
linuxbios.vpd:
	$(Q)printf "Building dummy VPD ..."
	$(Q)dd if=/dev/zero of=$(objtree)/linuxbios.vpd bs=$(( 0xf0 )) count=1
	$(Q)printf "ok.\n"


