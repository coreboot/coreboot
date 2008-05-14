/*
 * inteltool - dump all registers on an Intel CPU + chipset based system.
 *
 * Copyright (C) 2008 by coresystems GmbH 
 *  written by Stefan Reinauer <stepan@coresystems.de> 
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <getopt.h>
#include <sys/mman.h>
#include <sys/io.h>
#include <pci/pci.h>

#define INTELTOOL_VERSION "1.0"

/* Tested Chipsets: */
#define PCI_VENDOR_ID_INTEL		0x8086
#define PCI_DEVICE_ID_INTEL_ICH		0x2410
#define PCI_DEVICE_ID_INTEL_ICH0	0x2420
#define PCI_DEVICE_ID_INTEL_ICH4	0x24c0
#define PCI_DEVICE_ID_INTEL_ICH4M	0x24cc
#define PCI_DEVICE_ID_INTEL_ICH7	0x27b8
#define PCI_DEVICE_ID_INTEL_82945GM	0x27a0

#define ARRAY_SIZE(a) ((int)(sizeof(a) / sizeof((a)[0])))

int fd_mem;
int fd_msr;

typedef struct { uint32_t hi, lo; } msr_t;
typedef struct { uint16_t addr; int size; char *name; } io_register_t;


static const io_register_t ich0_gpio_registers[] =  {
	{ 0x00, 4, "GPIO_USE_SEL" },
	{ 0x04, 4, "GP_IO_SEL" },
	{ 0x08, 4, "RESERVED" },
	{ 0x0c, 4, "GP_LVL" },
	{ 0x10, 4, "RESERVED" },
	{ 0x14, 4, "GPO_TTL" },
	{ 0x18, 4, "GPO_BLINK" },
	{ 0x1c, 4, "RESERVED" },
	{ 0x20, 4, "RESERVED" },
	{ 0x24, 4, "RESERVED" },
	{ 0x28, 4, "RESERVED" },
	{ 0x2c, 4, "GPI_INV" },
	{ 0x30, 4, "RESERVED" },
	{ 0x34, 4, "RESERVED" },
	{ 0x38, 4, "RESERVED" },
	{ 0x3C, 4, "RESERVED" }
};

static const io_register_t ich4_gpio_registers[] =  {
	{ 0x00, 4, "GPIO_USE_SEL" },
	{ 0x04, 4, "GP_IO_SEL" },
	{ 0x08, 4, "RESERVED" },
	{ 0x0c, 4, "GP_LVL" },
	{ 0x10, 4, "RESERVED" },
	{ 0x14, 4, "GPO_TTL" },
	{ 0x18, 4, "GPO_BLINK" },
	{ 0x1c, 4, "RESERVED" },
	{ 0x20, 4, "RESERVED" },
	{ 0x24, 4, "RESERVED" },
	{ 0x28, 4, "RESERVED" },
	{ 0x2c, 4, "GPI_INV" },
	{ 0x30, 4, "GPIO_USE_SEL2" },
	{ 0x34, 4, "GP_IO_SEL2" },
	{ 0x38, 4, "GP_LVL2" },
	{ 0x3C, 4, "RESERVED" }
};

static const io_register_t ich7_gpio_registers[] =  {
	{ 0x00, 4, "GPIO_USE_SEL" },
	{ 0x04, 4, "GP_IO_SEL" },
	{ 0x08, 4, "RESERVED" },
	{ 0x0c, 4, "GP_LVL" },
	{ 0x10, 4, "RESERVED" },
	{ 0x14, 4, "RESERVED" },
	{ 0x18, 4, "GPO_BLINK" },
	{ 0x1c, 4, "RESERVED" },
	{ 0x20, 4, "RESERVED" },
	{ 0x24, 4, "RESERVED" },
	{ 0x28, 4, "RESERVED" },
	{ 0x2c, 4, "GPI_INV" },
	{ 0x30, 4, "GPIO_USE_SEL2" },
	{ 0x34, 4, "GP_IO_SEL2" },
	{ 0x38, 4, "GP_LVL2" },
	{ 0x3C, 4, "RESERVED" }
};

