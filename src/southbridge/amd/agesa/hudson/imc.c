#include "imc.h"
#include "amdlib.h"

void imc_fan_control(void)
{
	write8(ACPI_MMIO_BASE + PMIO2_BASE + 0x00, 0x06);
	write8(ACPI_MMIO_BASE + PMIO2_BASE + 0x01, 0x06);
	write8(ACPI_MMIO_BASE + PMIO2_BASE + 0x02, 0xf7);
	write8(ACPI_MMIO_BASE + PMIO2_BASE + 0x03, 0xff);
	write8(ACPI_MMIO_BASE + PMIO2_BASE + 0x04, 0xff);

#if !CONFIG_SOUTHBRIDGE_AMD_AGESA_YANGTZE
	write8(ACPI_MMIO_BASE + PMIO2_BASE + 0x10, 0x06);
	write8(ACPI_MMIO_BASE + PMIO2_BASE + 0x11, 0x06);
	write8(ACPI_MMIO_BASE + PMIO2_BASE + 0x12, 0xf7);
	write8(ACPI_MMIO_BASE + PMIO2_BASE + 0x13, 0xff);
	write8(ACPI_MMIO_BASE + PMIO2_BASE + 0x14, 0xff);
#endif

#if CONFIG_SOUTHBRIDGE_AMD_AGESA_YANGTZE
	UINT8                        PciData;
	PCI_ADDR                      PciAddress;
	AMD_CONFIG_PARAMS             StdHeader;
	PciAddress.AddressValue = MAKE_SBDFO (0, 0, 0x18, 0x3, 0x1E4);
	LibAmdPciRead(AccessWidth8, PciAddress, &PciData, &StdHeader);
	PciData &= (UINT8)0x8F;
	PciData |= 0x10;
	LibAmdPciWrite(AccessWidth8, PciAddress, &PciData, &StdHeader);
#endif

	imc_func_0x83();
	imc_func_0x85();
	imc_func_0x81();
}

void write_imc_msg(u8 idx, u8 val)
{
	outb(idx, 0x3e);
	outb(val, 0x3f);
}
