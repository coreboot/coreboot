#
# Makefile for this mainboard
#
# not configurable
LBARCH=x86

obj-y     = mainboard.o setup_before_car.o
core-$(UNCOMPRESSORS) += uncompressors

$(obj)/mainboard.o: $(obj)/dtc.h

$(obj)/dtc.h: $(src)/dts
	dtc -O lb $(src)/dts >$(obj)/dtc.h