int print_gpios(struct pci_dev *sb)
{
	int i, size;
	uint16_t gpiobase;
	const io_register_t *gpio_registers;

	printf("\n============= GPIOS =============\n\n");

	switch (sb->device_id) {
	case PCI_DEVICE_ID_INTEL_ICH7:
		gpiobase = pci_read_word(sb, 0x48) & 0xfffc;
		gpio_registers = ich7_gpio_registers;
		size = ARRAY_SIZE(ich7_gpio_registers);
		break;
	case PCI_DEVICE_ID_INTEL_ICH4:
	case PCI_DEVICE_ID_INTEL_ICH4M:
		gpiobase = pci_read_word(sb, 0x58) & 0xfffc;
		gpio_registers = ich4_gpio_registers;
		size = ARRAY_SIZE(ich4_gpio_registers);
		break;
	case PCI_DEVICE_ID_INTEL_ICH:
	case PCI_DEVICE_ID_INTEL_ICH0:
		gpiobase = pci_read_word(sb, 0x58) & 0xfffc;
		gpio_registers = ich0_gpio_registers;
		size = ARRAY_SIZE(ich0_gpio_registers);
		break;
	case 0x1234: // Dummy for non-existent functionality
		printf("Error: This southbridge does not have GPIOBASE.\n");
		return 1;
	default:
		printf("Error: Dumping GPIOs on this southbridge is not (yet) supported.\n");
		return 1;
	}

	printf("GPIOBASE = 0x%04x (IO)\n\n", gpiobase);

	for (i=0; i<size; i++) {
		switch (gpio_registers[i].size) {
		case 4:
			printf("gpiobase+0x%04x: 0x%08x (%s)\n", 
				gpio_registers[i].addr,
				inl(gpiobase+gpio_registers[i].addr),
				gpio_registers[i].name);
			break;
		case 2:
			printf("gpiobase+0x%04x: 0x%04x     (%s)\n", 
				gpio_registers[i].addr,
				inw(gpiobase+gpio_registers[i].addr),
				gpio_registers[i].name);
			break;
		case 1:
			printf("gpiobase+0x%04x: 0x%02x       (%s)\n", 
				gpio_registers[i].addr,
				inb(gpiobase+gpio_registers[i].addr),
				gpio_registers[i].name);
			break;
		}
	}

	return 0;
}

int print_rcba(struct pci_dev *sb)
{
	int i, size=0x4000;
	volatile uint8_t *rcba;
	uint32_t rcba_phys;

	printf("\n============= RCBA ==============\n\n");

	switch (sb->device_id) {
	case PCI_DEVICE_ID_INTEL_ICH7:
		rcba_phys = pci_read_long(sb, 0xf0) & 0xfffffffe;  
		break;
	case PCI_DEVICE_ID_INTEL_ICH:
	case PCI_DEVICE_ID_INTEL_ICH0:
	case PCI_DEVICE_ID_INTEL_ICH4:
	case PCI_DEVICE_ID_INTEL_ICH4M:
		printf("Error: This southbridge does not have RCBA.\n");
		return 1;
	default:
		printf("Error: Dumping RCBA on this southbridge is not (yet) supported.\n");
		return 1;
	}

	rcba = mmap(0, size, PROT_WRITE | PROT_READ, MAP_SHARED,
		    fd_mem, (off_t) rcba_phys);
	
	if (rcba == MAP_FAILED) {
		perror("Error mapping RCBA");
		exit(1);
	}

	printf("RCBA = 0x%08x (MEM)\n\n", rcba_phys);

	for (i=0; i<size; i+=4) {
		if(*(uint32_t *)(rcba+i))
			printf("0x%04x: 0x%08x\n", i, *(uint32_t *)(rcba+i));
	}

	munmap((void *) rcba, size);
	return 0;
}

