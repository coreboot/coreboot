/*
 *
 *  Copyright 2005 AMD
 *  2005.9 yhlu make it more dynamic for AMD Opteron Based MB
 */

#include <console/console.h>
#include <string.h>
#include <arch/acpi.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <cpu/x86/msr.h>
#include <cpu/amd/mtrr.h>

#define DUMP_ACPI_TABLES 0

#if DUMP_ACPI_TABLES == 1
static void dump_mem(unsigned start, unsigned end)
{
        
	unsigned i;
        print_debug("dump_mem:");
        for(i=start;i<end;i++) {
                if((i & 0xf)==0) {
                        printk_debug("\n%08x:", i);
                }
                printk_debug(" %02x", (unsigned char)*((unsigned char *)i));
        }
        print_debug("\n");
 }
#endif

#define HC_POSSIBLE_NUM 8
extern unsigned char AmlCode[];
extern unsigned char AmlCode_ssdt[];

#if ACPI_SSDTX_NUM >= 1
extern unsigned char AmlCode_ssdt2[];
//extern unsigned char AmlCode_ssdt3[];
//extern unsigned char AmlCode_ssdt4[];
//extern unsigned char AmlCode_ssdt5[];
//extern unsigned char AmlCode_ssdt6[];
//extern unsigned char AmlCode_ssdt7[];
//extern unsigned char AmlCode_ssdt8[];
#endif

#define IO_APIC_ADDR	0xfec00000UL

extern  unsigned char bus_isa;
extern  unsigned char bus_8132_0;
extern  unsigned char bus_8132_1;
extern  unsigned char bus_8132_2;
extern  unsigned char bus_8111_0;
extern  unsigned char bus_8111_1;
extern  unsigned char bus_8151_0;
extern  unsigned char bus_8151_1;
extern  unsigned apicid_8111;
extern  unsigned apicid_8132_1;
extern  unsigned apicid_8132_2;

extern  unsigned pci1234[];
extern  unsigned hc_possible_num;
extern  unsigned sblk;
extern  unsigned sbdn;

unsigned long acpi_fill_madt(unsigned long current)
{
	unsigned int gsi_base=0x18;
 
	/* create all subtables for processors */
	current = acpi_create_madt_lapics(current);
	
	/* Write 8111 IOAPIC */
	current += acpi_create_madt_ioapic((acpi_madt_ioapic_t *)current, apicid_8111,
			IO_APIC_ADDR, 0);

        /* Write all 8131 IOAPICs */
        {
                device_t dev;
                struct resource *res;
                dev = dev_find_slot(bus_8132_0, PCI_DEVFN(0x1 + HT_CHAIN_UNITID_BASE - 1, 1));
                if (dev) {
                        res = find_resource(dev, PCI_BASE_ADDRESS_0);
                        if (res) {
			        current += acpi_create_madt_ioapic((acpi_madt_ioapic_t *)current, apicid_8132_1,
		                        res->base, gsi_base );
				gsi_base+=7;

                        }
                }
                dev = dev_find_slot(bus_8132_0, PCI_DEVFN(0x2 + HT_CHAIN_UNITID_BASE - 1, 1));
                if (dev) {
                        res = find_resource(dev, PCI_BASE_ADDRESS_0);
                        if (res) {
                                current += acpi_create_madt_ioapic((acpi_madt_ioapic_t *)current, apicid_8132_2,
                                        res->base, gsi_base );
                                gsi_base+=7;
                        }
                }
        }

	current += acpi_create_madt_irqoverride( (acpi_madt_irqoverride_t *)
			current, 0, 0, 2, 5 );
		/* 0: mean bus 0--->ISA */
		/* 0: PIC 0 */
		/* 2: APIC 2 */ 
		/* 5 mean: 0101 --> Edige-triggered, Active high*/


	        /* create all subtables for processors */
        current = acpi_create_madt_lapic_nmis(current, 5, 1);
		/* 1: LINT1 connect to NMI */


	return current;
}

