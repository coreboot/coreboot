/*
 *	$Id$
 *
 *	PCI defines and function prototypes
 *	Copyright 1994, Drew Eckhardt
 *	Copyright 1997--1999 Martin Mares <mj@atrey.karlin.mff.cuni.cz>
 *
 *	For more information, please consult the following manuals (look at
 *	http://www.pcisig.com/ for how to get them):
 *
 *	PCI BIOS Specification
 *	PCI Local Bus Specification
 *	PCI to PCI Bridge Specification
 *	PCI System Design Guide
 */

#ifndef PCI_H
#define PCI_H

/*
 * Under PCI, each device has 256 bytes of configuration address space,
 * of which the first 64 bytes are standardized as follows:
 */
#define PCI_VENDOR_ID		0x00	/* 16 bits */
#define PCI_DEVICE_ID		0x02	/* 16 bits */
#define PCI_COMMAND		0x04	/* 16 bits */
#define  PCI_COMMAND_IO		0x1	/* Enable response in I/O space */
#define  PCI_COMMAND_MEMORY	0x2	/* Enable response in Memory space */
#define  PCI_COMMAND_MASTER	0x4	/* Enable bus mastering */
#define  PCI_COMMAND_SPECIAL	0x8	/* Enable response to special cycles */
#define  PCI_COMMAND_INVALIDATE	0x10	/* Use memory write and invalidate */
#define  PCI_COMMAND_VGA_PALETTE 0x20	/* Enable palette snooping */
#define  PCI_COMMAND_PARITY	0x40	/* Enable parity checking */
#define  PCI_COMMAND_WAIT 	0x80	/* Enable address/data stepping */
#define  PCI_COMMAND_SERR	0x100	/* Enable SERR */
#define  PCI_COMMAND_FAST_BACK	0x200	/* Enable back-to-back writes */

#define PCI_STATUS		0x06	/* 16 bits */
#define  PCI_STATUS_CAP_LIST	0x10	/* Support Capability List */
#define  PCI_STATUS_66MHZ	0x20	/* Support 66 Mhz PCI 2.1 bus */
#define  PCI_STATUS_UDF		0x40	/* Support User Definable Features [obsolete] */
#define  PCI_STATUS_FAST_BACK	0x80	/* Accept fast-back to back */
#define  PCI_STATUS_PARITY	0x100	/* Detected parity error */
#define  PCI_STATUS_DEVSEL_MASK	0x600	/* DEVSEL timing */
#define  PCI_STATUS_DEVSEL_FAST	0x000	
#define  PCI_STATUS_DEVSEL_MEDIUM 0x200
#define  PCI_STATUS_DEVSEL_SLOW 0x400
#define  PCI_STATUS_SIG_TARGET_ABORT 0x800 /* Set on target abort */
#define  PCI_STATUS_REC_TARGET_ABORT 0x1000 /* Master ack of " */
#define  PCI_STATUS_REC_MASTER_ABORT 0x2000 /* Set on master abort */
#define  PCI_STATUS_SIG_SYSTEM_ERROR 0x4000 /* Set when we drive SERR */
#define  PCI_STATUS_DETECTED_PARITY 0x8000 /* Set on parity error */

#define PCI_CLASS_REVISION	0x08	/* High 24 bits are class, low 8
					   revision */
#define PCI_REVISION_ID         0x08    /* Revision ID */
#define PCI_CLASS_PROG          0x09    /* Reg. Level Programming Interface */
#define PCI_CLASS_DEVICE        0x0a    /* Device class */

#define PCI_CACHE_LINE_SIZE	0x0c	/* 8 bits */
#define PCI_LATENCY_TIMER	0x0d	/* 8 bits */
#define PCI_HEADER_TYPE		0x0e	/* 8 bits */
#define  PCI_HEADER_TYPE_NORMAL	0
#define  PCI_HEADER_TYPE_BRIDGE 1
#define  PCI_HEADER_TYPE_CARDBUS 2