int print_pmbase(struct pci_dev *sb)
{
	int i, size=0x80;
	uint16_t pmbase;

	printf("\n============= PMBASE ============\n\n");

	switch (sb->device_id) {
	case PCI_DEVICE_ID_INTEL_ICH7:
		pmbase = pci_read_word(sb, 0x40) & 0xfffc; 
		break;
	case 0x1234: // Dummy for non-existent functionality
		printf("Error: This southbridge does not have PMBASE.\n");
		return 1;
	default:
		printf("Error: Dumping PMBASE on this southbridge is not (yet) supported.\n");
		return 1;
	}

	printf("PMBASE = 0x%04x (IO)\n\n", pmbase);

	for (i=0; i<size; i+=4) {
		printf("pmbase+0x%04x: 0x%08x\n", i, inl(pmbase+i));
	}

	return 0;
}

/*
 * (G)MCH MMIO Config Space
 */

int print_mchbar(struct pci_dev *nb)
{
	int i, size=(16*1024);
	volatile uint8_t *mchbar;
	uint32_t mchbar_phys;

	printf("\n============= MCHBAR ============\n\n");

	switch (nb->device_id) {
	case PCI_DEVICE_ID_INTEL_82945GM:
		mchbar_phys = pci_read_long(nb, 0x44) & 0xfffffffe;  
		break;
	case 0x1234: // Dummy for non-existent functionality
		printf("Error: This northbrigde does not have MCHBAR.\n");
		return 1;
	default:
		printf("Error: Dumping MCHBAR on this northbridge is not (yet) supported.\n");
		return 1;
	}

	mchbar = mmap(0, size, PROT_WRITE | PROT_READ, MAP_SHARED,
		    fd_mem, (off_t) mchbar_phys );
	
	if (mchbar == MAP_FAILED) {
		perror("Error mapping MCHBAR");
		exit(1);
	}

	printf("MCHBAR = 0x%08x (MEM)\n\n", mchbar_phys);

	for (i=0; i<size; i+=4) {
		if(*(uint32_t *)(mchbar+i))
			printf("0x%04x: 0x%08x\n", i, *(uint32_t *)(mchbar+i));
	}

	munmap((void *) mchbar, size);
	return 0;
}

/*
 * Egress Port Root Complex MMIO configuration space
 */
int print_epbar(struct pci_dev *nb)
{
	int i, size=4096;
	volatile uint8_t *epbar;
	uint32_t epbar_phys;

	printf("\n============= EPBAR =============\n\n");

	switch (nb->device_id) {
	case PCI_DEVICE_ID_INTEL_82945GM:
		epbar_phys = pci_read_long(nb, 0x40) & 0xfffffffe; 
		break;
	case 0x1234: // Dummy for non-existent functionality
		printf("Error: This northbrigde does not have EPBAR.\n");
		return 1;
	default:
		printf("Error: Dumping EPBAR on this northbridge is not (yet) supported.\n");
		return 1;
	}

	epbar = mmap(0, size, PROT_WRITE | PROT_READ, MAP_SHARED,
		    fd_mem, (off_t) epbar_phys );
	
	if (epbar == MAP_FAILED) {
		perror("Error mapping EPBAR");
		exit(1);
	}

	printf("EPBAR = 0x%08x (MEM)\n\n", epbar_phys);
	for (i=0; i<size; i+=4) {
		if(*(uint32_t *)(epbar+i))
			printf("0x%04x: 0x%08x\n", i, *(uint32_t *)(epbar+i));
	}

	munmap((void *) epbar, size);
	return 0;
}


/*
 * MCH-ICH Serial Interconnect Ingress Root Complex  MMIO configuration space
 */
