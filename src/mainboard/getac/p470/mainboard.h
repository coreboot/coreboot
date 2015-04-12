struct acpi_rsdp;

unsigned long mainboard_write_acpi_tables(device_t device,
					  unsigned long start,
					  struct acpi_rsdp *rsdp);