#define PCI_BIST		0x0f	/* 8 bits */
#define PCI_BIST_CODE_MASK	0x0f	/* Return result */
#define PCI_BIST_START		0x40	/* 1 to start BIST, 2 secs or less */
#define PCI_BIST_CAPABLE	0x80	/* 1 if BIST capable */

/*
 * Base addresses specify locations in memory or I/O space.
 * Decoded size can be determined by writing a value of 
 * 0xffffffff to the register, and reading it back.  Only 
 * 1 bits are decoded.
 */
#define PCI_BASE_ADDRESS_0	0x10	/* 32 bits */
#define PCI_BASE_ADDRESS_1	0x14	/* 32 bits [htype 0,1 only] */
#define PCI_BASE_ADDRESS_2	0x18	/* 32 bits [htype 0 only] */
#define PCI_BASE_ADDRESS_3	0x1c	/* 32 bits */
#define PCI_BASE_ADDRESS_4	0x20	/* 32 bits */
#define PCI_BASE_ADDRESS_5	0x24	/* 32 bits */
#define  PCI_BASE_ADDRESS_SPACE	0x01	/* 0 = memory, 1 = I/O */
#define  PCI_BASE_ADDRESS_SPACE_IO 0x01
#define  PCI_BASE_ADDRESS_SPACE_MEMORY 0x00
#define  PCI_BASE_ADDRESS_MEM_TYPE_MASK 0x06
#define  PCI_BASE_ADDRESS_MEM_TYPE_32	0x00	/* 32 bit address */
#define  PCI_BASE_ADDRESS_MEM_TYPE_1M	0x02	/* Below 1M [obsolete] */
#define  PCI_BASE_ADDRESS_MEM_TYPE_64	0x04	/* 64 bit address */
#define  PCI_BASE_ADDRESS_MEM_PREFETCH	0x08	/* prefetchable? */
#define  PCI_BASE_ADDRESS_MEM_MASK	(~0x0fUL)
#define  PCI_BASE_ADDRESS_IO_MASK	(~0x03UL)
/* bit 1 is reserved if address_space = 1 */

/* Header type 0 (normal devices) */
#define PCI_CARDBUS_CIS		0x28
#define PCI_SUBSYSTEM_VENDOR_ID	0x2c
#define PCI_SUBSYSTEM_ID	0x2e  
#define PCI_ROM_ADDRESS		0x30	/* Bits 31..11 are address, 10..1 reserved */
#define  PCI_ROM_ADDRESS_ENABLE	0x01
#define PCI_ROM_ADDRESS_MASK	(~0x7ffUL)

#define PCI_CAPABILITY_LIST	0x34	/* Offset of first capability list entry */

/* 0x35-0x3b are reserved */
#define PCI_INTERRUPT_LINE	0x3c	/* 8 bits */
#define PCI_INTERRUPT_PIN	0x3d	/* 8 bits */
#define PCI_MIN_GNT		0x3e	/* 8 bits */
#define PCI_MAX_LAT		0x3f	/* 8 bits */