int print_dmibar(struct pci_dev *nb)
{
	int i, size=4096;
	volatile uint8_t *dmibar;
	uint32_t dmibar_phys;

	printf("\n============= DMIBAR ============\n\n");

	switch (nb->device_id) {
	case PCI_DEVICE_ID_INTEL_82945GM:
		dmibar_phys = pci_read_long(nb, 0x4c) & 0xfffffffe; 
		break;
	case 0x1234: // Dummy for non-existent functionality
		printf("Error: This northbrigde does not have DMIBAR.\n");
		return 1;
	default:
		printf("Error: Dumping DMIBAR on this northbridge is not (yet) supported.\n");
		return 1;
	}

	dmibar = mmap(0, size, PROT_WRITE | PROT_READ, MAP_SHARED,
		    fd_mem, (off_t) dmibar_phys );
	
	if (dmibar == MAP_FAILED) {
		perror("Error mapping DMIBAR");
		exit(1);
	}

	printf("DMIBAR = 0x%08x (MEM)\n\n", dmibar_phys);
	for (i=0; i<size; i+=4) {
		if(*(uint32_t *)(dmibar+i))
			printf("0x%04x: 0x%08x\n", i, *(uint32_t *)(dmibar+i));
	}

	munmap((void *) dmibar, size);
	return 0;
}

/*
 * PCIe MMIO configuration space
 */
int print_pciexbar(struct pci_dev *nb)
{
	uint32_t pciexbar_reg;
	uint32_t pciexbar_phys;
	volatile uint8_t *pciexbar;
	int max_busses, devbase, i;
	int bus, dev, fn;

	printf("========= PCIEXBAR ========\n\n");

	switch (nb->device_id) {
	case PCI_DEVICE_ID_INTEL_82945GM:
		pciexbar_reg = pci_read_long(nb, 0x48); 
		break;
	case 0x1234: // Dummy for non-existent functionality
		printf("Error: This northbrigde does not have PCIEXBAR.\n");
		return 1;
	default:
		printf("Error: Dumping PCIEXBAR on this northbridge is not (yet) supported.\n");
		return 1;
	}

	if( !(pciexbar_reg & (1 << 0))) {
		printf("PCIEXBAR register is disabled.\n");
		return 0;
	}

	switch ((pciexbar_reg >> 1) & 3) {
	case 0: // 256MB
		pciexbar_phys = pciexbar_reg & ((1 << 31)|(1 << 30)|(1 << 29)|(1 << 28));
		max_busses = 256;
		break;
	case 1: // 128M
		pciexbar_phys = pciexbar_reg & ((1 << 31)|(1 << 30)|(1 << 29)|(1 << 28)|(1 << 27));
		max_busses = 128;
		break;
	case 2: // 64M
		pciexbar_phys = pciexbar_reg & ((1 << 31)|(1 << 30)|(1 << 29)|(1 << 28)|(1 << 27)|(1 << 26));
		max_busses = 64;
		break;
	default: // RSVD
		printf("Undefined Address base. Bailing out\n");
		return 1;
	}	

	printf("PCIEXBAR: 0x%08x\n", pciexbar_phys);

	pciexbar = mmap(0, (max_busses * 1024 * 1024), PROT_WRITE | PROT_READ, MAP_SHARED,
		    fd_mem, (off_t) pciexbar_phys );
	
	if (pciexbar == MAP_FAILED) {
		perror("Error mapping PCIEXBAR");
		exit(1);
	}
	
	for (bus = 0; bus < max_busses; bus++) {
		for (dev = 0; dev < 32; dev++) {
			for (fn = 0; fn < 8; fn++) {
				devbase = (bus * 1024 * 1024) + (dev * 32 * 1024) + (fn * 4 * 1024);

				if (*(uint16_t *)(pciexbar + devbase) == 0xffff)
					continue;
				
				/* This is a heuristics. Anyone got a better check? */
				if( (*(uint32_t *)(pciexbar + devbase + 256) == 0xffffffff) &&
					(*(uint32_t *)(pciexbar + devbase + 512) == 0xffffffff) ) {
#if DEBUG
					printf("Skipped non-PCIe device %02x:%02x.%01x\n", bus, dev, fn);
#endif
					continue;
				}

				printf("\nPCIe %02x:%02x.%01x extended config space:", bus, dev, fn);
				for (i=0; i<4096; i++) {
					if((i % 0x10) == 0)
						printf("\n%04x:", i);
					printf(" %02x", *(pciexbar+devbase+i));
				}
				printf("\n");
			}
		}
	}

	munmap((void *) pciexbar, (max_busses * 1024 * 1024));

	return 0;
}

