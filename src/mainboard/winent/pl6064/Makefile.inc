driver-y += mainboard.o

# Needed by irq_tables and mptable and acpi_tables.
obj-$(CONFIG_GENERATE_PIRQ_TABLE) += irq_tables.o

# This is part of the conversion to init-obj and away from included code.

initobj-y += crt0.o

ldscripts := $(src)/arch/i386/init/ldscript_fallback_cbfs.lb
ldscripts += $(src)/cpu/x86/16bit/entry16.lds
ldscripts += $(src)/cpu/x86/16bit/reset16.lds
ldscripts += $(src)/arch/i386/lib/id.lds
ldscripts += $(src)/arch/i386/lib/failover.lds