/* Header type 1 (PCI-to-PCI bridges) */
#define PCI_PRIMARY_BUS		0x18	/* Primary bus number */
#define PCI_SECONDARY_BUS	0x19	/* Secondary bus number */
#define PCI_SUBORDINATE_BUS	0x1a	/* Highest bus number behind the bridge */
#define PCI_SEC_LATENCY_TIMER	0x1b	/* Latency timer for secondary interface */
#define PCI_IO_BASE		0x1c	/* I/O range behind the bridge */
#define PCI_IO_LIMIT		0x1d
#define  PCI_IO_RANGE_TYPE_MASK	0x0f	/* I/O bridging type */
#define  PCI_IO_RANGE_TYPE_16	0x00
#define  PCI_IO_RANGE_TYPE_32	0x01
#define  PCI_IO_RANGE_MASK	~0x0f
#define PCI_SEC_STATUS		0x1e	/* Secondary status register, only bit 14 used */
#define PCI_MEMORY_BASE		0x20	/* Memory range behind */
#define PCI_MEMORY_LIMIT	0x22
#define  PCI_MEMORY_RANGE_TYPE_MASK 0x0f
#define  PCI_MEMORY_RANGE_MASK	~0x0f
#define PCI_PREF_MEMORY_BASE	0x24	/* Prefetchable memory range behind */
#define PCI_PREF_MEMORY_LIMIT	0x26
#define  PCI_PREF_RANGE_TYPE_MASK 0x0f
#define  PCI_PREF_RANGE_TYPE_32	0x00
#define  PCI_PREF_RANGE_TYPE_64	0x01
#define  PCI_PREF_RANGE_MASK	~0x0f
#define PCI_PREF_BASE_UPPER32	0x28	/* Upper half of prefetchable memory range */
#define PCI_PREF_LIMIT_UPPER32	0x2c
#define PCI_IO_BASE_UPPER16	0x30	/* Upper half of I/O addresses */
#define PCI_IO_LIMIT_UPPER16	0x32
/* 0x34 same as for htype 0 */
/* 0x35-0x3b is reserved */
#define PCI_ROM_ADDRESS1	0x38	/* Same as PCI_ROM_ADDRESS, but for htype 1 */
/* 0x3c-0x3d are same as for htype 0 */
#define PCI_BRIDGE_CONTROL	0x3e
#define  PCI_BRIDGE_CTL_PARITY	0x01	/* Enable parity detection on secondary interface */
#define  PCI_BRIDGE_CTL_SERR	0x02	/* The same for SERR forwarding */
#define  PCI_BRIDGE_CTL_NO_ISA	0x04	/* Disable bridging of ISA ports */
#define  PCI_BRIDGE_CTL_VGA	0x08	/* Forward VGA addresses */
#define  PCI_BRIDGE_CTL_MASTER_ABORT 0x20  /* Report master aborts */
#define  PCI_BRIDGE_CTL_BUS_RESET 0x40	/* Secondary bus reset */
#define  PCI_BRIDGE_CTL_FAST_BACK 0x80	/* Fast Back2Back enabled on secondary interface */

/* Header type 2 (CardBus bridges) */
#define PCI_CB_CAPABILITY_LIST	0x14
/* 0x15 reserved */
#define PCI_CB_SEC_STATUS	0x16	/* Secondary status */
#define PCI_CB_PRIMARY_BUS	0x18	/* PCI bus number */
#define PCI_CB_CARD_BUS		0x19	/* CardBus bus number */
#define PCI_CB_SUBORDINATE_BUS	0x1a	/* Subordinate bus number */
#define PCI_CB_LATENCY_TIMER	0x1b	/* CardBus latency timer */
#define PCI_CB_MEMORY_BASE_0	0x1c
#define PCI_CB_MEMORY_LIMIT_0	0x20
#define PCI_CB_MEMORY_BASE_1	0x24
#define PCI_CB_MEMORY_LIMIT_1	0x28
#define PCI_CB_IO_BASE_0	0x2c
#define PCI_CB_IO_BASE_0_HI	0x2e
#define PCI_CB_IO_LIMIT_0	0x30
#define PCI_CB_IO_LIMIT_0_HI	0x32
#define PCI_CB_IO_BASE_1	0x34
#define PCI_CB_IO_BASE_1_HI	0x36
#define PCI_CB_IO_LIMIT_1	0x38
#define PCI_CB_IO_LIMIT_1_HI	0x3a
#define  PCI_CB_IO_RANGE_MASK	~0x03
/* 0x3c-0x3d are same as for htype 0 */
#define PCI_CB_BRIDGE_CONTROL	0x3e
#define  PCI_CB_BRIDGE_CTL_PARITY	0x01	/* Similar to standard bridge control register */
#define  PCI_CB_BRIDGE_CTL_SERR		0x02
#define  PCI_CB_BRIDGE_CTL_ISA		0x04
#define  PCI_CB_BRIDGE_CTL_VGA		0x08
#define  PCI_CB_BRIDGE_CTL_MASTER_ABORT	0x20
#define  PCI_CB_BRIDGE_CTL_CB_RESET	0x40	/* CardBus reset */
#define  PCI_CB_BRIDGE_CTL_16BIT_INT	0x80	/* Enable interrupt for 16-bit cards */
#define  PCI_CB_BRIDGE_CTL_PREFETCH_MEM0 0x100	/* Prefetch enable for both memory regions */
#define  PCI_CB_BRIDGE_CTL_PREFETCH_MEM1 0x200
#define  PCI_CB_BRIDGE_CTL_POST_WRITES	0x400
#define PCI_CB_SUBSYSTEM_VENDOR_ID 0x40
#define PCI_CB_SUBSYSTEM_ID	0x42
#define PCI_CB_LEGACY_MODE_BASE	0x44	/* 16-bit PC Card legacy mode base address (ExCa) */
/* 0x48-0x7f reserved */