int msr_readerror = 0;

msr_t rdmsr(int addr)
{
	unsigned char buf[8];
	msr_t msr = { 0xffffffff, 0xffffffff };

	if (lseek(fd_msr, (off_t) addr, SEEK_SET) == -1) {
		perror("Could not lseek() to MSR");
		close(fd_msr);
		exit(1);
	}

	if (read(fd_msr, buf, 8) == 8) {
		msr.lo = *(uint32_t *)buf;
		msr.hi = *(uint32_t *)(buf+4);

		return msr;
	}

	if (errno == 5) {
		printf(" (*)"); // Not all bits of the MSR could be read
		msr_readerror = 1;
	} else {
		// A severe error.
		perror("Could not read() MSR");
		close(fd_msr);
		exit(1);
	}

	return msr;
}

int print_intel_core_msrs(void)
{
	unsigned int i, core;
	msr_t msr;


#define IA32_PLATFORM_ID		0x0017
#define EBL_CR_POWERON			0x002a
#define FSB_CLK_STS			0x00cd
#define IA32_TIME_STAMP_COUNTER		0x0010
#define IA32_APIC_BASE			0x001b

	typedef struct {
		int number;
		char *name;
	} msr_entry_t;

	msr_entry_t global_msrs[] = {
		{ 0x0017, "IA32_PLATFORM_ID" },
		{ 0x002a, "EBL_CR_POWERON" },
		{ 0x00cd, "FSB_CLOCK_STS" },
		{ 0x00ce, "FSB_CLOCK_VCC" },
		{ 0x00e2, "CLOCK_CST_CONFIG_CONTROL" },
		{ 0x00e3, "PMG_IO_BASE_ADDR" },
		{ 0x00e4, "PMG_IO_CAPTURE_ADDR" },
		{ 0x00ee, "EXT_CONFIG" },
		{ 0x011e, "BBL_CR_CTL3" },
		{ 0x0194, "CLOCK_FLEX_MAX" },
		{ 0x0198, "IA32_PERF_STATUS" },
		{ 0x01a0, "IA32_MISC_ENABLES" },
		{ 0x01aa, "PIC_SENS_CFG" },
		{ 0x0400, "IA32_MC0_CTL" },
		{ 0x0401, "IA32_MC0_STATUS" },
		{ 0x0402, "IA32_MC0_ADDR" },
		//{ 0x0403, "IA32_MC0_MISC" }, // Seems to be RO
		{ 0x040c, "IA32_MC4_CTL" },
		{ 0x040d, "IA32_MC4_STATUS" },
		{ 0x040e, "IA32_MC4_ADDR" },
		//{ 0x040f, "IA32_MC4_MISC" } // Seems to be RO
	};

	msr_entry_t per_core_msrs[] = {
		{ 0x0010, "IA32_TIME_STAMP_COUNTER" },
		{ 0x001b, "IA32_APIC_BASE" },
		{ 0x003a, "IA32_FEATURE_CONTROL" },
		{ 0x003f, "IA32_TEMPERATURE_OFFSET" },
		//{ 0x0079, "IA32_BIOS_UPDT_TRIG" }, // Seems to be RO
		{ 0x008b, "IA32_BIOS_SIGN_ID" },
		{ 0x00e7, "IA32_MPERF" },
		{ 0x00e8, "IA32_APERF" },
		{ 0x00fe, "IA32_MTRRCAP" },
		{ 0x015f, "DTS_CAL_CTRL" },
		{ 0x0179, "IA32_MCG_CAP" },
		{ 0x017a, "IA32_MCG_STATUS" },
		{ 0x0199, "IA32_PERF_CONTROL" },
		{ 0x019a, "IA32_CLOCK_MODULATION" },
		{ 0x019b, "IA32_THERM_INTERRUPT" },
		{ 0x019c, "IA32_THERM_STATUS" },
		{ 0x019d, "GV_THERM" },
		{ 0x01d9, "IA32_DEBUGCTL" },
		{ 0x0200, "IA32_MTRR_PHYSBASE0" },
		{ 0x0201, "IA32_MTRR_PHYSMASK0" },
		{ 0x0202, "IA32_MTRR_PHYSBASE1" },
		{ 0x0203, "IA32_MTRR_PHYSMASK1" },
		{ 0x0204, "IA32_MTRR_PHYSBASE2" },
		{ 0x0205, "IA32_MTRR_PHYSMASK2" },
		{ 0x0206, "IA32_MTRR_PHYSBASE3" },
		{ 0x0207, "IA32_MTRR_PHYSMASK3" },
		{ 0x0208, "IA32_MTRR_PHYSBASE4" },
		{ 0x0209, "IA32_MTRR_PHYSMASK4" },
		{ 0x020a, "IA32_MTRR_PHYSBASE5" },
		{ 0x020b, "IA32_MTRR_PHYSMASK5" },
		{ 0x020c, "IA32_MTRR_PHYSBASE6" },
		{ 0x020d, "IA32_MTRR_PHYSMASK6" },
		{ 0x020e, "IA32_MTRR_PHYSBASE7" },
		{ 0x020f, "IA32_MTRR_PHYSMASK7" },
		{ 0x0250, "IA32_MTRR_FIX64K_00000" },
		{ 0x0258, "IA32_MTRR_FIX16K_80000" },
		{ 0x0259, "IA32_MTRR_FIX16K_A0000" },
		{ 0x0268, "IA32_MTRR_FIX4K_C0000" },
		{ 0x0269, "IA32_MTRR_FIX4K_C8000" },
		{ 0x026a, "IA32_MTRR_FIX4K_D0000" },
		{ 0x026b, "IA32_MTRR_FIX4K_D8000" },
		{ 0x026c, "IA32_MTRR_FIX4K_E0000" },
		{ 0x026d, "IA32_MTRR_FIX4K_E8000" },
		{ 0x026e, "IA32_MTRR_FIX4K_F0000" },
		{ 0x026f, "IA32_MTRR_FIX4K_F8000" },
		{ 0x02ff, "IA32_MTRR_DEF_TYPE" },
		//{ 0x00c000080, "IA32_CR_EFER" }, // Seems to be RO
	};

 	fd_msr = open("/dev/cpu/0/msr", O_RDWR);
	if (fd_msr<0) {
		perror("Error while opening /dev/cpu/0/msr");
		printf("Did you run 'modprobe msr'?\n");
		return -1;
	}

	printf("\n===================== SHARED MSRs (All Cores) =====================\n");

	for (i = 0; i < ARRAY_SIZE(global_msrs); i++) {
		msr = rdmsr(global_msrs[i].number);
		printf(" MSR 0x%08X = 0x%08X:0x%08X (%s)\n",
			     global_msrs[i].number, msr.hi, msr.lo, global_msrs[i].name);
	}


	close(fd_msr);
	
	for (core=0; core < 8; core++) {
		char msrfilename[64];
		memset(msrfilename, 0, 64);
		sprintf(msrfilename, "/dev/cpu/%d/msr", core);

		fd_msr = open(msrfilename, O_RDWR);
		if (fd_msr<0) {
			/* If the file is not there, we're probably through. 
			 * No error, since we successfully opened /dev/cpu/0/msr before
			 */
			break;
		}

		printf("\n====================== UNIQUE MSRs  (core %d) ======================\n", core);

		for (i = 0; i < ARRAY_SIZE(per_core_msrs); i++) {
			msr = rdmsr(per_core_msrs[i].number);
			printf(" MSR 0x%08X = 0x%08X:0x%08X (%s)\n",
				     per_core_msrs[i].number, msr.hi, msr.lo, per_core_msrs[i].name);
		}

		close(fd_msr);
	}

	if (msr_readerror)
		printf("\n(*) Some MSRs could not be read. The marked values are unreliable.\n");	

	return 0;
}

