include $(src)/mainboard/Makefile.k8_CAR.inc

# Needed by irq_tables and mptable and acpi_tables.
obj-y += get_bus_conf.o

driver-y += ../../../drivers/i2c/i2cmux2/i2cmux2.o

