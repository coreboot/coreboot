/* $Id$ */
/* Copyright 2000  AG Electronics Ltd. */
/* This code is distributed without warranty under the GPL v2 (see COPYING) */

#include <pci.h>
#include <bsp.h>

unsigned __pci_config_read_32(unsigned address);
unsigned __pci_config_read_16(unsigned address);
unsigned __pci_config_read_8(unsigned address);
void __pci_config_write_32(unsigned address, unsigned data);
void __pci_config_write_16(unsigned address, unsigned short data);
void __pci_config_write_8(unsigned address, unsigned char data);

#define CONFIG_CMD(bus,devfn,where) (bus << 16 | devfn << 8 | where | 0x80000000)

static int pci_ppc_read_config_dword(unsigned char bus, int devfn, int where, u32 *data)
{
    *data = __pci_config_read_32(CONFIG_CMD(bus, devfn, where));
    return 0;
}

static int pci_ppc_read_config_word(unsigned char bus, int devfn, int where, u16 *data)
{
    *data = __pci_config_read_16(CONFIG_CMD(bus, devfn, where));
    return 0;
}

static int pci_ppc_read_config_byte(unsigned char bus, int devfn, int where, u8 *data)
{
    *data = __pci_config_read_8(CONFIG_CMD(bus, devfn, where));
    return 0;
}

static int pci_ppc_write_config_dword(unsigned char bus, int devfn, int where, u32 data)
{
    __pci_config_write_32(CONFIG_CMD(bus, devfn, where), data);
    return 0;
}

static int pci_ppc_write_config_word(unsigned char bus, int devfn, int where, u16 data)
{
    __pci_config_write_16(CONFIG_CMD(bus, devfn, where), data);
    return 0;
}

static int pci_ppc_write_config_byte(unsigned char bus, int devfn, int where, u8 data)
{
    __pci_config_write_8(CONFIG_CMD(bus, devfn, where), data);
    return 0;
}

struct pci_ops pci_direct_ppc =
{
    pci_ppc_read_config_byte,
    pci_ppc_read_config_word,
    pci_ppc_read_config_dword,
    pci_ppc_write_config_byte,
    pci_ppc_write_config_word,
    pci_ppc_write_config_dword     
};