void print_version(void)
{
	printf("inteltool v%s -- ", INTELTOOL_VERSION);
	printf("Copyright (C) 2008 coresystems GmbH\n\n");
	printf(
    "This program is free software: you can redistribute it and/or modify\n"
    "it under the terms of the GNU General Public License as published by\n"
    "the Free Software Foundation, version 2 of the License.\n\n"
    "This program is distributed in the hope that it will be useful,\n"
    "but WITHOUT ANY WARRANTY; without even the implied warranty of\n"
    "MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the\n"
    "GNU General Public License for more details.\n\n"
    "You should have received a copy of the GNU General Public License\n"
    "along with this program.  If not, see <http://www.gnu.org/licenses/>.\n\n");
}

void print_usage(const char *name)
{
	printf("usage: %s [-vh?grpmedPMa]\n", name);
	printf("\n"
	     "   -v | --version:                   print the version\n"
	     "   -h | --help:                      print this help\n\n"
	     "   -g | --gpio:                      dump soutbridge GPIO registers\n"
	     "   -r | --rcba:                      dump soutbridge RCBA registers\n"
	     "   -p | --pmbase:                    dump soutbridge Power Management registers\n\n"
	     "   -m | --mchbar:                    dump northbridge Memory Controller registers\n"
	     "   -e | --epbar:                     dump northbridge EPBAR registers\n"
	     "   -d | --dmibar:                    dump northbridge DMIBAR registers\n"
	     "   -P | --pciexpress:                dump northbridge PCIEXBAR registers\n\n"
	     "   -M | --msrs:                      dump CPU MSRs\n"
	     "   -a | --all:                       dump all known registers\n"
             "\n");
	exit(1);
}

