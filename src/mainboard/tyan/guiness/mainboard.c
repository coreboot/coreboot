#include <pci_ids.h>
#include <pci.h>
#include <arch/io.h>
#include <printk.h>
#include <delay.h>
#include <part/mainboard.h>
#include <part/hard_reset.h>
#include <arch/smp/mpspec.h>

unsigned long initial_apicid[MAX_CPUS] =
{
	0, 1
};

static void lpc_routing_fixup(void)
{
	struct pci_dev *dev;
	dev = pci_find_device(PCI_VENDOR_ID_AMD, PCI_DEVICE_ID_AMD_VIPER_7410,0);
	if (dev != NULL) {
#if 0
		/* Send ACPI, keyboard controller,
		 * FDC2, FDC1, ECP
		 * to the LPC bus
		 */
		pci_write_config_byte(dev, 0x51, (1<<7)|(1<<6)|(1<<5)|(1<<4)|(1<<3)|(1));
#else
		/* Send keyboard controller,
		 * FDC1, ECP
		 * to the LPC bus
		 * Send APCI ports 0x62 & 0x66 to the ISA bus
		 */
		pci_write_config_byte(dev, 0x51, (0<<7)|(1<<6)|(0<<5)|(1<<4)|(0<<3)|(1));
#endif
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
		
	}
}


static void enable_ioapic(void)
{
	struct pci_dev *dev;
	dev = pci_find_device(PCI_VENDOR_ID_AMD, PCI_DEVICE_ID_AMD_VIPER_7410,0);
	if (dev != NULL) {
		/* Enable the ioapic */
		pci_write_config_byte(dev, 0x4b, (0 << 3)|(0 <<2)|(0 << 1)|(1<< 0));
	}
	
}

static void serial_irq_fixup(void)
{
	struct pci_dev *dev;
	dev = pci_find_device(PCI_VENDOR_ID_AMD, PCI_DEVICE_ID_AMD_VIPER_7413, 0);	
	if (dev != NULL) {
		/* Setup serial irq's for the LPC bus. */
		pci_write_config_byte(dev, 0x4a, (1<<6)|(0<<2)|(0<<0));
	}
}

static void mouse_sends_irq12(void)
{
	struct pci_dev *dev;
	dev = pci_find_device(PCI_VENDOR_ID_AMD, PCI_DEVICE_ID_AMD_VIPER_7413, 0);	
	if (dev != NULL) {
		unsigned short word;
		/* Setup so irq12 is sent by the ps2 mouse port. */
		pci_read_config_word(dev, 0x46, &word);
		pci_write_config_word(dev, 0x46, word | (1<<9));
	}
}

static void disable_watchdog(void)
{
	struct pci_dev *dev;
	dev = pci_find_device(PCI_VENDOR_ID_AMD, PCI_DEVICE_ID_AMD_VIPER_7413, 0);
	if (dev != NULL) {
		unsigned char byte;
		/* Disable the watchdog timer */
		pci_read_config_byte(dev, 0x41, &byte);
		pci_write_config_byte(dev, 0x41, byte | (1<<6)|(1<<2));
	}
}

static void enable_reset_port_0xcf9(void)
{
	/* FIXME this code is correct but the bit isn't getting set on my test machine. */
	struct pci_dev *dev;
	dev = pci_find_device(PCI_VENDOR_ID_AMD, PCI_DEVICE_ID_AMD_VIPER_7413, 0);
	if (dev != NULL) {
		unsigned char byte;
		/* Enable reset port 0xcf9 */
		pci_read_config_byte(dev, 0x41, &byte);		
		pci_write_config_byte(dev, 0x41, byte | (1<<5));
	}
}