/* Capability lists */

#define PCI_CAP_LIST_ID		0	/* Capability ID */
#define  PCI_CAP_ID_PM		0x01	/* Power Management */
#define  PCI_CAP_ID_AGP		0x02	/* Accelerated Graphics Port */
#define  PCI_CAP_ID_VPD		0x03	/* Vital Product Data */
#define  PCI_CAP_ID_SLOTID	0x04	/* Slot Identification */
#define  PCI_CAP_ID_MSI		0x05	/* Message Signalled Interrupts */
#define  PCI_CAP_ID_CHSWP	0x06	/* CompactPCI HotSwap */
#define PCI_CAP_LIST_NEXT	1	/* Next capability in the list */
#define PCI_CAP_FLAGS		2	/* Capability defined flags (16 bits) */
#define PCI_CAP_SIZEOF		4

/* Power Management Registers */

#define  PCI_PM_CAP_VER_MASK	0x0007	/* Version */
#define  PCI_PM_CAP_PME_CLOCK	0x0008	/* PME clock required */
#define  PCI_PM_CAP_AUX_POWER	0x0010	/* Auxilliary power support */
#define  PCI_PM_CAP_DSI		0x0020	/* Device specific initialization */
#define  PCI_PM_CAP_D1		0x0200	/* D1 power state support */
#define  PCI_PM_CAP_D2		0x0400	/* D2 power state support */
#define  PCI_PM_CAP_PME		0x0800	/* PME pin supported */
#define PCI_PM_CTRL		4	/* PM control and status register */
#define  PCI_PM_CTRL_STATE_MASK	0x0003	/* Current power state (D0 to D3) */
#define  PCI_PM_CTRL_PME_ENABLE	0x0100	/* PME pin enable */
#define  PCI_PM_CTRL_DATA_SEL_MASK	0x1e00	/* Data select (??) */
#define  PCI_PM_CTRL_DATA_SCALE_MASK	0x6000	/* Data scale (??) */
#define  PCI_PM_CTRL_PME_STATUS	0x8000	/* PME pin status */
#define PCI_PM_PPB_EXTENSIONS	6	/* PPB support extensions (??) */
#define  PCI_PM_PPB_B2_B3	0x40	/* Stop clock when in D3hot (??) */
#define  PCI_PM_BPCC_ENABLE	0x80	/* Bus power/clock control enable (??) */
#define PCI_PM_DATA_REGISTER	7	/* (??) */
#define PCI_PM_SIZEOF		8

/* AGP registers */

