obj-y += mainboard.o

# Needed by irq_tables and mptable and acpi_tables.
obj-$(CONFIG_GENERATE_MP_TABLE) += mptable.o
obj-$(CONFIG_GENERATE_PIRQ_TABLE) += irq_tables.o
obj-$(CONFIG_GENERATE_ACPI_TABLES) += dsdt.o
obj-$(CONFIG_GENERATE_ACPI_TABLES) += acpi_tables.o

# This is part of the conversion to init-obj and away from included code.

initobj-y += crt0.o