static void enable_port92_reset(void)
{
	struct pci_dev *dev;
	dev = pci_find_device(PCI_VENDOR_ID_AMD, PCI_DEVICE_ID_AMD_VIPER_7410, 0);
	if (dev != NULL) {
		unsigned char byte;
		/* Enable reset using port 0x92 */
		pci_read_config_byte(dev, 0x41, &byte);		
		pci_write_config_byte(dev, 0x41, byte | (1<<5));
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


#define IDE_ERR_ICRC	0x80	/* ATA Ultra DMA bad CRC */
#define IDE_ERR_BBK	0x80	/* ATA bad block */
#define IDE_ERR_UNC	0x40	/* ATA uncorrected error */
#define IDE_ERR_MC	0x20	/* ATA media change */
#define IDE_ERR_IDNF	0x10	/* ATA id not found */
#define IDE_ERR_MCR	0x08	/* ATA media change request */
#define IDE_ERR_ABRT	0x04	/* ATA command aborted */
#define IDE_ERR_NTK0	0x02	/* ATA track 0 not found */
#define IDE_ERR_NDAM	0x01	/* ATA address mark not found */

#define IDE_STATUS_BSY	0x80	/* busy */
#define IDE_STATUS_RDY	0x40	/* ready */
#define IDE_STATUS_DF	0x20	/* device fault */
#define IDE_STATUS_WFT	0x20	/* write fault (old name) */
#define IDE_STATUS_SKC	0x10	/* seek complete */
#define IDE_STATUS_DRQ	0x08	/* data request */
#define IDE_STATUS_CORR	0x04	/* corrected */
#define IDE_STATUS_IDX	0x02	/* index */
#define IDE_STATUS_ERR	0x01	/* error (ATA) */
#define IDE_STATUS_CHK	0x01	/* check (ATAPI) */

#define IDE_CTRL_HD15	0x08	/* bit should always be set to one */
#define IDE_CTRL_SRST	0x04	/* soft reset */
#define IDE_CTRL_NIEN	0x02	/* disable interrupts */

/* Most mandtory and optional ATA commands (from ATA-3), */

#define IDE_CMD_CFA_ERASE_SECTORS            0xC0
#define IDE_CMD_CFA_REQUEST_EXT_ERR_CODE     0x03
#define IDE_CMD_CFA_TRANSLATE_SECTOR         0x87
#define IDE_CMD_CFA_WRITE_MULTIPLE_WO_ERASE  0xCD
#define IDE_CMD_CFA_WRITE_SECTORS_WO_ERASE   0x38
#define IDE_CMD_CHECK_POWER_MODE1            0xE5
#define IDE_CMD_CHECK_POWER_MODE2            0x98
#define IDE_CMD_DEVICE_RESET                 0x08
#define IDE_CMD_EXECUTE_DEVICE_DIAGNOSTIC    0x90
#define IDE_CMD_FLUSH_CACHE                  0xE7
#define IDE_CMD_FORMAT_TRACK                 0x50
#define IDE_CMD_IDENTIFY_DEVICE              0xEC
#define IDE_CMD_IDENTIFY_DEVICE_PACKET       0xA1
#define IDE_CMD_IDENTIFY_PACKET_DEVICE       0xA1
#define IDE_CMD_IDLE1                        0xE3
#define IDE_CMD_IDLE2                        0x97
#define IDE_CMD_IDLE_IMMEDIATE1              0xE1
#define IDE_CMD_IDLE_IMMEDIATE2              0x95
#define IDE_CMD_INITIALIZE_DRIVE_PARAMETERS  0x91
#define IDE_CMD_INITIALIZE_DEVICE_PARAMETERS 0x91
#define IDE_CMD_NOP                          0x00
#define IDE_CMD_PACKET                       0xA0
#define IDE_CMD_READ_BUFFER                  0xE4
#define IDE_CMD_READ_DMA                     0xC8
#define IDE_CMD_READ_DMA_QUEUED              0xC7
#define IDE_CMD_READ_MULTIPLE                0xC4
#define IDE_CMD_READ_SECTORS                 0x20
#define IDE_CMD_READ_VERIFY_SECTORS          0x40
#define IDE_CMD_RECALIBRATE                  0x10
#define IDE_CMD_SEEK                         0x70
#define IDE_CMD_SET_FEATURES                 0xEF
#define IDE_CMD_SET_MULTIPLE_MODE            0xC6
#define IDE_CMD_SLEEP1                       0xE6
#define IDE_CMD_SLEEP2                       0x99
#define IDE_CMD_STANDBY1                     0xE2
#define IDE_CMD_STANDBY2                     0x96
#define IDE_CMD_STANDBY_IMMEDIATE1           0xE0
#define IDE_CMD_STANDBY_IMMEDIATE2           0x94
#define IDE_CMD_WRITE_BUFFER                 0xE8
#define IDE_CMD_WRITE_DMA                    0xCA
#define IDE_CMD_WRITE_DMA_QUEUED             0xCC
#define IDE_CMD_WRITE_MULTIPLE               0xC5
#define IDE_CMD_WRITE_SECTORS                0x30
#define IDE_CMD_WRITE_VERIFY                 0x3C

static void reset_ide_controller(unsigned ide_ctl, unsigned ide_data)
{
	unsigned char status, error;
	printk_spew("resetting ide controller at 0x%04x\n", ide_ctl);
	outb(IDE_CTRL_HD15 | IDE_CTRL_SRST | IDE_CTRL_NIEN, ide_ctl); 
	printk_spew("1\n");
	udelay(25);
	printk_spew("2\n");
	outb(IDE_CTRL_HD15 | IDE_CTRL_NIEN, ide_ctl); 
	printk_spew("3\n");
	do {
		mdelay(2);
		printk_spew("4\n");
		error = inb_p(ide_data+1);
		status = inb_p(ide_data+7);
		printk_spew("error = 0x%02x status = 0x%02x\n", error, status);
	} while(status & IDE_STATUS_BSY);
	printk_spew("reset ide controller at 0x%04x\n", ide_ctl);

}

static void setup_ide_devices(void)
{
	struct pci_dev *dev;
	dev = pci_find_device(PCI_VENDOR_ID_AMD, PCI_DEVICE_ID_AMD_VIPER_7411, 0);
	if (dev != NULL) {
		/* Enable ide devices so the linux ide driver will work */
		u16 word;
		pci_read_config_word(dev, 0x40, &word);
		/* enable:
		 * both devices
		 * both devices posted write buffers
		 */
		pci_write_config_word(dev, 0x40, word |(1<<14)|(1<<12)|(1<<1)|(1<<0));

#if 1
		/* Setup the appropriate ide timing */
		pci_write_config_dword(dev, 0x48, 0x5e5e5e5e);
		pci_write_config_byte(dev, 0x4c, 0xaa);
#endif


		
#if 0
		/* Enable native ide native mode, at the legacy addresses */
		/* This would be cool if I could receive an interrupt from
		 * the ide controller.
		 */
	{
		u32 class_revision;

		pci_read_config_dword(dev, PCI_CLASS_REVISION, &class_revision);
		pci_write_config_dword(dev, PCI_CLASS_REVISION,
			class_revision | ( 1 << 8) | (1 << 10));
		pci_write_config_dword(dev, PCI_BASE_ADDRESS_0, 
			0x1f0 | PCI_BASE_ADDRESS_SPACE_IO);
		pci_write_config_dword(dev, PCI_BASE_ADDRESS_1,
			0x3f6 | PCI_BASE_ADDRESS_SPACE_IO);
		pci_write_config_dword(dev, PCI_BASE_ADDRESS_2,
			0x170 | PCI_BASE_ADDRESS_SPACE_IO);
		pci_write_config_dword(dev, PCI_BASE_ADDRESS_3,
			0x376 | PCI_BASE_ADDRESS_SPACE_IO);
	}
#endif
	}
	dev = pci_find_device(PCI_VENDOR_ID_AMD, PCI_DEVICE_ID_AMD_VIPER_7410, 0);
	if (dev != NULL) {
		unsigned char byte;
		/* optimize ide dma acknowledge */
		pci_read_config_byte(dev, 0x4a, &byte);
		pci_write_config_byte(dev, 0x4a, byte & ~(1<<7));
	}
}

static void enable_ide_devices(void)
{
	u32 ide0_data = 0, ide0_ctl = 0, ide1_data = 0, ide1_ctl = 0, dma_base = 0;
	struct pci_dev *dev;
	dev = pci_find_device(PCI_VENDOR_ID_AMD, PCI_DEVICE_ID_AMD_VIPER_7411, 0);
	if (dev != NULL) {
		ide0_data = 0x1f0;
		ide0_ctl = 0x3f6;
		ide1_data = 0x170;
		ide1_ctl = 0x376;
		pci_read_config_dword(dev, PCI_BASE_ADDRESS_4, &dma_base);
		if (dma_base & PCI_BASE_ADDRESS_SPACE_IO) {
			dma_base &= ~PCI_BASE_ADDRESS_SPACE_IO;
		} else {
			dma_base = 0;
		}
	}
#if 0
	/* IDE resets take forever, plus my reset code looks broken... */
	if (ide0_data && ide0_ctl) {
		reset_ide_controller(ide0_ctl,ide0_data + 7);
	}
	if (ide0_data && ide1_ctl) {
		reset_ide_controller(ide1_ctl, ide1_data + 7);
	}
#endif
	if (dma_base) {
		printk_debug("ide dma_base = 0x%08lx\n", dma_base);
#if 1
		/* Disable DMA */
		outb_p(inb_p(dma_base + 2) & ~((1<<5)|(1<<6)), dma_base +2);
		outb_p(inb_p(dma_base+8 + 2) & ~((1<<5)|(1<<6)), dma_base+8 +2);
#endif
#if 0
		/* Enable DMA */
		outb_p(inb_p(dma_base + 2) | ((1<<5)|(1<<6)), dma_base +2);
		outb_p(inb_p(dma_base+8 + 2) | ((1<<5)|(1<<6)), dma_base+8 +2);
#endif
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
	pci_write_config_word(pdev, PCI_SUBSYSTEM_ID, 0x2462);
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


static void cpu_reset_sends_init(void)
{
	struct pci_dev *dev;
	dev = pci_find_device(PCI_VENDOR_ID_AMD, PCI_DEVICE_ID_AMD_VIPER_7410, 0);
	if (dev != NULL) {
		unsigned char byte;
		pci_read_config_byte(dev, 0x47, &byte);
		pci_write_config_byte(dev, 0x47, byte | (1<<7));
	}

}

static void decode_stop_grant_fixup(void)
{
	struct pci_dev *dev;
	dev = pci_find_device(PCI_VENDOR_ID_AMD, PCI_DEVICE_ID_AMD_VIPER_7413, 0);
	if (dev != NULL) {
		unsigned char byte;
		pci_read_config_byte(dev, 0x41, &byte);
		pci_write_config_byte(dev, 0x41, byte | (1<<1));
	}

}

static void pm_controller_classcode_fixup(void)
{
	struct pci_dev *dev;
	dev = pci_find_device(PCI_VENDOR_ID_AMD, PCI_DEVICE_ID_AMD_VIPER_7413, 0);
	if (dev != NULL) {
		pci_write_config_dword(dev, 0x60, 0x06800000);
	}

}

#if 0
static void set_power_on_after_power_fail(int enable)
{
	/* FIXME: This may be part of the picture but it isn't
	 * the whole story :(
	 */
	struct pci_dev *dev;
	dev = pci_find_device(PCI_VENDOR_ID_AMD, PCI_DEVICE_ID_AMD_VIPER_7413, 0);
	if (dev != NULL) {
		unsigned char byte;
		pci_read_config_byte(dev, 0x43, &byte);
		if (enable) { 
			byte &= ~(1<<6);
		}
		else {
			byte |= (1<<6);
		}
		pci_write_config_byte(dev, 0x43, byte);
	}
}
#endif

static void posted_memory_write_enable(void)
{
	struct pci_dev *dev;
	dev = pci_find_device(PCI_VENDOR_ID_AMD, PCI_DEVICE_ID_AMD_VIPER_7410, 0);
	if (dev != NULL) {
		unsigned char byte;
		pci_read_config_byte(dev, 0x46, &byte);
		pci_write_config_byte(dev, 0x46, byte | (1<<0));
	}

}

static void setup_pci_irq_to_isa_routing(void)
{
	struct pci_dev *dev;
	dev = pci_find_device(PCI_VENDOR_ID_AMD, PCI_DEVICE_ID_AMD_VIPER_7413, 0);
	if (dev != NULL) {
		/*
		 * PIRQA -> 5
		 * PIRQB -> 10
		 * PIRQC -> 11
		 * PIRQD -> 3
		 */
		pci_write_config_word(dev, 0x56,(3 << 12)|(0xb << 8)|(0xa <<4)|(5 <<0));
	}
}

static void setup_pci_arbiter(void)
{
	struct pci_dev *dev;
	dev = pci_find_device(PCI_VENDOR_ID_AMD, PCI_DEVICE_ID_AMD_FE_GATE_700C, 0);
	if (dev) {
		/* Enable:
		 * PCI parking
		 * memory prefetching
		 * EV6 mode
		 * Enable power management registers
		 * The southbridge lock
		 * Read data error disable
		 * PCI retries
		 * AGP retries
		 * AGP chaining
		 * PCI chaining
		 */
		pci_write_config_dword(dev, 0x84, 
			(0<<24)
			|(1<<23)
			|(1<<17)|(1<<16)
			|(0<<15)|(1<<14)|(1<<13)|(1<<12)
			|(0<<11)|(0<<10)|(0<<9)|(0<<8)
			|(1<<7)|(0<<6)|(0<<5)|(1<<4)
			|(0<<3)|(1<<2)|(1<<1)|(1<<0));
	}
}

static void usb_setup(void)
{
	/* FIXME this is untested incomplete implementation. */
	struct pci_dev *dev;
	dev = pci_find_device(PCI_VENDOR_ID_AMD, PCI_DEVICE_ID_AMD_VIPER_7414, 0);
	if (dev) {
		u32 cmd;
		pci_read_config_dword(dev, PCI_COMMAND, &cmd);
		pci_write_config_dword(dev, PCI_COMMAND, 
			cmd | PCI_COMMAND_IO | PCI_COMMAND_MEMORY | 
			PCI_COMMAND_MASTER | PCI_COMMAND_INVALIDATE);
	}
}

static void hide_devices(void)
{
	struct pci_dev *dev;
	dev = pci_find_device(PCI_VENDOR_ID_AMD, PCI_DEVICE_ID_AMD_VIPER_7410, 0);
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

void mainboard_fixup(void)
{
	disable_watchdog();
	lpc_routing_fixup();
	enable_ioapic();
	serial_irq_fixup();
	enable_reset_port_0xcf9();
	enable_port92_reset();
#if 0
	print_whami();
#endif
	setup_ide_devices();
	onboard_scsi_fixup();
	cpu_reset_sends_init();
	rtc_init();
	decode_stop_grant_fixup();
	posted_memory_write_enable();
	pm_controller_classcode_fixup();
#if 1
	mouse_sends_irq12();
#endif
	setup_pci_irq_to_isa_routing();
	setup_pci_arbiter();
	isa_dma_init();
#if 0
	usb_setup();
#endif
#if 0
	hide_devices();
#endif
}


void final_mainboard_fixup(void)
{
#if 1
	enable_ide_devices();
#endif
}

void hard_reset(void)
{
	pci_set_method();
	/* Allow the watchdog timer to reboot us, and enable 0xcf9 */
	pcibios_write_config_byte(0, (AMD766_DEV >> 8) | 3, 0x41, (1<<5)|(1<<1));
	/* Try rebooting though port 0xcf9 */
	outb((0<<3)|(1<<2)|(1<<1), 0xcf9);
	return;
}