#define PCI_AGP_VERSION		2	/* BCD version number */
#define PCI_AGP_RFU		3	/* Rest of capability flags */
#define PCI_AGP_STATUS		4	/* Status register */
#define  PCI_AGP_STATUS_RQ_MASK	0xff000000	/* Maximum number of requests - 1 */
#define  PCI_AGP_STATUS_SBA	0x0200	/* Sideband addressing supported */
#define  PCI_AGP_STATUS_64BIT	0x0020	/* 64-bit addressing supported */
#define  PCI_AGP_STATUS_FW	0x0010	/* FW transfers supported */
#define  PCI_AGP_STATUS_RATE4	0x0004	/* 4x transfer rate supported */
#define  PCI_AGP_STATUS_RATE2	0x0002	/* 2x transfer rate supported */
#define  PCI_AGP_STATUS_RATE1	0x0001	/* 1x transfer rate supported */
#define PCI_AGP_COMMAND		8	/* Control register */
#define  PCI_AGP_COMMAND_RQ_MASK 0xff000000  /* Master: Maximum number of requests */
#define  PCI_AGP_COMMAND_SBA	0x0200	/* Sideband addressing enabled */
#define  PCI_AGP_COMMAND_AGP	0x0100	/* Allow processing of AGP transactions */
#define  PCI_AGP_COMMAND_64BIT	0x0020 	/* Allow processing of 64-bit addresses */
#define  PCI_AGP_COMMAND_FW	0x0010 	/* Force FW transfers */
#define  PCI_AGP_COMMAND_RATE4	0x0004	/* Use 4x rate */
#define  PCI_AGP_COMMAND_RATE2	0x0002	/* Use 4x rate */
#define  PCI_AGP_COMMAND_RATE1	0x0001	/* Use 4x rate */
#define PCI_AGP_SIZEOF		12

/* Slot Identification */

#define PCI_SID_ESR		2	/* Expansion Slot Register */
#define  PCI_SID_ESR_NSLOTS	0x1f	/* Number of expansion slots available */
#define  PCI_SID_ESR_FIC	0x20	/* First In Chassis Flag */
#define PCI_SID_CHASSIS_NR	3	/* Chassis Number */

/* Message Signalled Interrupts registers */

#define PCI_MSI_FLAGS		2	/* Various flags */
#define  PCI_MSI_FLAGS_64BIT	0x80	/* 64-bit addresses allowed */
#define  PCI_MSI_FLAGS_QSIZE	0x70	/* Message queue size configured */
#define  PCI_MSI_FLAGS_QMASK	0x0e	/* Maximum queue size available */
#define  PCI_MSI_FLAGS_ENABLE	0x01	/* MSI feature enabled */
#define PCI_MSI_RFU		3	/* Rest of capability flags */
#define PCI_MSI_ADDRESS_LO	4	/* Lower 32 bits */
#define PCI_MSI_ADDRESS_HI	8	/* Upper 32 bits (if PCI_MSI_FLAGS_64BIT set) */
#define PCI_MSI_DATA_32		8	/* 16 bits of data for 32-bit devices */
#define PCI_MSI_DATA_64		12	/* 16 bits of data for 64-bit devices */

/*
 * The PCI interface treats multi-function devices as independent
 * devices.  The slot/function address of each device is encoded
 * in a single byte as follows:
 *
 *	7:3 = slot
 *	2:0 = function
 */
#define PCI_DEVFN(slot,func)	((((slot) & 0x1f) << 3) | ((func) & 0x07))
#define PCI_SLOT(devfn)		(((devfn) >> 3) & 0x1f)
#define PCI_FUNC(devfn)		((devfn) & 0x07)


#include <types.h>

/*
 * There is one pci_dev structure for each slot-number/function-number
 * combination:
 */
struct pci_dev {
	struct pci_bus	*bus;		/* bus this device is on */
	struct pci_dev	*sibling;	/* next device on this bus */
	struct pci_dev	*next;		/* chain of all devices */

	void		*sysdata;	/* hook for sys-specific extension */
	struct proc_dir_entry *procent;	/* device entry in /proc/bus/pci */

	unsigned int	devfn;		/* encoded device & function index */
	unsigned short	vendor;
	unsigned short	device;
	unsigned int	class;		/* 3 bytes: (base,sub,prog-if) */
	unsigned int	hdr_type;	/* PCI header type */
	unsigned int	master : 1;	/* set if device is master capable */
  u8 command;
	/*
	 * In theory, the irq level can be read from configuration
	 * space and all would be fine.  However, old PCI chips don't
	 * support these registers and return 0 instead.  For example,
	 * the Vision864-P rev 0 chip can uses INTA, but returns 0 in
	 * the interrupt line and pin registers.  pci_init()
	 * initializes this field with the value at PCI_INTERRUPT_LINE
	 * and it is the job of pcibios_fixup() to change it if
	 * necessary.  The field must not be 0 unless the device
	 * cannot generate interrupts at all.
	 */
	unsigned int	irq;		/* irq generated by this device */

