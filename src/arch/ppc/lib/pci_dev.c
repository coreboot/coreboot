#include <arch/io.h>
#include <arch/pciconf.h>

/*
 * Direct access to PCI hardware...
 */

uint8_t pci_ppc_read_config8(unsigned char bus, int devfn, int where)
{
	uint8_t res;

	out_le32((unsigned *)CONFIG_PCIC0_CFGADDR, CONFIG_CMD(bus, devfn, where));
	res = in_8((unsigned char *)CONFIG_PCIC0_CFGDATA + (where & 3));
	return res;
}

uint16_t pci_ppc_read_config16(unsigned char bus, int devfn, int where)
{
	uint16_t res;

	out_le32((unsigned *)CONFIG_PCIC0_CFGADDR, CONFIG_CMD(bus, devfn, where));
	res = in_le16((unsigned short *)CONFIG_PCIC0_CFGDATA + (where & 2));
	return res;
}

uint32_t pci_ppc_read_config32(unsigned char bus, int devfn, int where)
{
	uint32_t res;

	out_le32((unsigned *)CONFIG_PCIC0_CFGADDR, CONFIG_CMD(bus, devfn, where));
	res = in_le32((unsigned *)CONFIG_PCIC0_CFGDATA);
	return res;
}

int pci_ppc_write_config8(unsigned char bus, int devfn, int where, uint8_t data)
{
	out_le32((unsigned *)CONFIG_PCIC0_CFGADDR, CONFIG_CMD(bus, devfn, where));
	out_8((unsigned char *)CONFIG_PCIC0_CFGDATA + (where & 3), data);
	return 0;
}

int pci_ppc_write_config16(unsigned char bus, int devfn, int where, uint16_t data)
{
	out_le32((unsigned *)CONFIG_PCIC0_CFGADDR, CONFIG_CMD(bus, devfn, where));
	out_le16((unsigned short *)CONFIG_PCIC0_CFGDATA + (where & 2), data);
	return 0;
}

int pci_ppc_write_config32(unsigned char bus, int devfn, int where, uint32_t data)
{
	out_le32((unsigned *)CONFIG_PCIC0_CFGADDR, CONFIG_CMD(bus, devfn, where));
	out_le32((unsigned *)CONFIG_PCIC0_CFGDATA, data);
	return 0;
}