int main(int argc, char *argv[])
{
	struct pci_access *pacc;
	struct pci_dev *sb, *nb;
	int opt;
	int option_index = 0;
	int i;

	char *sbname="unknown", *nbname="unknown";

	int dump_gpios=0, dump_mchbar=0, dump_rcba=0;
	int dump_pmbase=0, dump_epbar=0, dump_dmibar=0;
	int dump_pciexbar=0, dump_coremsrs=0;

	static struct option long_options[] = {
		{"version", 0, 0, 'v'},
		{"help", 0, 0, 'h'},
		{"gpios", 0, 0, 'g'},
		{"mchbar", 0, 0, 'm'},
		{"rcba", 0, 0, 'r'},
		{"pmbase", 0, 0, 'p'},
		{"epbar", 0, 0, 'e'},
		{"dmibar", 0, 0, 'd'},
		{"pciexpress", 0, 0, 'P'},
		{"msrs", 0, 0, 'M'},
		{"all", 0, 0, 'a'},
		{0, 0, 0, 0}
	};

	struct {
		uint16_t vendor_id, device_id;
		char * name;
	} supported_chips_list[] = {
		{ PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_82945GM, "i945" },
		{ PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_ICH7, "ICH7" },
		{ PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_ICH4M, "ICH4-M" },
		{ PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_ICH4, "ICH4" },
		{ PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_ICH0, "ICH0" },
		{ PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_ICH, "ICH" }
	};

	while ((opt = getopt_long(argc, argv, "vh?grpmedPMa",
                                 long_options, &option_index)) != EOF) {
		switch (opt) {
		case 'v':
			print_version();
			exit(0);
			break;
		case 'g':
			dump_gpios = 1;
			break;
		case 'm':
			dump_mchbar = 1;
			break;
		case 'r':
			dump_rcba = 1;
			break;
		case 'p':
			dump_pmbase = 1;
			break;
		case 'e':
			dump_epbar = 1;
			break;
		case 'd':
			dump_dmibar = 1;
			break;
		case 'P':
			dump_pciexbar = 1;
			break;
		case 'M':
			dump_coremsrs = 1;
			break;
		case 'a':
			dump_gpios = 1;
			dump_mchbar = 1;
			dump_rcba = 1;
			dump_pmbase = 1;
			dump_epbar = 1;
			dump_dmibar = 1;
			dump_pciexbar = 1;
			dump_coremsrs = 1;
			break;
		case 'h':
		case '?':
		default:
			print_usage(argv[0]);
			exit(0);
			break;
		}
	}

	if (iopl(3)) { printf("You need to be root.\n"); exit(1); }

	if ((fd_mem = open("/dev/mem", O_RDWR)) < 0) {
		perror("Can not open /dev/mem");
		exit(1);
	}

	pacc = pci_alloc();
	pci_init(pacc);
	pci_scan_bus(pacc);


	/* Find the required devices */

	sb = pci_get_dev(pacc, 0, 0, 0x1f, 0);
	if (!sb) {
		printf("No southbridge found.\n");
		exit(1);
	}

	pci_fill_info(sb, PCI_FILL_IDENT|PCI_FILL_BASES|PCI_FILL_SIZES|PCI_FILL_CLASS);

	if (sb->vendor_id != PCI_VENDOR_ID_INTEL) {
		printf("Not an Intel(R) southbridge.\n");
		exit(1);
	}

	nb = pci_get_dev(pacc, 0, 0, 0x00, 0);
	if (!nb) {
		printf("No northbridge found.\n");
		exit(1);
	}

	pci_fill_info(nb, PCI_FILL_IDENT|PCI_FILL_BASES|PCI_FILL_SIZES|PCI_FILL_CLASS);

	if (nb->vendor_id != PCI_VENDOR_ID_INTEL) {
		printf("Not an Intel(R) northbridge.\n");
		exit(1);
	}

	/* TODO check cpuid, too */

	/* Determine names */
	for (i=0; i<ARRAY_SIZE(supported_chips_list); i++)
		if (nb->device_id == supported_chips_list[i].device_id)
			nbname = supported_chips_list[i].name;
	for (i=0; i<ARRAY_SIZE(supported_chips_list); i++)
		if (sb->device_id == supported_chips_list[i].device_id)
			sbname = supported_chips_list[i].name;

	printf("Intel Northbridge: %04x:%04x (%s)\n", 
		nb->vendor_id, nb->device_id, nbname);

	printf("Intel Southbridge: %04x:%04x (%s)\n", 
		sb->vendor_id, sb->device_id, sbname);

	/* Now do the deed */

	if (dump_gpios) {
		print_gpios(sb);
		printf("\n\n");
	}

	if (dump_rcba) {
		print_rcba(sb);
		printf("\n\n");
	}

	if (dump_pmbase) {
		print_pmbase(sb);
		printf("\n\n");
	}

	if (dump_mchbar) {
		print_mchbar(nb);
		printf("\n\n");
	}

	if (dump_epbar) {
		print_epbar(nb);
		printf("\n\n");
	}

	if (dump_dmibar) {
		print_dmibar(nb);
		printf("\n\n");
	}

	if (dump_pciexbar) {
		print_pciexbar(nb);
		printf("\n\n");
	}

	if (dump_coremsrs) {
		print_intel_core_msrs();
		printf("\n\n");
	}


	/* Clean up */

	pci_free_dev(nb);
	pci_free_dev(sb);
	pci_cleanup(pacc);

	return 0;
}
