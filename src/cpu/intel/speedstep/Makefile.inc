ifeq ($(CONFIG_HAVE_ACPI_TABLES), y)
ifeq ($(CONFIG_CPU_INTEL_SOCKET_MFCPGA478), y)
	obj-y += acpi.o
endif
endif
