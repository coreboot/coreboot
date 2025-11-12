/* amdtool - dump all registers on an AMD CPU + chipset based system */
/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef AMDTOOL_H
#define AMDTOOL_H 1

#if defined(__linux__)
#include <linux/stddef.h>
#endif
#include <arch/mmio.h>
#include <commonlib/helpers.h>

#include <stdint.h>

#if defined(__linux__)
#include <sys/io.h>
#endif
#if (defined(__MACH__) && defined(__APPLE__))
/* DirectHW is available here: https://www.coreboot.org/DirectHW */
#define __DARWIN__
#include <DirectHW/DirectHW.h>
#endif

#ifdef __NetBSD__
#include <pciutils/pci.h>
#else
#include <pci/pci.h>
#endif

/* This #include is needed for freebsd_{rd,wr}msr. */
#if defined(__FreeBSD__)
#include <machine/cpufunc.h>
#endif

#ifdef __NetBSD__
static inline uint8_t inb(unsigned port)
{
	uint8_t data;
	__asm volatile("inb %w1,%0" : "=a" (data) : "d" (port));
	return data;
}
static inline uint16_t inw(unsigned port)
{
	uint16_t data;
	__asm volatile("inw %w1,%0": "=a" (data) : "d" (port));
	return data;
}
static inline uint32_t inl(unsigned port)
{
	uint32_t data;
	__asm volatile("inl %w1,%0": "=a" (data) : "d" (port));
	return data;
}

static inline void outb(uint8_t value, uint16_t port)
{
	__asm__ __volatile__ ("outb %0, %w1" : : "a" (value), "d" (port));
}

static inline void outw(uint16_t value, uint16_t port)
{
	__asm__ __volatile__ ("outw %0, %w1" : : "a" (value), "d" (port));
}

static inline void outl(uint32_t value, uint16_t port)
{
	__asm__ __volatile__ ("outl %0, %w1" : : "a" (value), "d" (port));
}
#endif

#define AMDTOOL_VERSION "0.1"

#define PCI_VENDOR_ID_AMD			0x1022

#define PCI_DEVICE_ID_AMD_FCH_SMB_1		0x780b
#define PCI_DEVICE_ID_AMD_FCH_LPC_1		0x780e
#define PCI_DEVICE_ID_AMD_FCH_SMB_2		0x790b
#define PCI_DEVICE_ID_AMD_FCH_LPC_2		0x790e

#define PCI_DEVICE_ID_AMD_BRH_ROOT_COMPLEX	0x153a
#define PCI_DEVICE_ID_AMD_BRH_DATA_FABRIC_0	0x12c0
#define PCI_DEVICE_ID_AMD_BRH_DATA_FABRIC_1	0x12c1
#define PCI_DEVICE_ID_AMD_BRH_DATA_FABRIC_2	0x12c2
#define PCI_DEVICE_ID_AMD_BRH_DATA_FABRIC_3	0x12c3
#define PCI_DEVICE_ID_AMD_BRH_DATA_FABRIC_4	0x12c4
#define PCI_DEVICE_ID_AMD_BRH_DATA_FABRIC_5	0x12c5
#define PCI_DEVICE_ID_AMD_BRH_DATA_FABRIC_6	0x12c6
#define PCI_DEVICE_ID_AMD_BRH_DATA_FABRIC_7	0x12c7

#define PCI_DEVICE_ID_AMD_PHX_ROOT_COMPLEX	0x14e8
#define PCI_DEVICE_ID_AMD_PHX_DATA_FABRIC_0	0x14f0
#define PCI_DEVICE_ID_AMD_PHX_DATA_FABRIC_1	0x14f1
#define PCI_DEVICE_ID_AMD_PHX_DATA_FABRIC_2	0x14f2
#define PCI_DEVICE_ID_AMD_PHX_DATA_FABRIC_3	0x14f3
#define PCI_DEVICE_ID_AMD_PHX_DATA_FABRIC_4	0x14f4
#define PCI_DEVICE_ID_AMD_PHX_DATA_FABRIC_5	0x14f5
#define PCI_DEVICE_ID_AMD_PHX_DATA_FABRIC_6	0x14f6
#define PCI_DEVICE_ID_AMD_PHX_DATA_FABRIC_7	0x14f7

#define CPUID_TURIN_C1				0x00b00f21
#define CPUID_PHOENIX_A1			0x00a70f41
#define CPUID_PHOENIX_A2			0x00a70f52
#define CPUID_PHOENIX2_A0			0x00a70f80
#define CPUID_HAWKPOINT2_A0			0x00a70fc0

#if !defined(__DARWIN__) && !defined(__FreeBSD__)
typedef struct { uint32_t hi, lo; } msr_t;
#endif
#if defined (__FreeBSD__)
/* FreeBSD already has conflicting definitions for wrmsr/rdmsr. */
#undef rdmsr
#undef wrmsr
#define rdmsr freebsd_rdmsr
#define wrmsr freebsd_wrmsr
typedef struct { uint32_t hi, lo; } msr_t;
#endif
typedef struct { uint16_t addr; int size; char *name; } io_register_t;
typedef struct {
	uint32_t eax;
	uint32_t ebx;
	uint32_t ecx;
	uint32_t edx;
} cpuid_result_t;

void *map_physical(uint64_t phys_addr, size_t len);
void unmap_physical(void *virt_addr, size_t len);

int find_smbus_dev_rev(uint16_t vendor, uint16_t device);

uint32_t cpuid(uint32_t eax);
int print_amd_msrs(void);
int print_cpu_info(void);
int print_lpc(struct pci_dev *sb);
int print_espi(struct pci_dev *sb, struct pci_dev *nb);
int print_gpios(struct pci_dev *sb, struct pci_dev *nb, int show_all, int show_diffs);
int print_spi(struct pci_dev *sb, struct pci_dev *nb);
int print_acpimmio(struct pci_dev *sb);
void print_psb(struct pci_dev *nb);
int print_irq_routing(struct pci_dev *sb, struct pci_dev *nb);

#endif
