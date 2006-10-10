#
# Makefile for this mainboard
#
# not configurable
LBARCH=x86

obj-y     = mainboard.o irq_tables.o setup_before_car.o
core-$(COMPRESSORS) += compressors

mainboard.o: dtc.h

dtc.h: dts
	dtc -O lb dts > $@