	/* Base registers for this device, can be adjusted by
	 * pcibios_fixup() as necessary.
	 */
	unsigned long	base_address[6];
  unsigned long   size[6];
	unsigned long	rom_address;
};

struct pci_bus {
	struct pci_bus	*parent;	/* parent bus this bridge is on */
	struct pci_bus	*children;	/* chain of P2P bridges on this bus */
	struct pci_bus	*next;		/* chain of all PCI buses */

	struct pci_dev	*self;		/* bridge device as seen by parent */
	struct pci_dev	*devices;	/* devices behind this bridge */

	void		*sysdata;	/* hook for sys-specific extension */
	struct proc_dir_entry *procdir;	/* directory entry in /proc/bus/pci */
	unsigned char	number;		/* bus number */
	unsigned char	primary;	/* number of primary bridge */
	unsigned char	secondary;	/* number of secondary bridge */
	unsigned char	subordinate;	/* max number of subordinate buses */

  unsigned long mem, prefmem, io;       /* amount of mem, prefetch mem, 
					 * and I/O needed for this bridge. 
					 * computed by compute_resources, 
					 * inclusive of all child bridges
					 * and devices 
					 */
  u32 membase, memlimit;
  u32 prefmembase, prefmemlimit;
  u32 iobase, iolimit;
};

extern struct pci_bus	pci_root;	/* root bus */
extern struct pci_dev	*pci_devices;	/* list of all devices */

/*
 * Error values that may be returned by the PCI bios.
 */
#define PCIBIOS_SUCCESSFUL		0x00
#define PCIBIOS_FUNC_NOT_SUPPORTED	0x81
#define PCIBIOS_BAD_VENDOR_ID		0x83
#define PCIBIOS_DEVICE_NOT_FOUND	0x86
#define PCIBIOS_BAD_REGISTER_NUMBER	0x87
#define PCIBIOS_SET_FAILED		0x88
#define PCIBIOS_BUFFER_TOO_SMALL	0x89

/* Low-level architecture-dependent routines */

int pcibios_present (void);
void pcibios_init(void);
void pcibios_fixup(void);
void pcibios_fixup_bus(struct pci_bus *);
char *pcibios_setup (char *str);
int pcibios_read_config_byte (unsigned char bus, unsigned char dev_fn,
			      unsigned char where, u8 *val);
int pcibios_read_config_word (unsigned char bus, unsigned char dev_fn,
			      unsigned char where, u16 *val);
int pcibios_read_config_dword (unsigned char bus, unsigned char dev_fn,
			       unsigned char where, u32 *val);
int pcibios_write_config_byte (unsigned char bus, unsigned char dev_fn,
			       unsigned char where, u8 val);
int pcibios_write_config_word (unsigned char bus, unsigned char dev_fn,
			       unsigned char where, u16 val);
int pcibios_write_config_dword (unsigned char bus, unsigned char dev_fn,
				unsigned char where, u32 val);
int pcibios_debugwrite_config_byte (unsigned char bus, unsigned char dev_fn,
			       unsigned char where, u8 val);
int pcibios_debugwrite_config_word (unsigned char bus, unsigned char dev_fn,
			       unsigned char where, u16 val);
int pcibios_debugwrite_config_dword (unsigned char bus, unsigned char dev_fn,
				unsigned char where, u32 val);

/* Don't use these in new code, use pci_find_... instead */

int pcibios_find_class (unsigned int class_code, unsigned short index, unsigned char *bus, unsigned char *dev_fn);
int pcibios_find_device (unsigned short vendor, unsigned short dev_id,
			 unsigned short index, unsigned char *bus,
			 unsigned char *dev_fn);

/* Generic PCI interface functions */

void pci_init(void);
void pci_setup(char *str, int *ints);
void pci_quirks_init(void);
unsigned int pci_scan_bus(struct pci_bus *bus);
struct pci_bus *pci_scan_peer_bridge(int bus);
void pci_proc_init(void);
void proc_old_pci_init(void);
int get_pci_list(char *buf);
int pci_proc_attach_device(struct pci_dev *dev);
int pci_proc_detach_device(struct pci_dev *dev);

