#include <pci_ids.h>
#include <pci.h>
#include <arch/io.h>
#include <printk.h>
#include <delay.h>
#include <part/mainboard.h>
#include <part/hard_reset.h>
#include <arch/smp/mpspec.h>
#include <pc80/mc146818rtc.h>
#include <pc80/isa_dma.h>
#include <superio/w83627hf.h>
#include <superio/generic.h>
#include <southbridge/amd/amd768.h>
#include <northbridge/amd/amd76x.h>

unsigned long initial_apicid[MAX_CPUS] =
{
	0, 1
};

static void lpc_routing_fixup(void)
{
	struct pci_dev *dev;
	dev = pci_find_device(PCI_VENDOR_ID_AMD, PCI_DEVICE_ID_AMD_SB768_7440,0);
	if (dev != NULL) {

        	/* This register is not documented, but the factory sets 
			it to 0xde */
                pci_write_config_byte(dev, 0x75, 0xde);

		/* States in the documentation that this bit must be set
			for things to work right */
                pci_write_config_byte(dev, 0x4d, 0x04);
        
#if 0
		/* Send keyboard controller,
		 * FDC1, ECP
		 * to the LPC bus
		 * Send APCI ports 0x62 & 0x66 to the ISA bus
		 */
		pci_write_config_byte(dev, 0x51, (0<<7)|(1<<6)|(0<<5)|(1<<4)|(0<<3)|(1));
		/* Route io for both serial ports to the LPC bus */
		pci_write_config_byte(dev, 0x52, (1<<7)|(1<<4)|(1<<3)|(0<<0));
		/* Don't route any audio ports to the LPC bus */
		pci_write_config_byte(dev, 0x53, (0<<6)|(0<<4)|(0<<3)|(0<<2)|(0<<0));
		/* Route superio configuration accesses to the LPC bus */
		pci_write_config_byte(dev, 0x54, (0<<5)|(1<<4)|(0<<2)|(0<<0));
		/* Don't use LPC decode register 4 */
		pci_write_config_byte(dev, 0x55, (0<<4)|(0<<0));
		/* Don't use LPC decode register 5 */
		pci_write_config_byte(dev, 0x56, (0<<4)|(0<<0));
		/* Route 512 byte io address range 0x0c00 - 0xc200 the LPC bus */
		pci_write_config_dword(dev, 0x58, 0x00000c01);		
		/* Route 1MB memory address range 0 - 0 to  the LPC bus */
		pci_write_config_dword(dev, 0x5c, 0x00000000);		
#endif
		
	}
}


static void enable_reset_port_0xcf9(void)
{
	struct pci_dev *dev;
	dev = pci_find_device(PCI_VENDOR_ID_AMD, PCI_DEVICE_ID_AMD_SB768_7443, 0);
	if (dev != NULL) {
		unsigned char byte;
		/* Enable reset port 0xcf9 */
		pci_read_config_byte(dev, 0x41, &byte);		
		pci_write_config_byte(dev, 0x41, byte | (1<<7) | (1<<5));
		pci_write_config_byte(dev, 0x42, 0x09);
	}
}


static void print_whami(void)
{
	struct pci_dev *dev;
	u32 whami;
	/* Find out which processor I'm running on, and if it was the boot
	 * procesor so I can verify everything was setup correctly
	 */
	dev = pci_find_device(PCI_VENDOR_ID_AMD, PCI_DEVICE_ID_AMD_FE_GATE_700C,0);
	if (dev  != NULL) {
		pci_read_config_dword(dev, 0x80, &whami);
		printk_spew("whami = 0x%08lx\n", whami);
	}
}

static void check_cpus(void)
{
        struct pci_dev *dev;
        u32 biu0, biu1;

        /* Find out which processor I'm running on, and if it was the boot
         * procesor so I can verify everything was setup correctly
         */
        dev = pci_find_device(PCI_VENDOR_ID_AMD, PCI_DEVICE_ID_AMD_FE_GATE_700C,0);
        if (dev  != NULL) {
                pci_read_config_dword(dev, 0x60, &biu0);
                if( !(biu0 & 0x80000000)) {
                        initial_apicid[0] = -1;
                }
                pci_read_config_dword(dev, 0x68, &biu1);
		if( !(biu1 & 0x80000000)) { 
			initial_apicid[1] = -1;
		}
                printk_debug("biu0 = 0x%08lx, apicid[0] = %d\n", biu0,
                        initial_apicid[0]);
                printk_debug("biu1 = 0x%08lx, apicid[1] = %d\n", biu1, 
			initial_apicid[1]);
        }
}



static void fixup_adaptec_7899P(struct pci_dev *pdev)
{
	/* Enable writes to the device id */
	pci_write_config_byte(pdev, 0xff, 1);
	/* Set the device id */
	pci_write_config_word(pdev, PCI_DEVICE_ID, PCI_DEVICE_ID_ADAPTEC2_7899P);
	/* Set the subsytem vendor id */
	pci_write_config_word(pdev, PCI_SUBSYSTEM_VENDOR_ID, PCI_VENDOR_ID_TYAN);
	/* Set the subsytem id */
	pci_write_config_word(pdev, PCI_SUBSYSTEM_ID, 0x2469);
	/* Disable writes to the device id */
	pci_write_config_byte(pdev, 0xff, 0);
}