//FIXME: next could be moved to northbridge/amd/amdk8/amdk8_acpi.c or cpu/amd/k8/k8_acpi.c begin
static void int_to_stream(uint32_t val, uint8_t *dest)
{
	int i;
	for(i=0;i<4;i++) {
		*(dest+i) = (val >> (8*i)) & 0xff;
	}
}

extern void get_bus_conf(void);

static void update_ssdt(void *ssdt)
{
	uint8_t *BUSN;
	uint8_t *MMIO;
	uint8_t *PCIO;
	uint8_t *SBLK;
	uint8_t *TOM1;
	uint8_t *HCLK;
	uint8_t *SBDN;
	int i;
	device_t dev;
	uint32_t dword;
	msr_t msr;
	
	BUSN = ssdt+0x3a; //+5 will be next BUSN
	MMIO = ssdt+0x57; //+5 will be next MMIO
	PCIO = ssdt+0xaf; //+5 will be next PCIO
	SBLK = ssdt+0xdc; // one byte
	TOM1 = ssdt+0xe3; //
	HCLK = ssdt+0xfa; //+5 will be next HCLK
	SBDN = ssdt+0xed;//

        dev = dev_find_slot(0, PCI_DEVFN(0x18, 1));

	for(i=0;i<4;i++) {
		dword = pci_read_config32(dev, 0xe0+i*4);
		int_to_stream(dword, BUSN+i*5); 
	}	

        for(i=0;i<0x10;i++) {
                dword = pci_read_config32(dev, 0x80+i*4);
                int_to_stream(dword, MMIO+i*5);
        }

        for(i=0;i<0x08;i++) {
                dword = pci_read_config32(dev, 0xc0+i*4);
                int_to_stream(dword, PCIO+i*5);
        }
	
	get_bus_conf(); //it will get sblk, pci1234, and sbdn
	
	*SBLK = (uint8_t)(sblk);

	msr = rdmsr(TOP_MEM);
	int_to_stream(msr.lo, TOM1);

	for(i=0;i<hc_possible_num;i++) {
		int_to_stream(pci1234[i], HCLK + i*5);
	}
	for(i=hc_possible_num; i<HC_POSSIBLE_NUM; i++) { // in case we set array size to other than 8
		int_to_stream(0x00000000, HCLK + i*5);
	}

	int_to_stream(sbdn, SBDN);
		
}
//end