struct pci_dev *pci_find_device (unsigned int vendor, unsigned int device, struct pci_dev *from);
struct pci_dev *pci_find_class (unsigned int class, struct pci_dev *from);
struct pci_dev *pci_find_slot (unsigned int bus, unsigned int devfn);

#define pci_present pcibios_present
int pci_read_config_byte(struct pci_dev *dev, u8 where, u8 *val);
int pci_read_config_word(struct pci_dev *dev, u8 where, u16 *val);
int pci_read_config_dword(struct pci_dev *dev, u8 where, u32 *val);
int pci_write_config_byte(struct pci_dev *dev, u8 where, u8 val);
int pci_write_config_word(struct pci_dev *dev, u8 where, u16 val);
int pci_write_config_dword(struct pci_dev *dev, u8 where, u32 val);
int pci_debugwrite_config_byte(struct pci_dev *dev, u8 where, u8 val);
int pci_debugwrite_config_word(struct pci_dev *dev, u8 where, u16 val);
int pci_debugwrite_config_dword(struct pci_dev *dev, u8 where, u32 val);
void pci_set_master(struct pci_dev *dev);
void pci_set_method(void);
void pci_enumerate(void);
void pci_configure(void);
void pci_enable(void);
void pci_zero_irq_settings(void);

// historical functions ...
void intel_conf_writeb(unsigned long port, unsigned char value);
unsigned char intel_conf_readb(unsigned long port);


#include <kmalloc.h>

// Rounding for boundaries. 
// Due to some chip bugs, go ahead and roung IO to 16
#define IO_ALIGN 16 
#define IO_BRIDGE_ALIGN 4096
#define MEM_ALIGN 4096

#include <pciconf.h>

/* linkages from devices of a type (e.g. superio devices) 
 * to the actual physical PCI device. This type is used in an array of 
 * structs built by NLBConfig.py. We owe this idea to Plan 9.
 */

struct superio;

struct superio_control {
  void (*pre_pci_init)(struct superio *s);
  void (*init)(struct superio *s);
  void (*finishup)(struct superio *s);
  unsigned int defaultport;     /* the defaultport. Can be overridden
				 * by commands in config
				 */
  // This is the print name for debugging
  char *name;
};

struct com_ports {
  unsigned int enable,baud, base, irq;
};

// lpt port description. 
// Note that for many superios you only really need to define the 
// enable. 
struct lpt_ports {
	unsigned int enable, // 1 if this port is enabled
		     mode,   // pp mode
		     base,   // IO base of the parallel port 
                     irq;    // irq
};

struct superio {
  struct superio_control *super; // the ops for the device. 
  unsigned int port; // if non-zero, overrides the default port
  // com ports. This is not done as an array (yet). 
  // We think it's easier to set up from python if it is not an array. 
  struct com_ports com1, com2, com3, com4;
  // DMA, if it exists. 
  struct lpt_ports lpt1, lpt2;
  /* flags for each device type. Unsigned int. */
  // low order bit ALWAYS means enable. Next bit means to enable
  // LPT is in transition, so we leave this here for the moment. 
  unsigned int ide, floppy, lpt;
};

struct southbridge;

struct southbridge_control {
  void (*pre_pci_init)(struct southbridge *s);
  void (*init)(struct southbridge *s);
  void (*finishup)(struct southbridge *s);
  // this is the  vendor and device id
  unsigned int vendor, device;
  // This is the print name for debugging
  char *name;
};

struct southbridge {
  struct pci_dev *device; // the device. 
  struct southbridge_control *southbridge; // the ops for the device. 
  unsigned int devfn;     /* the devfn. 
			   * if devfn is known, the device can be 
			   * configured for PCI discovery. 
			   * this is needed for some devices such as acer m1535
			   */
  /* flags for each device type. Unsigned int. */
  // low order bit ALWAYS means enable. Next bit means to enable
  // DMA, if it exists. 
  unsigned int ide;
};

#endif /* PCI_H */




