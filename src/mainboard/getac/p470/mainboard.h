struct acpi_rsdp;

unsigned long mainboard_write_acpi_tables(unsigned long start,
					  struct acpi_rsdp *rsdp);