static void onboard_scsi_fixup(void)
{
	struct pci_dev *dev;

	/* Set the scsi device id's */
	dev = pci_find_slot(0, PCI_DEVFN(0xd, 0));
	if (dev != NULL) {
		fixup_adaptec_7899P(dev);
	}
	/* Set the scsi device id's */
	dev = pci_find_slot(0, PCI_DEVFN(0xd, 1));
	if (dev != NULL) {
		fixup_adaptec_7899P(dev);
	}
}


#define MAINBOARD_POWER_OFF 0
#define MAINBOARD_POWER_ON 1

#ifndef MAINBOARD_POWER_ON_AFTER_POWER_FAIL
#define MAINBOARD_POWER_ON_AFTER_POWER_FAIL MAINBOARD_POWER_ON
#endif

static void set_power_on_after_power_fail(int setting)
{
        switch(setting) {
        case MAINBOARD_POWER_ON:
        default:
                amd768_power_after_power_fail(1);
                w83627hf_power_after_power_fail(POWER_ON);
                break;
        case MAINBOARD_POWER_OFF:
                amd768_power_after_power_fail(0);
                w83627hf_power_after_power_fail(POWER_OFF);
                break;

        }
}


#define ECC_DISABLE 0
#define ECC_ENABLE  1

#ifndef ECC_OPTION
#define ECC_OPTION ECC_ENABLE
#endif

static void set_ecc(int setting)
{
        struct pci_dev *dev;

        dev = pci_find_device(PCI_VENDOR_ID_AMD, PCI_DEVICE_ID_AMD_FE_GATE_700C, 0);
        if (dev) {
	        switch(setting) {
	        case ECC_ENABLE:
	        default:
	                pci_write_config_dword(dev, 0x48,
	                	(3 << 14)|(2 << 10)|(3 << 8)|(0 << 4)|(0 << 0));
       	         break;
       		 case ECC_DISABLE:
                        pci_write_config_dword(dev, 0x48,
                                (0 << 14)|(0 << 10)|(0 << 8)|(0 << 4)|(0 << 0));
       		         break;
		}
        }
}


static void setup_pci_irq_to_isa_routing(void)
{
	struct pci_dev *dev;
	dev = pci_find_device(PCI_VENDOR_ID_AMD, PCI_DEVICE_ID_AMD_SB768_7443, 0);
	if (dev != NULL) {
		/*
		 * PIRQA -> 5
		 * PIRQB -> 10
		 * PIRQC -> 11
		 * PIRQD -> 3
		 */
		pci_write_config_word(dev, 0x56,
			(0xa << 12)|(0xb << 8)|(0x0 <<4)|(0x5 <<0));
#if 0
		pci_write_config_word(dev, 0x56,
			(3 << 12)|(0xb << 8)|(0xa <<4)|(5 <<0));
#endif
	}
}

static void hide_devices(void)
{
	struct pci_dev *dev;
	dev = pci_find_device(PCI_VENDOR_ID_AMD, PCI_DEVICE_ID_AMD_SB768_7440, 0);
	if (dev) {
		u8 byte;
		pci_read_config_byte(dev, 0x48, &byte);
#if 0
		/* Hide both the ide controller and the usb controller */
		pci_write_config_byte(dev, 0x48, byte | (0<<3)|(1<<2)|(1<<1));
#endif
#if 0
		/* Hide the usb controller */
		pci_write_config_byte(dev, 0x48, byte | (0<<3)|(1<<2)|(0<<1));
#endif
#if 1
		/* Hide no devices */
		pci_write_config_byte(dev, 0x48, byte | (0<<3)|(0<<2)|(0<<1));
#endif
	}
}

void mainboard_cpu_fixup(int cpu)
{
	/*  This is an error, something is locked up  */
	full_reset();
}

void mainboard_fixup(void)
{
	int power_on_after_power_fail;
	int ecc;

	amd768_disable_watchdog();
	lpc_routing_fixup();
	amd768_enable_ioapic();
	amd768_enable_serial_irqs(1, 0, 0);
	enable_reset_port_0xcf9();
	amd768_enable_port92_reset();
	check_cpus();
#if 0
	print_whami();
#endif
	amd768_enable_ide(1,1);
	onboard_scsi_fixup();
	amd768_cpu_reset_sends_init();
	rtc_init(0);
	amd768_decode_stop_grant(DECODE_STPGNT_DATA);
	amd768_posted_memory_write_enable();
	amd768_set_pm_classcode();
	amd768_mouse_sends_irq12();
	setup_pci_irq_to_isa_routing();
	amd76x_setup_pci_arbiter();
	isa_dma_init();

	power_on_after_power_fail = MAINBOARD_POWER_ON_AFTER_POWER_FAIL;
	get_option(&power_on_after_power_fail, "power_on_after_fail");
	set_power_on_after_power_fail(power_on_after_power_fail);
	
	ecc = ECC_OPTION;
        get_option(&ecc, "ECC_memory");
        set_ecc(ecc);

#if 0
	amd768_usb_setup();
#endif
#if 0
	hide_devices();
#endif
}


void final_mainboard_fixup(void)
{
#if 0
	enable_ide_devices();
#endif
}

void hard_reset(void)
{
	amd768_hard_reset();
}
