driver-y += northbridge.o
driver-y += misc_control.o

obj-$(CONFIG_HAVE_ACPI_TABLES) += amdfam10_acpi.o
obj-$(CONFIG_HAVE_ACPI_TABLES) += ssdt.o
obj-$(CONFIG_HAVE_ACPI_TABLES) += sspr1.o
obj-$(CONFIG_HAVE_ACPI_TABLES) += sspr2.o
obj-$(CONFIG_HAVE_ACPI_TABLES) += sspr3.o
obj-$(CONFIG_HAVE_ACPI_TABLES) += sspr4.o
obj-$(CONFIG_HAVE_ACPI_TABLES) += sspr5.o

obj-y += get_pci1234.o