unsigned long write_acpi_tables(unsigned long start)
{
	unsigned long current;
	acpi_rsdp_t *rsdp;
	acpi_rsdt_t *rsdt;
	acpi_hpet_t *hpet;
	acpi_madt_t *madt;
	acpi_srat_t *srat;
	acpi_fadt_t *fadt;
	acpi_facs_t *facs;
	acpi_header_t *dsdt;
	acpi_header_t *ssdt;
	acpi_header_t *ssdtx;

	unsigned char *AmlCode_ssdtx[HC_POSSIBLE_NUM];

	int i;

	/* Align ACPI tables to 16byte */
	start   = ( start + 0x0f ) & -0x10;
	current = start;
	
	printk_info("ACPI: Writing ACPI tables at %lx...\n", start);

	/* We need at least an RSDP and an RSDT Table */
	rsdp = (acpi_rsdp_t *) current;
	current += sizeof(acpi_rsdp_t);
	rsdt = (acpi_rsdt_t *) current;
	current += sizeof(acpi_rsdt_t);

	/* clear all table memory */
	memset((void *)start, 0, current - start);
	
	acpi_write_rsdp(rsdp, rsdt);
	acpi_write_rsdt(rsdt);

	/*
	 * We explicitly add these tables later on:
	 */
	printk_debug("ACPI:    * HPET\n");
	hpet = (acpi_hpet_t *) current;
	current += sizeof(acpi_hpet_t);
	acpi_create_hpet(hpet);
	acpi_add_table(rsdt,hpet);

	/* If we want to use HPET Timers Linux wants an MADT */
	printk_debug("ACPI:    * MADT\n");
	madt = (acpi_madt_t *) current;
	acpi_create_madt(madt);
	current+=madt->header.length;
	acpi_add_table(rsdt,madt);


	/* SRAT */
        printk_debug("ACPI:    * SRAT\n");
        srat = (acpi_srat_t *) current;
        acpi_create_srat(srat);
        current+=srat->header.length;
        acpi_add_table(rsdt,srat);

	/* SSDT */
	printk_debug("ACPI:    * SSDT\n");
	ssdt = (acpi_header_t *)current;
	current += ((acpi_header_t *)AmlCode_ssdt)->length;
	memcpy((void *)ssdt, (void *)AmlCode_ssdt, ((acpi_header_t *)AmlCode_ssdt)->length);
	//Here you need to set value in pci1234, sblk and sbdn in get_bus_conf.c
	update_ssdt((void*)ssdt);
        /* recalculate checksum */
        ssdt->checksum = 0;
        ssdt->checksum = acpi_checksum((unsigned char *)ssdt,ssdt->length);
	acpi_add_table(rsdt,ssdt);

#if ACPI_SSDTX_NUM >= 1
	// we need to make ssdt2 match to PCI2 in pci2.asl,... pci1234[1] 
	AmlCode_ssdtx[1] = AmlCode_ssdt2;
//	AmlCode_ssdtx[2] = AmlCode_ssdt3;
//	AmlCode_ssdtx[3] = AmlCode_ssdt4;
//      AmlCode_ssdtx[4] = AmlCode_ssdt5;
//      AmlCode_ssdtx[5] = AmlCode_ssdt6;
//      AmlCode_ssdtx[6] = AmlCode_ssdt7;
//      AmlCode_ssdtx[7] = AmlCode_ssdt8;

	//same htio, but different possition? We may have to copy, change HCIN, and recalculate the checknum and add_table
	
	for(i=1;i<hc_possible_num;i++) {  // 0: is hc sblink
		if((pci1234[i] & 1) != 1 ) continue;
	        printk_debug("ACPI:    * SSDT for PCI%d\n", i+1); //pci0 and pci1 are in dsdt
        	ssdtx = (acpi_header_t *)current;
	        current += ((acpi_header_t *)AmlCode_ssdtx[i])->length;
	        memcpy((void *)ssdtx, (void *)AmlCode_ssdtx[i], ((acpi_header_t *)AmlCode_ssdtx[i])->length);
        	acpi_add_table(rsdt,ssdtx);
	}
#endif


	/* FACS */
	printk_debug("ACPI:    * FACS\n");
	facs = (acpi_facs_t *) current;
	current += sizeof(acpi_facs_t);
	acpi_create_facs(facs);

	/* DSDT */
	printk_debug("ACPI:    * DSDT\n");
	dsdt = (acpi_header_t *)current;
	current += ((acpi_header_t *)AmlCode)->length;
	memcpy((void *)dsdt,(void *)AmlCode, \
			((acpi_header_t *)AmlCode)->length);
	printk_debug("ACPI:    * DSDT @ %08x Length %x\n",dsdt,dsdt->length);

	/* FDAT */
	printk_debug("ACPI:    * FADT\n");
	fadt = (acpi_fadt_t *) current;
	current += sizeof(acpi_fadt_t);

	acpi_create_fadt(fadt,facs,dsdt);
	acpi_add_table(rsdt,fadt);

#if DUMP_ACPI_TABLES == 1
	printk_debug("rsdp\n");
	dump_mem(rsdp, ((void *)rsdp) + sizeof(acpi_rsdp_t));

        printk_debug("rsdt\n");
        dump_mem(rsdt, ((void *)rsdt) + sizeof(acpi_rsdt_t));

        printk_debug("madt\n");
        dump_mem(madt, ((void *)madt) + madt->header.length);

        printk_debug("srat\n");
        dump_mem(srat, ((void *)srat) + srat->header.length);

        printk_debug("ssdt\n");
        dump_mem(ssdt, ((void *)ssdt) + ssdt->length);

        printk_debug("fadt\n");
        dump_mem(fadt, ((void *)fadt) + fadt->header.length);
#endif

	printk_info("ACPI: done.\n");
	return current;
}

